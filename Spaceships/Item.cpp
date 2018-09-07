#include "Item.h"
#include "Game.h"

Item::Item(char nsign): isPushed(false) {

	setSign(nsign);
	fallWeight = getSize();
	setColor((Color)(sign - '0'));
}

bool Item::canBeMoved(DIRECTION dir, string allowedChars) {

	vector<Point> dests = locs;
	board->updatePoints(dests, dir);

	for (size_t i = 0; i < dests.size(); i++) {
		char c = board->getChar(dests[i]);
		if (c != sign && c != ' ' && c != '*' && allowedChars.find(c) == string::npos)
			return false;
	}
	return true;
}


bool Item::move(DIRECTION dir) {
	if (hasMoved)
		return true;

	vector<Point> upLocs = locs;

	if (!GameObject::move(dir))		
		return false; // item detonated a bomb

	hasMoved = true;

	// if we're carried, move all items above us too
	if (!isPushed && !isFalling && dir != UP && !isSlowPushed) {
		board->updatePoints(upLocs, UP);
		isPushed = true;;
		for (size_t i = 0; i < upLocs.size(); i++) {
			char c = board->getChar(upLocs[i]);
			if (c < '1' || c > '9')
				continue;

			Item *upItem = game->getItem(c);
			if (!upItem->isPushed && !upItem->isFalling && upItem->canBeMoved(dir))
				upItem->move(dir);
		}
	}

	return true;
}

void Item::fall() {

	vector<Point> upLocs = locs;
	isFalling = true;
	if (!move(DOWN))
		return;
	fallWeight = getSize();

	// make items that are above this item fall too, and aggregate the weights:
	board->updatePoints(upLocs, UP);
	for (size_t i = 0; i < upLocs.size(); i++) {
		char c = board->getChar(upLocs[i]);
		if (c < '1' || c > '9' || c == sign)
			continue;

		Item *upItem = game->getItem(c);
		if (upItem->canBeMoved(DOWN) && !upItem->isFalling) {
			upItem->fall();
			fallWeight += upItem->fallWeight;
		}
	}
}

void Item::floodFill(int x, int y, map<Point, bool> &visitedPoints) {

	Point point(x, y);

	if (board->getChar(point) != sign)
		return; // point doesn't belong to item

	if (visitedPoints.find(point) != visitedPoints.end())
		return; // already visited this point

	visitedPoints[point] = true;

	floodFill(X_WRAP(x - 1), y    , visitedPoints);
	floodFill(X_WRAP(x + 1), y    , visitedPoints);
	floodFill(x    , Y_WRAP(y - 1), visitedPoints);
	floodFill(x    , Y_WRAP(y + 1), visitedPoints);
}

bool Item::isValid() {

	if (locs.empty())
		return false;

	map<Point, bool> visitedPoints;
	
	// flood fill the map, starting at the first location of item
	floodFill(locs[0].getX(), locs[0].getY(), visitedPoints);

	// now check that all item's locations have been visited
	for (size_t i = 0; i < locs.size(); i++) {
		if (visitedPoints.find(locs[i]) == visitedPoints.end())
			return false;
	}

	return true;
}