CPP_FLAGS = -std=c++20
all:
	g++ ${CPP_FLAGS} src/asyncen.cpp src/game.cpp src/objects.cpp src/geometry.cpp src/agents.cpp -g -o asyncen -lSDL2 -I src
	g++ keypad.cpp -o keypad -lSDL2
	g++ easya.cpp -o easya

clean:
	/bin/rm asyncen easya keypad

run:
	asyncen easya keypad easya easya
