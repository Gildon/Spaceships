#include "Game.h"
#include <set>
#include <string.h>

Game::Game(string directory) :gameStatus(MENU), workingDirectory(directory), isSlowPushMode(false)
{
	smallShip.setGame(this);
	largeShip.setGame(this);
	smallShip.setSign('@');
	largeShip.setSign('#');
	smallShip.setColor(LIGHTMAGENTA);
	largeShip.setColor(YELLOW);

	if (workingDirectory.length() > 0 && workingDirectory[workingDirectory.length() - 1] != '\\')
		workingDirectory.append("\\");

	levels = loadLevels(findFiles(workingDirectory + "*.spg"));
}

void Game::updateDataMembers() 
{
	for (int y = Y_MIN; y <= Y_MAX; y++) {
		for (int x = X_MIN; x <= X_MAX; x++) {
			char c = board.getChar(x, y);
			if (c == '@')
				smallShip.getLocs().push_back(Point(x, y));
			else if (c == '#')
				largeShip.getLocs().push_back(Point(x, y));
			else if (c == 'W')
				evilShips.push_back(EvilShip(Point(x, y)));
			else if (c >= '1' && c <= '9')  {
				Item *currItem = getItem(c);
				if (!currItem) { // read new item from the board
					items.push_back(Item(c));
					currItem = &items[items.size() - 1];
					currItem->setGame(this);
				}
				currItem->getLocs().push_back(Point(x, y));
			}
		}
	}
	for (vector<EvilShip>::iterator itr = evilShips.begin(); itr != evilShips.end(); itr++)
		itr->setGame(this);
	if (smallShip.getSize() == 2) {
		int x1 = smallShip.getLocs()[0].getX(), x2 = smallShip.getLocs()[1].getX();
		if (x1 == x2) // determining the small ship's orientation
			smallShip.setDir(VERTICAL);
		else
			smallShip.setDir(HORIZONTAL);
	}
	smallShip.isActive = smallShip.isValid();
	largeShip.isActive = largeShip.isValid();

	if (recordedTurns.size() == 0)
		turn = 0;
	else
		turn = recordedTurns.back().getTurnNum();
}

void Game::clearDataMembers() {

	smallShip.getLocs().clear();
	largeShip.getLocs().clear();
	evilShips.clear();
	items.clear();
	explosions.clear();
	currBlastWave.clear();
	recordedTurns.clear();
	board.clear();
	isSlowPushMode = false;
}

void Game::moveShipCommand(PlayerShip &ship, DIRECTION dir) {

	if (!ship.isActive) // ship has reached the exit and been removed from the game
		return;

	vector<Point> locs(ship.getLocs()); // locations of object we're currently checking adjacent cells of

	set<Item *> itemsToPush; // use a set so we can ignore duplicates (if same item is added twice)
	bool itemChecked[9];
	memset(itemChecked, 0, sizeof(itemChecked));

	bool locsAreShip = true;
	bool doneCheckingItems = false;
	bool bothShipsPushing = false;
	bool adjacentToShip = true; // true if we're checking cells adjacent to the ship
	bool reachedExit = false;
	int totalItemsSize = 0;

	while (!doneCheckingItems) {
		board.updatePoints(locs, dir); // shift locs in push direction, to get adjacent cells

		for (size_t i = 0; i < locs.size(); i++) {
			char c = board.getChar(locs[i]);


			if (c >= '1' && c <= '9') {
				itemsToPush.insert(getItem(c));
			}
			else if (c == '#' || c == '@') {
				if (!locsAreShip || c != ship.getSign())
					return;
			}
			else if (locsAreShip && c == 'X')
				reachedExit = true;
			else if (c != ' ' && c != '*')
				return; // any other character is a wall
		}

		doneCheckingItems = true;

		for (set<Item*>::iterator item = itemsToPush.begin(); item != itemsToPush.end(); item++) {
			char itemNum = (*item)->getSign() - '1';

			if (!itemChecked[itemNum]) {
				// we've not checked this item's adjacent cells yet
				itemChecked[itemNum] = true;

				if (adjacentToShip) {
					if ((*item)->isPushed && (*item)->pushedDirection == dir) {
						bothShipsPushing = true;
					}
					else {
						(*item)->isPushed = true;
						(*item)->pushedDirection = dir;
					}

					adjacentToShip = false; // we're moving on to check cells not directly adjacent to ship
				}

				if (isSlowPushMode) {
					vector<Point> upLocs = (*item)->getLocs();
					board.updatePoints(upLocs, UP);
					for (size_t j = 0; j < upLocs.size(); j++) {
						char c = board.getChar(upLocs[j]);
						Item *upItem = (c >= '1' && c <= '9') ? getItem(c) : NULL;
						if (upItem && itemsToPush.find(upItem) == itemsToPush.end()) {
							itemsToPush.insert(upItem);
							upItem->isSlowPushed = true;
						}
					}
				}

				totalItemsSize += (*item)->getSize();
				locs = (*item)->getLocs(); // we now move to check this item's adjacent locations
				doneCheckingItems = false;
				locsAreShip = false;
				break;
			}
		}
	}

	if (bothShipsPushing) {
		if (totalItemsSize > 6)
			return;
	}
	else if (totalItemsSize > ship.getSize())
		return;

	for (set<Item*>::iterator item = itemsToPush.begin(); item != itemsToPush.end(); item++)
		(*item)->move(dir);

	if (reachedExit) {
		ship.isActive = false;
		board.erasePoints(ship.getLocs(), ship.getSign());
		return;
	}

	if (bothShipsPushing) {
		// both ships are pushing, so both need to move.
		// but when first ship tried to push, we didn't let it. so we'll move both now:
		largeShip.move(dir);
		smallShip.move(dir);
	}
	else
		ship.move(dir);
}

