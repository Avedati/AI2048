all:
	g++ -o bin/ai2048 -std=c++11 src/*.cpp -lsfml-system -lsfml-graphics -lsfml-window
