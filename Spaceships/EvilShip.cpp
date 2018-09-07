#include "EvilShip.h"
#include "Game.h"

EvilShip::EvilShip(Point loc) {
	locs.push_back(loc);
	setSign('W');
	setColor(LIGHTRED);
}

void EvilShip::chaseTarget() {

	if (!isActive)
		return;
	vector<Point> dest;
	DIRECTION xDir, yDir, chaseDir[2];
	Point from = locs[0];
	Point to = game->findClosestShipPoint(from);
	xDir = (from.getX() == to.getX()) ? 0 : (from.getX() < to.getX()) ? RIGHT : LEFT;
	yDir = (from.getY() == to.getY()) ? 0 : (from.getY() < to.getY()) ? DOWN : UP;

	if (abs(from.getX() - to.getX()) < (abs(from.getY() - to.getY())))
		chaseDir[0] = yDir;
	else
		chaseDir[0] = xDir;

	chaseDir[1] = (chaseDir[0] == yDir) ? xDir : yDir;

	for (int i = 0; i < 2; i++) {
		if (chaseDir[i] == 0)
			continue;
		dest = locs;
		board->updatePoints(dest, chaseDir[i]);
		if (canMoveToDests(dest)) {
			move(chaseDir[i]);
			return;
		}
		else if (board->getChar(dest[0]) == '@' || board->getChar(dest[0]) == '#') // reached player ship
			game->removeGameObjectAtPoint(dest[0]);
	}
}