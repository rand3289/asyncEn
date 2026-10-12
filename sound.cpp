// A sound-emitting example agent for the asyncEn environment.
// It moves the same way easya.cpp does (random turn/kick every frame) but, in
// addition, beeps a different frequency for each sensor that generates an event.
// Sensor frequencies are spaced logarithmically from 100 Hz up to a few kHz.
//
// Event wire format from the environment (see src/life.cpp Life::event):
//     type,time,sensor\n
//   type:   1 = wave, 2 = collision, 3 = death
//   sensor: 0 .. SENSOR_COUNT-1  (the perimeter sensor that fired; death uses 0)
//
// Action wire format we send back (see src/life.cpp Life::move):
//     time,action,param\n
//   action: 1 = kick left, 2 = kick right, 3 = kick both
//
// g++ sound.cpp -o sound -lSDL2
#include <SDL2/SDL.h>
#include <unistd.h> // getpid(), read()
#include <fcntl.h>  // fcntl()
#include <stdio.h>  // sscanf(), printf(), fflush()
#include <string.h> // memmove()
#include <cstdlib>  // rand(), srand()
#include <cmath>    // sin(), pow(), M_PI
#include <ctime>    // time()
#include <thread>   // sleep_for()
#include <chrono>   // milliseconds
#include <atomic>

// Must match SENSOR_COUNT in src/life.h.  Each sensor maps to its own tone.
static const int SENSOR_COUNT = 16;

static const int    SAMPLE_RATE = 44100;
static const double LOW_HZ      = 100.0;   // frequency of sensor 0
static const double HIGH_HZ     = 4000.0;  // frequency of the last sensor
static const double TONE_SEC    = 0.12;    // length of each beep
static const double AMPLITUDE   = 0.25;    // 0..1, keep well below clipping

// --- Simple additive tone synthesizer driven from the SDL audio callback ----
// A beep is one active sine oscillator with a short linear decay envelope.
// Several sensors can fire close together, so we allow a few voices to overlap.
static const int MAX_VOICES = 8;

struct Voice {
    std::atomic<bool>   active{false};
    double              freq{0.0};
    double              phase{0.0};        // radians
    int                 samplesLeft{0};
    int                 samplesTotal{0};
};

static Voice voices[MAX_VOICES];

// Logarithmic spacing so the pitch step sounds even across the sensor range.
static double sensorFrequency(int sensor) {
    if (SENSOR_COUNT <= 1) return LOW_HZ;
    if (sensor < 0) sensor = 0;
    if (sensor >= SENSOR_COUNT) sensor = SENSOR_COUNT - 1;
    const double t = double(sensor) / double(SENSOR_COUNT - 1); // 0..1
    return LOW_HZ * std::pow(HIGH_HZ / LOW_HZ, t);
}

// Start (or steal) a voice to play the tone for the given sensor.
static void triggerBeep(int sensor) {
    const double freq = sensorFrequency(sensor);
    const int samples = int(TONE_SEC * SAMPLE_RATE);

    // Prefer a free voice; otherwise steal the one with the least time left.
    int chosen = -1;
    int leastLeft = 0x7fffffff;
    for (int v = 0; v < MAX_VOICES; ++v) {
        if (!voices[v].active.load(std::memory_order_acquire)) { chosen = v; break; }
        if (voices[v].samplesLeft < leastLeft) { leastLeft = voices[v].samplesLeft; chosen = v; }
    }
    if (chosen < 0) return;

    Voice& vo = voices[chosen];
    vo.active.store(false, std::memory_order_release); // pause before reconfiguring
    vo.freq         = freq;
    vo.phase        = 0.0;
    vo.samplesTotal = samples;
    vo.samplesLeft  = samples;
    vo.active.store(true, std::memory_order_release);
}