void Game::doCommand(char input) {

	switch (input) {
	case 'i': moveShipCommand(largeShip, UP);		break;
	case 'j': moveShipCommand(largeShip, LEFT);		break;
	case 'l': moveShipCommand(largeShip, RIGHT);	break;
	case 'm': moveShipCommand(largeShip, DOWN);		break;
	case 'w': moveShipCommand(smallShip, UP);		break;
	case 'a': moveShipCommand(smallShip, LEFT);		break;
	case 'd': moveShipCommand(smallShip, RIGHT);	break;
	case 'x': moveShipCommand(smallShip, DOWN);		break;
	case 'z': smallShip.changeDir();				break;
	case 'b': changePushMode();						break;
	case ESC: 
		if (gameStatus == IN_GAME)
			setStatus(IN_GAME_PLAY_MENU);
		else setStatus(SOLUTION_PLAY_MENU);			break;
	}
}

bool Game::isSmallInput(char input) {
	return (input == 'w' || input == 'a' || input == 'd' || input == 'x' || input == 'z');
}

bool Game::isLargeInput(char input) {
	return (input == 'i' || input == 'j' || input == 'l' || input == 'm');
}

void Game::printBoard() {

	board.printBoard();
	board.printPoints(smallShip.getLocs(), smallShip.getSign(), smallShip.getColor());
	board.printPoints(largeShip.getLocs(), largeShip.getSign(), largeShip.getColor());
	for (size_t i = 0; i < items.size(); i++) {
		board.printPoints(items[i].getLocs(), items[i].getSign(), items[i].getColor());
	}
}

void Game::play() 
{
	while (gameStatus != QUIT) 
	{
		if (gameStatus == MENU) {
			mainMenu();
		}

		if (gameStatus == SOLUTION_PLAY_MENU || gameStatus == IN_GAME_PLAY_MENU) {
			playMenu();
		}

		if (gameStatus == IN_GAME || gameStatus == PLAY_SOLUTION) {
			playTurn();
		}

		if (gameStatus == GAME_OVER) {
			gameOver();
		}

		if (gameStatus == VICTORY) {
			victory();
		}
	}
}

void Game::printBottomInfo() const {

	string clock = "\\\\\\|||///---";
	setColor(LIGHTGRAY);
	gotoxy(0, 24);	cout << clock[turn % 12];	cout << " " << turn;
	if (turn == 1) {
		// deleting extra character that may have been left by a restarted or completed level
		cout << string(75, ' ');
		currLevel->printInfo();
	}
	setColor(WHITE);
}

