#ifndef _GAME_H_
#define _GAME_H_

#include "defs.h"
#include "io_utils.h"
#include "EvilShip.h"
#include "SmallShip.h"
#include "Board.h"
#include "Item.h"
#include "Level.h"
#include "TurnActions.h"

typedef enum { MENU, SOLUTION_PLAY_MENU, IN_GAME_PLAY_MENU, IN_GAME, GAME_OVER, VICTORY, PLAY_SOLUTION, QUIT } GameStatus;
enum { START_NEW_GAME = 1, SHOW_INSTRUCTIONS, CHECK_BOARD_VALIDITY, LOAD_GAME, START_FROM_LEVEL, EXIT = 9 };
enum { RESTART_LEVEL = 1, SAVE_AND_EXIT, SHOW_SOLUTION, BACK_TO_MENU = 8 };

class Game {

	GameStatus gameStatus;
	string workingDirectory;
	PlayerShip largeShip;
	SmallShip smallShip;
	vector<EvilShip> evilShips;
	vector<Item> items;
	vector<Level> levels;
	PlayerShip *deadShip;
	vector<vector<Point *>> explosions;
	vector<unsigned int> currBlastWave;
	vector<Level>::iterator currLevel;
	unsigned int turn;
	vector<TurnActions> recordedTurns;
	vector<TurnActions>::iterator currRecordedTurn;
	bool isSlowPushMode;

	void writeSolutionToFile(ofstream &fileName);
	char scrollingMenu(vector<Level> &files, const string title, int &selection);
	void moveShipCommand(PlayerShip &ship, DIRECTION dir);
	void printBottomInfo() const;
	void handleItems();
	void handlePlayerInput();

public:
	Board board;

	// main game functions
	void mainMenu();
	void startNewGame();
	void validateLevel();
	bool validateBoard(vector<string> &errors);
	void showGameGuide();
	void saveGame();
	void loadGame();
	void loadLevel();
	void play();
	void playMenu();

	// data management functions
	Game(string directory);
	void updateDataMembers();
	void clearDataMembers();
	void setStatus(GameStatus newStatus) { gameStatus = newStatus; };
	GameStatus getStatus() { return gameStatus; };
	void removeGameObjectAtPoint(Point &loc);	

	// utility functions
	void printBoard();
	static bool isSmallInput(char input);
	static bool isLargeInput(char input);
	static double distance(Point &a, Point &b);
	void doCommand(char input);
	void loadBoard(string levelName);
	Item *getItem(char itemSign);
	bool isSlowPushingMode() const { return isSlowPushMode; }
	void playTurn();
	void gameOver();
	void victory();
	void newBestSolution();
	void playSolution();
	vector<Level> loadLevels(vector<string> fileNames);
	Point findClosestShipPoint(Point loc); // check which of the ships' points are closest to the given point
	bool checkBombs(vector<Point> dests);
	void detonateBomb(Point &bombLoc, int explosionRadius);
	void animateBombExplosion(size_t explosionNum);
	void changePushMode();
};

#endif