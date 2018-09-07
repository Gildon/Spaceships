#include "GameObject.h"
#include "Game.h"

void GameObject::setGame(Game *g) {

	game = g;
	board = &g->board;
}

bool GameObject::move(DIRECTION ndir) {
	vector<Point> tempLocs = locs;
	board->updatePoints(tempLocs, ndir);
	if (!game->checkBombs(tempLocs))
		return false;
	board->erasePoints(locs, sign);
	setLocs(tempLocs);
	board->printPoints(locs, sign, color);
	return true;
}