void Game::changePushMode() {

	isSlowPushMode = (isSlowPushMode) ? false : true;
	gotoxy(17, 24);
	setColor(YELLOW);
	cout << ((isSlowPushMode) ? "[SLOW]" : "      ");
	setColor(WHITE);
}

void Game::handleItems() {

	for (size_t i = 0; i < items.size(); i++) {
		items[i].isPushed = false;
		items[i].isSlowPushed = false;
		items[i].isFalling = false;
		items[i].hasMoved = false;
	}

	for (size_t i = 0; i < items.size(); i++) {
		Item &item = items[i];

		if (item.canBeMoved(DOWN, "W") && !item.isFalling) {
			// item can be moved down (maybe by killing evilships)
			if (!item.canBeMoved(DOWN)) { // more effecient assuming there are more ships than the size of the item
				// evilship(s) are in the way, let's kill them
				for (vector<EvilShip>::iterator evilship = evilShips.begin(); evilship != evilShips.end();) {
					if (evilship->isItemAbove(&item)) {
						//board.erasePoints(evilship->getLocs(), evilship->getSign());
						evilship = evilShips.erase(evilship);
					}
					else
						evilship++;
				}
			}

			item.fall();
		}
		else {
			PlayerShip *ship = NULL;

			// check if item is above a ship, and the ship was not already carrying it:
			if (smallShip.isItemAbove(&item) && !smallShip.isCarryingItem(&item))
				ship = &smallShip;
			else if (largeShip.isItemAbove(&item) && !largeShip.isCarryingItem(&item))
				ship = &largeShip;

			if (ship && item.fallWeight >= ship->getSize() / 2) {
				// game over
				deadShip = ship;
				setStatus(GAME_OVER);
				return;
			}
		}
	}
}

void Game::playTurn() {

	// main game loop
	turn++;

	printBottomInfo();

	if (explosions.size() > 0) { // animate explosions if any bombs have been "recently" set off
		for (size_t i = 0; i < explosions.size(); i++)
			animateBombExplosion(i);
	}

	if (!smallShip.isActive && !largeShip.isActive) {
		if (gameStatus == IN_GAME)
			setStatus(VICTORY);
		else 
			setStatus(MENU);
		return;
	}

	Sleep(60);
	if (gameStatus == PLAY_SOLUTION) // slow motion playback
		Sleep(40);

	handleItems();

	// move evil ships towards their targets
	if (turn % 2 == 0 && evilShips.size() > 0) {
		for (vector<EvilShip>::iterator evilship = evilShips.begin(); evilship != evilShips.end();) {
			evilship->chaseTarget();
			if (!evilship->isActive) // evil ship has died while chasing a target
				evilship = evilShips.erase(evilship);
			else
				evilship++;
		}
	}

	if (!smallShip.isActive)
		smallShip.getLocs().clear();
	if (!largeShip.isActive)
		largeShip.getLocs().clear();

	handlePlayerInput();

	smallShip.findCarriedItems();
	largeShip.findCarriedItems();
}

void Game::handlePlayerInput() {

	TurnActions turnActions;
	char smallInput = 0, largeInput = 0;
	char currInput = 0;
	while (_kbhit()) { // comparing input against two chars and saving last char for each respective ship
		currInput = _getch();
		if (gameStatus != PLAY_SOLUTION) {
			if (isSmallInput(currInput))
				smallInput = currInput;
			else if (isLargeInput(currInput))
				largeInput = currInput;
		}
	}

	if (gameStatus == PLAY_SOLUTION && currInput == 'b')
		currInput = NULL;

	if (gameStatus == PLAY_SOLUTION &&
		currRecordedTurn != currLevel->getSolution().end() &&
		turn == currRecordedTurn->getTurnNum())
	{ // recorded turn with player actions
		smallInput = currRecordedTurn->getSmall();
		largeInput = currRecordedTurn->getLarge();
		if (currRecordedTurn->wasModeChanged())
			currInput = 'b';
		currRecordedTurn++;
	}

	smallShip.findCarriedItems();
	largeShip.findCarriedItems();

	if (currInput && !isSmallInput(currInput) && !isLargeInput(currInput))
		doCommand(currInput);

	if (largeInput) {
		doCommand(largeInput);
		turnActions.setLarge(largeInput);
		turnActions.setNum(turn);
	}
	if (smallInput) {
		doCommand(smallInput);
		turnActions.setSmall(smallInput);
		turnActions.setNum(turn);
	}
	if (currInput == 'b') {
		turnActions.modeChanged();
		turnActions.setNum(turn);
	}
	if (gameStatus != PLAY_SOLUTION && turnActions.getTurnNum() > 0)
		recordedTurns.push_back(turnActions);
}

