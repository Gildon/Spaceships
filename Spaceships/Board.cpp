#include "Board.h"
#include <string.h>

Board::Board() {

	clear();
}

void Board::clear() {

	memset(board, ' ', X_RANGE * Y_RANGE * sizeof(char));
	hasExit = false;
}

void Board::updatePoint(Point &p, int dx, int dy) 
{
	p.setX(X_WRAP(p.getX() + dx));
	p.setY(Y_WRAP(p.getY() + dy));
}

void Board::updatePoints(vector<Point> &points, int dx, int dy) {

	for (size_t i = 0; i < points.size(); i++)
		updatePoint(points[i], dx, dy);
}

void Board::updatePoints(vector<Point> &points, DIRECTION dir) {

	int dx = 0, dy = 0;
	switch(dir) {
		case UP:	dy--;	break;
		case DOWN:	dy++;	break;
		case LEFT:	dx--;	break;
		case RIGHT: dx++;	break;
	}
	updatePoints(points, dx, dy);
}

void Board::printBoard() {

	for (int y = 0; y <= Y_MAX; y++) {
		gotoxy(0, y);
		for (int x = 0; x <= X_MAX; x++) {
			if (board[y][x] == 'X')
				printPoint(Point(x, y), 'X', LIGHTGREEN);
			else if (board[y][x] >= '1' && board[y][x] <= '9')
				printPoint(Point(x, y), board[y][x], (Color)(board[y][x] - '0'));
			else if (board[y][x] == '#')
				printPoint(Point(x, y), '#', YELLOW);
			else if (board[y][x] == '@')
				printPoint(Point(x, y), '@', LIGHTMAGENTA);
			else if (board[y][x] == 'W')
				printPoint(Point(x, y), 'W', LIGHTRED);
			else if (board[y][x] == '*')
				printPoint(Point(x, y), '*', LIGHTRED);
			else
				cout << board[y][x];
		}
	}
}

char Board::getChar(int x, int y) {

	return board[y][x];
}

char Board::getChar(Point &loc) {

	return board[loc.getY()][loc.getX()];
}

void Board::setChar(int x, int y, char sign) {

	board[y][x] = sign;
}

void Board::setChar(Point &loc, char sign) {

	board[loc.getY()][loc.getX()] = sign;
}

void Board::setLine(int y, const string &line) {

	memcpy(board[y], line.c_str(), sizeof(char) * line.size());
}

string Board::getLine(int y) 
{
	string line(board[y], 80);
	line.resize(80, ' ');
	return line;
}

void Board::erasePoint(Point &dest, char origSign) {

	if (getChar(dest) == origSign)
		printPoint(dest, ' ');
}

void Board::erasePoints(vector<Point> &dests, char origSign) {

	// erase points in dests (if they belong to the same object being erased)
	for (size_t i = 0; i < dests.size(); i++) {
		erasePoint(dests[i], origSign);
	}
}

void Board::printPoints(vector<Point> &dests, char printSign, Color printColor) {

	for (size_t i = 0; i < dests.size(); i++) 
		printPoint(dests[i], printSign, printColor);
}

void Board::printPoint(Point dest, char printSign, Color printColor) {

	setChar(dest.getX(), dest.getY(), printSign);
	setColor(printColor); gotoxy(dest.getX(), dest.getY());	cout << printSign;
	if (printColor != WHITE)
		setColor(WHITE);
}

void Board::loadFromFile(ifstream &file){

	string line;
	getline(file, line); // skipping first line
	hasExit = false;
	for (int i = Y_MIN; i <= Y_MAX && !file.eof(); i++) {
		getline(file, line);
		line.resize(80, ' ');
		setLine(i, line);
		hasExit = hasExit || (line.find('X') != string::npos);
	}
}