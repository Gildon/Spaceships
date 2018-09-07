#ifndef _PLAYERSHIP_H_
#define _PLAYERSHIP_H_

#include "Spaceship.h"
#include "Item.h"

class PlayerShip: public Spaceship {

protected:
	vector<Item *> carriedItems;

public:
	PlayerShip();
	PlayerShip(char nsign, vector<Point> nlocs);

	bool isActive; 

	void findCarriedItems();
	bool isCarryingItem(Item *item);
	virtual bool move(DIRECTION ndir);
	virtual bool isValid();
};

#endif