void Game::removeGameObjectAtPoint(Point &loc) {

	char c = board.getChar(loc);

	if (c == 'W') {
		for (vector<EvilShip>::iterator evilship = evilShips.begin(); evilship != evilShips.end(); evilship++) {
			if (evilship->getLocs()[0] == loc) {
				board.erasePoints(evilship->getLocs(), c);
				evilship->isActive = false;
				break;
			}
		}
	}
	else if (c >= '1' && c <= '9') {
		for (vector<Item>::iterator item = items.begin(); item != items.end(); item++) {
			if (item->getSign() == c) {
				board.erasePoints(item->getLocs(), c);
				items.erase(item);
				break;
			}
		}
	}
	else if (c == '@') {
		deadShip = &smallShip;
		setStatus(GAME_OVER);
	}
	else if (c == '#') {
		deadShip = &largeShip;
		setStatus(GAME_OVER);
	}
	else if (c == '*') {
		board.erasePoint(loc, c);
		detonateBomb(loc, BOMB_BLAST_RADIUS);
	}
	else if (c == '+')
		board.erasePoint(loc, c);
}

void Game::detonateBomb(Point &bombLoc, int explosionRadius) {

	vector<Point *> blastWaves;

	// create bomb blast waves dependent on the blast radius
	for (int i = 1; i <= explosionRadius; i++) {
		Point *blastWave = new Point[i * 8];
		blastWaves.push_back(blastWave);
	}
	
	// find the points which are affeced by each blast wave
	for (int waveNum = 0; waveNum < explosionRadius; waveNum++) {
		int pointsPerSide = (waveNum + 2) * 2 - 1;
		int pointsEntered = 0;
		for (int i = 0; i < pointsPerSide; i++) {
			if (i == 0) {
				for (int j = 1; j < pointsPerSide - 1; j++) { // add pointsPerSide - 2 points of the column to the arr
					blastWaves[waveNum][pointsEntered++] = Point(X_WRAP(bombLoc.getX() - waveNum - 1),
																 Y_WRAP(bombLoc.getY() - waveNum - 1 + j)); // left column
					blastWaves[waveNum][pointsEntered++] = Point(X_WRAP(bombLoc.getX() + waveNum + 1),
																 Y_WRAP(bombLoc.getY() - waveNum - 1 + j)); // right column
				}
			}
			blastWaves[waveNum][pointsEntered++] = Point(X_WRAP(bombLoc.getX() - waveNum - 1 + i),
														 Y_WRAP(bombLoc.getY() - waveNum - 1)); // add top row
			blastWaves[waveNum][pointsEntered++] = Point(X_WRAP(bombLoc.getX() - waveNum - 1 + i),
														 Y_WRAP(bombLoc.getY() + waveNum + 1)); // add bottom row
		}
	}

	// destroy objects in the blast radius
	removeGameObjectAtPoint(bombLoc);
	for (int waveNum = 0; waveNum < explosionRadius; waveNum++) {
		for (int i = 0; i < (waveNum + 1) * 8; i++) {
			removeGameObjectAtPoint(blastWaves[waveNum][i]);
		}
	}

	explosions.push_back(blastWaves);
	currBlastWave.push_back(0);
}

