#include "Level.h"
#include "Game.h"
#include <string.h>

unsigned int Level::getTurnSolved(string workingDir) {

	ifstream levelSolution;

	string levelName = name;
	levelName[levelName.length() - 1] = 's'; // changing .spg to .sps

	levelSolution.open((workingDir + levelName).c_str());

	// go to last line in the file, and read backwards until the beginning of the line
	levelSolution.seekg(-1, ios_base::end);
	bool middleOfLine = true, extraLines = true;
	char c;
	while (extraLines) { // ignore empty lines at the end of file
		levelSolution.get(c);
		if (c != '\n')
			extraLines = false;
		// can use the following in both cases since there are always enough characters in a real turn actions line
		levelSolution.seekg(-3, ios_base::cur);
	}
	// last (relevant) line is reached
	while (middleOfLine) {
		levelSolution.get(c);
		if (c == '\n')
			middleOfLine = false;
		else
			levelSolution.seekg(-2, ios_base::cur);
	}
	
	string lastTurn;
	getline(levelSolution, lastTurn);
	lastTurn = lastTurn.substr(0, lastTurn.find(':'));

	return atoi(lastTurn.c_str());
}

void Level::buildSolution(string workingDir) {

	ifstream savedSolution;

	string levelName = name;
	levelName[levelName.length() - 1] = 's'; // changing .spg to .sps

	savedSolution.open((workingDir + levelName).c_str());

	string solver;
	getline(savedSolution, solver); // skip "ScreenID=" line
	getline(savedSolution, solver); // "NameOfSolver=" line
	
	solver = solver.substr(13, 50);
	nameOfSolver = solver;

	solution = readSolutionFromFile(savedSolution);
}

vector<TurnActions> Level::readSolutionFromFile(ifstream &fileName) {

	vector<TurnActions> turns;
	string turn;
	while (!fileName.eof()) {
		TurnActions currTurn;
		getline(fileName, turn);
		if (turn.empty())
			break;
		size_t colonPos = turn.find(':');
		currTurn.setNum(atoi(turn.substr(0, colonPos).c_str()));

		string actions = turn.substr(colonPos + 2);

		for (size_t i = 0; i < actions.size(); i++) {
			char c = actions[i];
			if (Game::isLargeInput(c))
				currTurn.setLarge(c);
			if (Game::isSmallInput(c))
				currTurn.setSmall(c);
			if (c == 'b')
				currTurn.modeChanged();
		}
		turns.push_back(currTurn);
	}
	return turns;
}

bool Level::loadFromFile(string workingDir, string fileName, vector<string> &errors) {

	ifstream file;
	string levelNum;

	file.open((workingDir + fileName).c_str());
	
	if (!file.good()) {
		errors.push_back(string("File not found: ") + fileName);
		return false;
	}

	getline(file, levelNum);
	file.close();

	const size_t textLen = strlen("ScreenID=");
	if (levelNum.size() < textLen || levelNum.compare(0, textLen, "ScreenID=") != 0) {
		errors.push_back("ScreenID not found");
		return false;
	}
	levelNum = levelNum.substr(textLen); // skipping "ScreenID="

	char fullPath[MAX_PATH];
	_fullpath(fullPath, (workingDir + fileName).c_str(), MAX_PATH);

	setName(fileName);
	setPrintName(convertToPrintableName(fileName));
	setFullPath(fullPath);
	setNum(atoi(levelNum.c_str()));

	fileName[fileName.length() - 1] = 's'; // changing to .sps
	file.open((workingDir + fileName).c_str());
	setSolution(file.good()); // checking whether a solution file exists		
	file.close();

	return true;
}

string Level::convertToPrintableName(string fileName) {

	if (fileName.size() <= strlen(".spg"))
		return fileName;

	string printableName = fileName;

	replace(printableName.begin(), printableName.end(), '_', ' ');
	printableName.resize(printableName.size() - strlen(".spg"));

	printableName[0] = toupper(printableName[0]);

	bool newWord = false;
	for (size_t i = 1; i < printableName.length(); ++i) {
		printableName[i] = tolower(printableName[i]);
		if (newWord) {
			printableName[i] = toupper(printableName[i]);
			newWord = false;
		}
		if (printableName[i] == ' ')
			newWord = true;
	}
	return printableName;
}

void Level::printInfo() {

	setColor(LIGHTGRAY);
	gotoxy(25, 24);	cout << "Level: " << printName;
	gotoxy(65, 24);	cout << "Record: ";
	if (hasSolution) {
		string workingDirectory = fullPath;
		workingDirectory.resize(workingDirectory.size() - name.size());
		cout << getTurnSolved(workingDirectory);
	}
	else
		cout << "N/A";
}