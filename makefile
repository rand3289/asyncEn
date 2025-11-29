all:
	g++ asyncen.cpp game.cpp objects.cpp geometry.cpp agents.cpp -g -o asyncen -lSDL2
	g++ keypad.cpp -o keypad -lSDL2
	g++ easya.cpp -o easya