void Game::animateBombExplosion(size_t explosionNum) {
	
	if (currBlastWave[explosionNum] < explosions[explosionNum].size()) {
		for (size_t i = 0; i < (currBlastWave[explosionNum] + 1) * 8; i++) {
			Point &p = explosions[explosionNum][currBlastWave[explosionNum]][i];
			if (board.getChar(p) != 'X')
				board.printPoint(p, (char)177, LIGHTRED);
		}
	}

	// remove trail from the previous blast
	for (size_t i = 0; i < (currBlastWave[explosionNum] * 8); i++) {
		Point &p = explosions[explosionNum][currBlastWave[explosionNum] - 1][i];
		board.erasePoint(p, (char)177);
	}
	
	// blast radius has been reached, animation over
	if (currBlastWave[explosionNum] == explosions[explosionNum].size()) {
		for (int i = 0; i < BOMB_BLAST_RADIUS; i++)
			delete explosions[explosionNum][i];
		explosions[explosionNum].clear();
		explosions.erase(explosions.begin() + explosionNum);
		currBlastWave.erase(currBlastWave.begin() + explosionNum);
	}
	else
		currBlastWave[explosionNum]++;
} 

bool Game::checkBombs(vector<Point> dests) {
	bool noBombs = true;
	for (vector<Point>::iterator point = dests.begin(); point != dests.end(); point++) {
		if (board.getChar(*point) == '*') {
			board.erasePoint(*point, '*');
			detonateBomb(*point, BOMB_BLAST_RADIUS);
			noBombs = false;
		}
	}
	return noBombs;
}

Item *Game::getItem(char itemSign) {

	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].getSign() == itemSign)
			return &items[i];
	}
	return NULL;
}

void Game::gameOver() {

	const char *boom = ".oO@*";
	int time = 1500;
	int i = 0;
	while (time > 0) {
		board.printPoints(deadShip->getLocs(), boom[i], (Color)( (int)LIGHTRED - (8 * (i % 2)) ));
		Sleep(100);
		time -= 100;
		i = (i + 1) % strlen(boom);
	}

	system("cls");
	ifstream menu("gameover.ttl");
	string line;
	while (!menu.eof()) {
		getline(menu, line);
		cout << line;
	}
	menu.close();
	Sleep(2500);

	clearDataMembers();
	setStatus(MENU);
}

void Game::victory() {

	system("cls");
	ifstream menu("victory.ttl");
	string line;
	while (!menu.eof()) {
		getline(menu, line);
		cout << line;
	}
	menu.close();
	Sleep(3000);

	if (!currLevel->hasBeenSolved() || recordedTurns.back().getTurnNum() < currLevel->getTurnSolved(workingDirectory))
		newBestSolution();

	clearDataMembers();

	// load next level
	currLevel++;
	if (currLevel == levels.end())
		setStatus(MENU);
	else {
		loadBoard(currLevel->getName());
		printBoard();
		setStatus(IN_GAME);
	}
}

void Game::newBestSolution() {

	system("cls");
	ofstream newSolution;

	string solutionName = currLevel->getName();
	solutionName[solutionName.size() - 1] = 's';

	newSolution.open(solutionName.c_str(), ios_base::trunc);

	gotoxy(5, 1);	cout << "Well done! You have set a new best record for this level!";
	gotoxy(10, 3);	cout << "Please enter your name: ";

	string nameOfSolver;
	getline(cin, nameOfSolver);

	newSolution << "ScreenID=" << currLevel->getNum() << '\n';
	newSolution << "NameOfSolver=" << nameOfSolver << '\n';

	writeSolutionToFile(newSolution);
	currLevel->setSolution(true);
	newSolution.close();
}

void Game::writeSolutionToFile(ofstream &fileName) {

	for (size_t i = 0; i < recordedTurns.size(); i++) {
		TurnActions &currTurn = recordedTurns[i];
		if (i > 0)
			fileName << '\n';
		fileName << currTurn.getTurnNum() << ": ";
		if (currTurn.getSmall())
			fileName << currTurn.getSmall();
		if (currTurn.getLarge())
			fileName << currTurn.getLarge();
		if (currTurn.wasModeChanged())
			fileName << 'b';
	}
}

