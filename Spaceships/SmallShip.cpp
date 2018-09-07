#include "SmallShip.h"

SmallShip::SmallShip() {};

SmallShip::SmallShip(DIRECTION ndir) :dir(ndir) {};

void SmallShip::changeDir() {

	if (!isActive || carriedItems.size() > 0)
		return;

	bool canMove = false;
	vector<Point> dests = locs;

	if (dir == HORIZONTAL) {
		board->updatePoints(dests, DOWN);
		canMove = canMoveToDests(dests);

		if (!canMove) {
			dests = locs;
			board->updatePoints(dests, UP);
			canMove = canMoveToDests(dests);
		}
	}
	else {
		board->updatePoints(dests, RIGHT);
		canMove = canMoveToDests(dests);

		if (!canMove) {
			dests = locs;
			board->updatePoints(dests, LEFT);
			canMove = canMoveToDests(dests);
		}
	}

	if (!canMove)
		return;

	board->erasePoints(locs, sign);
	locs[1] = dests[0];
	board->printPoints(locs, sign, color);
	dir *= -1;
}

bool SmallShip::move(DIRECTION ndir) {

	// in case small ship's direction was previously changed causing the pivot location to switch,
	// we'll reset the pivot to be top/left location, after every move
	if ((dir == HORIZONTAL && locs[0].getX() > locs[1].getX()) ||
		(dir == VERTICAL   && locs[0].getY() > locs[1].getY()))
	{
		vector<Point> newLocs;
		newLocs.push_back(locs[1]);
		newLocs.push_back(locs[0]);
		locs = newLocs;
	}

	return PlayerShip::move(ndir);
}

bool SmallShip::isValid() {

	vector<Point> minusLocs(locs), plusLocs(locs);
	board->updatePoints(minusLocs, -1, -1);
	board->updatePoints(plusLocs, 1, 1);

	if (getSize() != 2)
		return false;

	return
		(locs[0].getX() == locs[1].getX() &&
		(locs[0].getY() == minusLocs[1].getY() || locs[0].getY() == plusLocs[1].getY()))
		||
		((locs[0].getX() == minusLocs[1].getX() || locs[0].getX() == plusLocs[1].getX()) &&
		locs[0].getY() == locs[1].getY());
}