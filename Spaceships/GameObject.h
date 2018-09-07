#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "defs.h"
#include "io_utils.h"
#include "Point.h"
#include "Board.h"

class Game;

class GameObject {

protected:
	char sign;
	vector<Point> locs;
	Color color;
	Board *board;
	Game *game;

public:
	char getSign() { return sign; };
	vector<Point> &getLocs() { return locs; };
	Color getColor() { return color; };
	int getSize() { return locs.size(); };
	void setSign(char nSign) { sign = nSign; };
	void setLocs(vector<Point> nLocs) { locs = nLocs; };
	void setColor(Color nColor) { color = nColor; };
	void setGame(Game *g);
	virtual bool isValid() = 0;
	virtual bool move(DIRECTION dir);
};

#endif