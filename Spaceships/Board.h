#ifndef _BOARD_H_
#define _BOARD_H_

#include "defs.h"
#include "io_utils.h"
#include "Point.h"

class Board {

	char board[Y_RANGE][X_RANGE];
	bool hasExit;

public:
	Board();
	void clear();
	void printBoard();
	void printPoints(vector<Point> &dests, char printSign, Color printColor = WHITE);
	void printPoint(Point dest, char printSign, Color printColor = WHITE);
	char getChar(int x, int y);
	char getChar(Point &loc);
	void setChar(int x, int y, char sign);
	void setChar(Point &loc, char sign);
	string getLine(int y);
	void setLine(int y, const string &line);
	void updatePoint(Point &p, int dx, int dy);
	void updatePoints(vector<Point> &points, int dx, int dy);
	void updatePoints(vector<Point> &points, DIRECTION dir);
	void erasePoint(Point &dest, char origSign);
	void erasePoints(vector<Point> &dests, char origSign);
	void loadFromFile(ifstream &file);
	bool doesHaveExit() { return hasExit; }
};

#endif