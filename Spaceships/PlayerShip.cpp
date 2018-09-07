#include "PlayerShip.h"
#include "Game.h"
#include "Item.h"

PlayerShip::PlayerShip(): isActive(true) {}
PlayerShip::PlayerShip(char nsign, vector<Point> nlocs):isActive(true) {
	setSign(nsign);
	setLocs(nlocs);
}

void PlayerShip::findCarriedItems() {

	vector<Point> upLocs(locs);
	board->updatePoints(upLocs, UP);
	carriedItems.clear();
	vector<Point> carryLocs;
	for (size_t i = 0; i < upLocs.size(); i++) {
		char c = board->getChar(upLocs[i]);
		if (c < '1' || c > '9')
			continue;
		Item *item = game->getItem(c);
		if (std::find(carriedItems.begin(), carriedItems.end(), item) != carriedItems.end())
			continue;
		if (!item->isFalling) {
			carriedItems.push_back(item);
			carryLocs.push_back(upLocs[i]);
		}
	}

	if (carriedItems.size() >= 2 && carryLocs[0].getX() > carryLocs[1].getX()) {
		// make sure carriedItems are sorted by the carry location's x coordinate, in ascending order
		reverse(carriedItems.begin(), carriedItems.end());
	}
}

bool PlayerShip::isCarryingItem(Item *item)  {

	return find(carriedItems.begin(), carriedItems.end(), item) != carriedItems.end();
}

bool PlayerShip::move(DIRECTION ndir) {

	if (!GameObject::move(ndir))
		return false;

	// carry items:
	if (ndir != UP && carriedItems.size() > 0) {
		vector<Item*> itemsInOrder(carriedItems); // we want to move items in the right order (in case carrying 2 items)
		if (ndir == RIGHT)
			std::reverse(itemsInOrder.begin(), itemsInOrder.end()); // start with rightmost object

		for (size_t i = 0; i < itemsInOrder.size(); i++) {
			Item *item = itemsInOrder[i];
			if (!item->isPushed && item->canBeMoved(ndir))
				item->move(ndir);
		}
	}
	return true;
}

bool PlayerShip::isValid() {

	vector<Point> minusLocs(locs), plusLocs(locs);
	board->updatePoints(minusLocs, -1, -1);
	board->updatePoints(plusLocs, 1, 1);

	if (getSize() != 4)
		return false;

	return 
		(locs[0].getX() == minusLocs[1].getX() || locs[0].getX() == plusLocs[1].getX()) &&
		locs[0].getY() == locs[1].getY()												&&
		(locs[2].getX() == minusLocs[3].getX() || locs[2].getX() == plusLocs[3].getX()) &&
		locs[2].getY() == locs[3].getY()												&&
		locs[0].getX() == locs[2].getX()												&&
		(locs[0].getY() == minusLocs[2].getY() || locs[0].getY() == plusLocs[2].getY());
}