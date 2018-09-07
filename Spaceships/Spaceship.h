#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include "GameObject.h"

class Item;

class Spaceship: public GameObject {

protected:
	bool canMoveToDests(vector<Point> &dests);

public:
	Spaceship();
	Spaceship(char nsign, vector<Point> nlocs);
	bool isItemAbove(Item *item);
	bool isActive;
	virtual bool isValid() = 0;
};

#endif