void Game::playSolution() {

	if (!currLevel->hasBeenSolved()) {
		gotoxy(30, 14);	cout << "This level has not been solved yet!";
		Sleep(1500);
		gotoxy(30, 14);	cout << "                                   ";
		return;
	}

	clearDataMembers();
	currLevel->buildSolution(workingDirectory);

	loadBoard(currLevel->getName());
	printBoard();

	currRecordedTurn = currLevel->getSolution().begin();
	setStatus(PLAY_SOLUTION);
}


void Game::playMenu() {

	setColor(LIGHTGRAY);
	ifstream menu("pausemenu.ttl");
	string line;
	for (size_t i = 6; !menu.eof(); i++) {
		getline(menu, line);
		gotoxy(23, i);
		cout << line;
	}
	menu.close();
	setColor(WHITE);

	char c = _getch();
	while (c != '1' && c != '2' && c != '3' && c != '8' && c != '9' && c != ESC)
		c = _getch();

	int selection = c;
	if (c != ESC)
		selection -= '0';

	switch (selection) {

	case RESTART_LEVEL: 
		loadBoard(currLevel->getName());
		printBoard();
		setStatus(IN_GAME);
		break;

	case SAVE_AND_EXIT: 
		saveGame();
		break;

	case SHOW_SOLUTION: 
		playSolution();	
		break;

	case BACK_TO_MENU:	
		clearDataMembers();
		setStatus(MENU);
		break;

	case EXIT:
		setStatus(QUIT);
		break;

	case ESC:
		printBoard();
		if (gameStatus == IN_GAME_PLAY_MENU)
			setStatus(IN_GAME);
		else setStatus(PLAY_SOLUTION);
		break;
	}
}

void Game::mainMenu() {

	system("cls");
	ifstream menu("mainmenu.ttl");
	string line;
	while (!menu.eof()) {
		getline(menu, line);
		cout << line;
	}
	menu.close();
	gotoxy(0, 0);

	char c = _getch();
	while (c != '1' && c != '2' && c != '3' && c != '4' && c != '5'&& c != '9')
		c = _getch();

	int selection = c - '0';

	switch (selection) {

	case START_NEW_GAME:		startNewGame();		break;
	case SHOW_INSTRUCTIONS:		showGameGuide();	break;
	case CHECK_BOARD_VALIDITY:	validateLevel();	break;
	case LOAD_GAME:				loadGame();			break;
	case START_FROM_LEVEL:		loadLevel();		break;
	case EXIT:					setStatus(QUIT);	break;
	}
}

void Game::startNewGame() {

	if (levels.empty()) {
		gotoxy(30, 14);	cout << "ERROR: No levels exist!";
		Sleep(1500);
		gotoxy(30, 14);	cout << "                       ";
		return;
	}
	currLevel = levels.begin();
	loadBoard(currLevel->getName());
	printBoard();
	setStatus(IN_GAME);
}

void Game::showGameGuide() {

	system("cls");
	ifstream instructions("guide.ttl");
	string line;
	while (!instructions.eof()) {
		getline(instructions, line);
		cout << line;
	}
	instructions.close();
	vector<Point> largePoints;
	largePoints.push_back(Point(65, 9));
	largePoints.push_back(Point(65, 10));
	largePoints.push_back(Point(66, 9));
	largePoints.push_back(Point(66, 10));
	board.printPoints(largePoints, '#', YELLOW);

	// small ship
	board.printPoint(Point(65, 12), '@', LIGHTMAGENTA);
	board.printPoint(Point(66, 12), '@', LIGHTMAGENTA);
	// exit
	board.printPoint(Point(65, 14), 'X', LIGHTGREEN); 
	
	board.printPoint(Point(65, 18), '1', (Color)1);
	board.printPoint(Point(65, 22), '9', (Color)9);
	_getch();
}

