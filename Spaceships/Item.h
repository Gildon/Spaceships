#ifndef _ITEM_H_
#define _ITEM_H_

#include "defs.h"
#include "io_utils.h"
#include "Point.h"
#include "Board.h"
#include "GameObject.h"

class Item: public GameObject {

public:
	bool isPushed; // is any ship already pushing this item in current turn?
	DIRECTION pushedDirection; // the push direction, if item already being pushed
	bool isFalling; // did the item fall this turn?
	int fallWeight; // total weight of this item and items above it, falling as a group
	bool hasMoved; // did it move in this turn?
	bool isSlowPushed;

	Item(char nsign);

	bool move(DIRECTION dir);
	bool canBeMoved(DIRECTION dir, string allowedChars = "");
	void fall();
	void floodFill(int x, int y, map<Point, bool> &visitedPoints);
	virtual bool isValid();
};

#endif