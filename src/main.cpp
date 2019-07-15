#include "../include/roundedRectangleShape.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024
#define BOARD_COLS 4
#define BOARD_ROWS 4 
#define NUM_ITERATIONS 100

bool boardsAreEqual(int** a, int** b) {
	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			if(a[y][x] != b[y][x]) { return false; }
		}
	}
	return true;
}

void copyBoard(int** src, int** dst) {
	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			dst[y][x] = src[y][x];
		}
	}
}

void freeBoard(int** board) {
	for(int y=0;y<BOARD_ROWS;y++) { delete board[y]; }
	delete board;
}

void shift(int** board) {
	int** copiedBoard = new int*[BOARD_ROWS];
	for(int y=0;y<BOARD_ROWS;y++) {
		copiedBoard[y] = new int[BOARD_COLS];
		int j = 0;
		for(int x=0;x<BOARD_COLS;x++) {
			if(board[y][x] != 0) {
				copiedBoard[y][j++] = board[y][x];
			}
		}
		for(;j<BOARD_COLS;j++) {
			copiedBoard[y][j] = 0;
		}
	}

	copyBoard(copiedBoard, board);
	freeBoard(copiedBoard);
}

int combine(int** board) {
	int score = 0;
	shift(board);
	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS-1;x++) {
			if(board[y][x] == board[y][x+1] && board[y][x] != 0) {
				board[y][x] *= 2;
				board[y][x+1] = 0;
				score += board[y][x];
			}
		}
	}
	shift(board);
	return score;
}

void spawn(int** board) {
	int xs[BOARD_ROWS * BOARD_COLS];
	int ys[BOARD_ROWS * BOARD_COLS];
	int j=0;
	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			if(board[y][x] == 0) { xs[j] = x; ys[j++] = y; }
		}
	}
	if(j > 0) {
		int choice = rand() % j;
		board[ys[choice]][xs[choice]] = (((int)rand() % 10 < 9) ? 2 : 4);
	}
}

void rotateC(int** board) {
	int** copiedBoard = new int*[BOARD_ROWS];
	for(int y=0;y<BOARD_ROWS;y++) {
		copiedBoard[y] = new int[BOARD_COLS];
	}

	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			copiedBoard[x][BOARD_ROWS - y - 1] = board[y][x];
		}
	}

	copyBoard(copiedBoard, board);
	freeBoard(copiedBoard);
}

void rotateCC(int** board) {
	for(int i=0;i<3;i++) { rotateC(board); }
}

int left(int** board) {
	int result = combine(board);
	spawn(board);
	return result;
}

int up(int** board) {
	rotateCC(board);
	int result = left(board);
	rotateC(board);
	return result;
}

int right(int** board) {
	rotateC(board); rotateC(board);
	int result = left(board);
	rotateC(board); rotateC(board);
	return result;
}

int down(int** board) {
	rotateC(board);
	int result = left(board);
	rotateCC(board);
	return result;
}

int move(int idx, int** board) {
	switch(idx) {
		case 0:
			return left(board); break;
		case 1:
			return up(board); break;
		case 2:
			return right(board); break;
		case 3:
			return down(board); break;
		default:
			break;
	}
	return -1;
}

bool gameFinished(int** board) {
	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			if(board[y][x] == 0) { return false; }
		}
	}

	for(int y=0;y<BOARD_ROWS-1;y++) {
		for(int x=0;x<BOARD_COLS;x++) {
			if(board[y][x] == board[y+1][x]) { return false; }
		}
	}

	for(int y=0;y<BOARD_ROWS;y++) {
		for(int x=0;x<BOARD_COLS-1;x++) {
			if(board[y][x] == board[y][x+1]) { return false; }
		}
	}

	return true;
}

bool canMakeMove(int moveIndex, int** board) {
	int** copiedBoard = new int*[BOARD_ROWS];
	for(int y=0;y<BOARD_ROWS;y++) { copiedBoard[y] = new int[BOARD_COLS]; }
	copyBoard(board, copiedBoard);
	move(moveIndex, copiedBoard);
	bool result = !boardsAreEqual(copiedBoard, board);
	freeBoard(copiedBoard);
	return result;
}