void Game::validateLevel() {

	string filepath;
	Level levelToValidate;
	vector<string> errors;

	system("cls");
	cout << "File you would like to validate: ";
	getline(cin, filepath);
	if (filepath.length() < 5 || filepath.substr(filepath.length() - 4) != ".spg")
		filepath += ".spg";

	if (levelToValidate.loadFromFile(workingDirectory, filepath, errors)) 
	{
		vector<Level> levels = loadLevels(findFiles(workingDirectory + "*.spg")); // get levels in curr directory
	
		for (size_t i = 0; i < levels.size(); i++)  {
			Level &currLevel = levels[i];
			if (currLevel.getNum() == levelToValidate.getNum() &&
				currLevel.getFullPath() != levelToValidate.getFullPath()) 
			{
				errors.push_back(string("Screen ID already exists in file: ") + currLevel.getName());
			}
		}

		loadBoard(filepath);
		validateBoard(errors);
	}

	cout << '\n';

	if (errors.size() == 0) 
		cout << "Validated successfully!\n";
	else {
		cout << "Validation errors:\n\n";
		for (size_t i = 0; i < errors.size(); i++)
			cout << '\t' << i+1 << ". " << errors[i] << '\n';
	}	
	cout << "\nPress any key to return to the menu...";
	_getch();
}

bool Game::validateBoard(vector<string> &errors) {

	size_t prevNumErrors = errors.size();

	if (!board.doesHaveExit())
		errors.push_back("Board is missing an exit");

	if (!smallShip.isValid())
		errors.push_back("Small ship has invalid size or shape");
	if (!largeShip.isValid())
		errors.push_back("Large ship has invalid size or shape");

	for (size_t i = 0; i < items.size(); i++) {
		if (!items[i].isValid())
			errors.push_back(string("Item ") + items[i].getSign() + string(" has invalid shape") );
	}

	return errors.size() == prevNumErrors;
}

void Game::saveGame() {

	system("cls");
	ofstream saveFile;

	gotoxy(1, 1); cout << "Save name: ";
	string saveName;
	getline(cin, saveName);
	saveName += ".spp";

	ifstream fileExists((workingDirectory + saveName).c_str());
	if (fileExists.good()) {
		gotoxy(1, 3);	cout << "A save file with this name already exists. Overwrite? y/n";
		char c = _getch();
		while (c != 'y' && c != 'n')
			c = _getch();
		fileExists.close();
		if (c == 'n') {
			printBoard();
			currLevel->printInfo();
			return;
		}
	}

	saveFile.open((workingDirectory + saveName).c_str());

	saveFile << "ScreenID=" << currLevel->getNum() << '\n';

	for (int y = Y_MIN; y <= Y_MAX; y++) {
		saveFile << board.getLine(y) << "\n";
	}

	writeSolutionToFile(saveFile);
	saveFile.close();
	setStatus(MENU);
}

void Game::loadGame() {

	system("cls");

	vector<Level> savedGames = loadLevels(findFiles(workingDirectory + "*.spp"));

	if (savedGames.empty()) {
		gotoxy(32, 11);	cout << "No files found";
		Sleep(1500);
		return;
	}
	int selection;

	char ch = scrollingMenu(savedGames, "Load game", selection);

	if (ch == ESC) {
		setStatus(MENU);
		return;
	}
		
	string saveName = savedGames[selection].getName();
	loadBoard(saveName);
	printBoard();
	// find the corresponding level
	for (vector<Level>::iterator level = levels.begin(); level != levels.end(); level++) {
		if (savedGames[selection].getNum() == level->getNum()) {
			currLevel = level;
			level = levels.end() - 1;
		}
	}
	currLevel->printInfo();
	setStatus(IN_GAME);
}

void Game::loadBoard(string levelName) {

	clearDataMembers();
	ifstream file;

	file.open((workingDirectory + levelName).c_str());

	board.loadFromFile(file);
	
	if (levelName[levelName.size() - 1] == 'p')  // if loading a saved game, update recordedTurns
		recordedTurns = Level::readSolutionFromFile(file);
	
	file.close();

	updateDataMembers();
}