// SDL calls this from the audio thread to fill the output buffer.
static void audioCallback(void* /*userdata*/, Uint8* stream, int len) {
    float* out = reinterpret_cast<float*>(stream);
    const int frames = len / int(sizeof(float));

    for (int i = 0; i < frames; ++i) {
        double sample = 0.0;
        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& vo = voices[v];
            if (!vo.active.load(std::memory_order_acquire)) continue;

            const double env = double(vo.samplesLeft) / double(vo.samplesTotal); // linear decay 1->0
            sample += AMPLITUDE * env * std::sin(vo.phase);

            vo.phase += 2.0 * M_PI * vo.freq / double(SAMPLE_RATE);
            if (vo.phase >= 2.0 * M_PI) vo.phase -= 2.0 * M_PI;

            if (--vo.samplesLeft <= 0) vo.active.store(false, std::memory_order_release);
        }
        if (sample >  1.0) sample =  1.0; // guard against overlap clipping
        if (sample < -1.0) sample = -1.0;
        out[i] = float(sample);
    }
}

static void setNonBlock(int fd) { // act without waiting for events
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int /*argc*/, char* /*argv*/[]) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init audio failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec want{}, have{};
    want.freq     = SAMPLE_RATE;
    want.format   = AUDIO_F32SYS;
    want.channels = 1;
    want.samples  = 512;            // buffer size; smaller = lower latency
    want.callback = audioCallback;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (dev == 0) {
        fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_PauseAudioDevice(dev, 0); // start playback

    // --- Startup self-test -------------------------------------------------
    // Sweep every sensor frequency ascending then descending so you can confirm
    // the audio device actually produces sound before the agent starts moving.
    {
        const int holdMs = int(TONE_SEC * 1000.0) + 40; // let each beep finish
        for (int s = 0; s < SENSOR_COUNT; ++s) {         // ascending
            triggerBeep(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
        }
        for (int s = SENSOR_COUNT - 2; s >= 0; --s) {    // descending
            triggerBeep(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
        }
    }

    setNonBlock(0);                  // 0 = stdin; do not block on events
    srand(time(0) - getpid());       // seed the random number generator

    // Line-buffered reader.  stdio's scanf() is unreliable on a non-blocking fd
    // (an EAGAIN latches the stream's error flag and later reads keep failing),
    // so we read raw bytes with read() and parse whole lines ourselves.
    char   rbuf[4096];
    size_t rlen = 0;                 // bytes currently held in rbuf

    while (true) {
        // Drain everything available on stdin this frame.
        for (;;) {
            if (rlen >= sizeof(rbuf) - 1) rlen = 0; // overflow guard: drop partial
            ssize_t n = read(0, rbuf + rlen, sizeof(rbuf) - 1 - rlen);
            if (n > 0) {
                rlen += size_t(n);
                continue; // keep reading until the pipe is drained (EAGAIN)
            }
            break; // n == 0 (EOF) or n < 0 (EAGAIN/no more data)
        }
        rbuf[rlen] = 0;

        // Parse and consume every complete '\n'-terminated line in the buffer.
        char* start = rbuf;
        char* nl;
        while ((nl = (char*)memchr(start, '\n', rbuf + rlen - start)) != nullptr) {
            *nl = 0;
            int type; long long evtime; int sensor; // time is 64-bit microseconds
            if (3 == sscanf(start, "%d,%lld,%d", &type, &evtime, &sensor)) {
                // type 1 = wave, 2 = collision carry a real sensor index.
                // type 3 = death carries sensor 0; nothing to sense, so skip.
                if (type == 1 || type == 2) {
                    triggerBeep(sensor);
                }
            }
            start = nl + 1;
        }
        // Keep any trailing partial line for the next iteration.
        rlen = rbuf + rlen - start;
        memmove(rbuf, start, rlen);

        // Move the same way easya.cpp does: random action 1, 2 or 3.
        printf("0,%d,0\n", 1 + (rand() % 3));
        fflush(stdout); // prevent stdio from buffering the action

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // asyncEn runs at 30fps
    }

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