int aiGetMove(int** board) {
	int scores[4][NUM_ITERATIONS];
	int score;
	std::vector<double> averaged;
	std::vector<int> averagedIndices;
	int** copiedBoard = new int*[BOARD_ROWS];
	for(int y=0;y<BOARD_ROWS;y++) { copiedBoard[y] = new int[BOARD_COLS]; }

	for(int i=0;i<NUM_ITERATIONS;i++) {
		copyBoard(board, copiedBoard);
		left(copiedBoard);
		score = 0;
		while(!gameFinished(copiedBoard)) {
			score += move(rand() % 4, copiedBoard);
		}
		scores[0][i] = score;

		copyBoard(board, copiedBoard);
		up(copiedBoard);
		score = 0;
		while(!gameFinished(copiedBoard)) {
			score += move(rand() % 4, copiedBoard);
		}
		scores[1][i] = score;

		copyBoard(board, copiedBoard);
		right(copiedBoard);
		score = 0;
		while(!gameFinished(copiedBoard)) {
			score += move(rand() % 4, copiedBoard);
		}
		scores[2][i] = score;

		copyBoard(board, copiedBoard);
		down(copiedBoard);
		score = 0;
		while(!gameFinished(copiedBoard)) {
			score += move(rand() % 4, copiedBoard);
		}
		scores[3][i] = score;
	}

	freeBoard(copiedBoard);

	for(int i=0;i<4;i++) {
		averagedIndices.push_back(i);
		averaged.push_back(0);
		for(int j=0;j<NUM_ITERATIONS;j++) { averaged[i] += scores[i][j]; }
		averaged[i] /= NUM_ITERATIONS;
	}

	std::sort(averagedIndices.begin(), averagedIndices.end(), [averaged](int a, int b) -> bool { return averaged[a] < averaged[b]; });
	for(int i=averagedIndices.size()-1;i>=0;i--) {
		if(canMakeMove(averagedIndices[i], board)) { return averagedIndices[i]; }
	}
	return -1;
}

int main(int argc, char** argv) {
	srand(time(0));

	sf::RenderWindow win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2048 Played Using Artificial Intelligence");

	sf::Color colors[] = {
		sf::Color(238,229,218),
		sf::Color(232,223,198),
		sf::Color(245,178,122),
		sf::Color(246,150,99),
		sf::Color(236,206,115),
		sf::Color(235,198,79),
		sf::Color(238,77,89),
		sf::Color(94,160,222),
		sf::Color(0,124,189)
	};

	sf::RoundedRectangleShape shape(sf::Vector2f(WINDOW_WIDTH / 4 - 4, WINDOW_HEIGHT / 4 - 4), 15, 4);

	sf::Font font;
	font.loadFromFile("/System/Library/Fonts/Keyboard.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(60);
	text.setFillColor(sf::Color(0,0,0));

	int** board = new int*[BOARD_ROWS];
	for(int y=0;y<BOARD_ROWS;y++) {
		board[y] = new int[BOARD_COLS];
		for(int x=0;x<BOARD_COLS;x++) {
			board[y][x] = 0;
		}
	}
	spawn(board); spawn(board);

	while(win.isOpen()) {
		sf::Event e;
		while(win.pollEvent(e)) {
			if(e.type == sf::Event::Closed) { win.close(); }
			else if(e.type == sf::Event::KeyPressed) {
				if(e.key.code == sf::Keyboard::Left)       { left(board);  }
				else if(e.key.code == sf::Keyboard::Up)    { up(board);    }
				else if(e.key.code == sf::Keyboard::Right) { down(board);  }
				else if(e.key.code == sf::Keyboard::Down)  { right(board); }
			}
		}

		move(aiGetMove(board), board);

		win.clear();
		for(int y=0;y<4;y++) {
			for(int x=0;x<4;x++) {
				shape.setPosition(2 + x * WINDOW_WIDTH / 4, 2 + y * WINDOW_HEIGHT / 4);
				if(board[y][x] == 0) { shape.setFillColor(sf::Color(204, 204, 206)); }
				else { shape.setFillColor(colors[(int)(log(board[y][x]) / log(2)) % (sizeof(colors)/sizeof(colors[0]))]); }
				win.draw(shape);

				if(board[y][x] != 0) {
					text.setString(std::to_string(board[y][x]));
					text.setPosition(x * WINDOW_WIDTH / 4 + WINDOW_WIDTH / 8 - text.getLocalBounds().width / 2, y * WINDOW_HEIGHT / 4 + WINDOW_HEIGHT / 8 - 30);
					win.draw(text);
				}
			}
		}
		win.display();
	}

	freeBoard(board);
	return 0;
}