char Game::scrollingMenu(vector<Level> &files, const string title, int &selection) {

	gotoxy(1, 1); cout << title;
	gotoxy(1, 2); cout << string(title.length(), '=');
	gotoxy(1, 21); cout << "Use the w and x keys to scroll between files and press enter to select";

	size_t cursorLoc = 0, currFirst = 0, prevFirst = -1;
	char ch;

	do {
		if (currFirst != prevFirst) {
			for (size_t i = currFirst; i < 16 + currFirst && i < files.size(); i++) {
				gotoxy(4, i + 4 - currFirst);
				cout << i + 1 << ". " << files[i].getName();
				if (prevFirst != -1) {
					int nameLenDiff = files[i + prevFirst - currFirst].getName().length() - files[i].getName().length();
					if (nameLenDiff > 0)
						cout << string(nameLenDiff, ' ');
				}

				if (files[i].hasBeenSolved()) {
					gotoxy(70, i + 4); cout << "*SOLVED*";
				}
			}
			prevFirst = currFirst;
		}

		gotoxy(1, cursorLoc - currFirst + 4);	cout << "=>";

		ch = _getch();

		gotoxy(1, cursorLoc - currFirst + 4);	cout << "  ";

		if (ch == 'x' && cursorLoc < files.size() - 1)
			cursorLoc++;
		else if (ch == 'w' && cursorLoc > 0)
			cursorLoc--;

		if (cursorLoc < currFirst)
			currFirst = cursorLoc;
		else if (cursorLoc > currFirst + 15)
			currFirst = cursorLoc - 15;

	} while (ch != ESC && ch != ENTER);

	if (ch == ENTER)
		selection = cursorLoc;

	return ch;
}

void Game::loadLevel() {

	system("cls");

	levels = loadLevels(findFiles(workingDirectory + "*.spg"));
	if (levels.empty()) {
		gotoxy(28, 11);	cout << "There are no saved games!";
		Sleep(1500);
		return;
	}
	int selection;

	char ch = scrollingMenu(levels, "Load level", selection);

	if (ch == ESC) {
		setStatus(MENU);
		return;
	}
	
	currLevel = levels.begin() + selection;
	loadBoard(currLevel->getName());
	printBoard();
	setStatus(IN_GAME);
}

vector<Level> Game::loadLevels(vector<string> fileNames) {

	vector<string> errors;
	vector<Level> levels;
	set<unsigned int> screenIds; // keep track of screenIds seen so far, don't add duplicate ids
	
	for (vector<string>::const_iterator fileName = fileNames.begin(); fileName != fileNames.end(); fileName++) {
		Level level;
		
		if (level.loadFromFile(workingDirectory, *fileName, errors))
		{
			if (fileName->at(fileName->size() - 1) == 'g')  // validate spg files
			{
				if (screenIds.find(level.getNum()) != screenIds.end())
					continue; // duplicate screenId

				loadBoard(*fileName);
				if (!validateBoard(errors))
					continue; // board is invalid

				screenIds.insert(level.getNum());
			}

			levels.push_back(level);
		}
	}

	sort(levels.begin(), levels.end(), Level::sortByNum);
	return levels;
}

double Game::distance(Point &a, Point &b) {

	return sqrt(pow((double)(a.getX() - b.getX()), 2) + pow((double)(a.getY() - b.getY()), 2));
}

Point Game::findClosestShipPoint(Point loc) {

	PlayerShip *currShip;
	PlayerShip *ships[2] = { &smallShip, &largeShip };
	Point currTarget, target;
	double minDist = 99999, currDist;
	int shiftDir;

	for (int i = 0; i < 2; i++) {
		currShip = ships[i];
		if (!currShip->isActive)
			continue;

		for (int j = 0; j < currShip->getSize(); j++) {
			Point origTarget = currShip->getLocs()[j];

			currTarget = origTarget;
			currDist = distance(loc, currTarget);

			if (currDist < minDist) {
				minDist = currDist;
				target = currTarget;
			}

			shiftDir = (loc.getX() < origTarget.getX()) ? -1 : 1;
			currTarget = Point(origTarget.getX() + shiftDir * X_RANGE, origTarget.getY());
			currDist = distance(loc, currTarget);

			if (currDist < minDist) {
				minDist = currDist;
				target = currTarget;
			}

			shiftDir = (loc.getY() < origTarget.getY()) ? -1 : 1;
			currTarget = Point(origTarget.getX(), origTarget.getY() + shiftDir * (Y_MAX + 1));
			currDist = distance(loc, currTarget);

			if (currDist < minDist) {
				minDist = currDist;
				target = currTarget;
			}
		}
	}

	return target;
}