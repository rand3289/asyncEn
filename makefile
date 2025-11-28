all:
	g++ -g asyncen.cpp game.cpp objects.cpp geometry.cpp agents.cpp -o asyncen -lSDL2
