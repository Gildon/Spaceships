#ifndef _SMALLSHIP_H_
#define _SMALLSHIP_H_

#include "PlayerShip.h"

class SmallShip: public PlayerShip {

	DIRECTION dir;

public:
	SmallShip();
	SmallShip(DIRECTION ndir);

	DIRECTION getDir() { return dir; };
	void setDir(DIRECTION ndir) { dir = ndir; };
	void changeDir();
	virtual bool move(DIRECTION ndir);
	virtual bool isValid();
};

#endif