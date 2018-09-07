#ifndef _EVILSHIP_H_
#define _EVILSHIP_H_

#include "SmallShip.h"

class EvilShip : public Spaceship {

	Point *findTarget();

public:
	EvilShip(Point loc);
	virtual bool isValid() { return true; };
	void chaseTarget();
};

#endif