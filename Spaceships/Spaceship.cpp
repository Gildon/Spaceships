#include "Spaceship.h"
#include "Item.h"

Spaceship::Spaceship():isActive(true) {}

Spaceship::Spaceship(char nsign, vector<Point> nlocs) : isActive(true) {
	locs = nlocs;
}

bool Spaceship::canMoveToDests(vector<Point> &dests) {

	for (size_t i = 0; i < dests.size(); i++) {
		char c = board->getChar(dests[i]);
		if (c != ' ' && c != '*')
			return false;
	}
	return true;
}

bool Spaceship::isItemAbove(Item *item) {

	vector<Point> upLocs(locs);
	board->updatePoints(upLocs, UP);
	for (size_t i = 0; i < upLocs.size(); i++) {
		char c = board->getChar(upLocs[i]);
		if (c == item->getSign())
			return true;
	}
	return false;
}