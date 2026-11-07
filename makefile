CPP_FLAGS = -std=c++20
all:
	g++ ${CPP_FLAGS} src/asyncen.cpp src/game.cpp src/life.cpp src/waves.cpp src/geometry.cpp src/agents.cpp -g -o asyncen -lSDL2 -I src
	g++ keypad.cpp -o keypad -lSDL2
	g++ easya.cpp -o easya
	g++ bounce.cpp -o bounce
	g++ sound.cpp -o sound -lSDL2

clean:
	/bin/rm asyncen easya bounce keypad sound

run:
	asyncen easya bounce keypad sound
