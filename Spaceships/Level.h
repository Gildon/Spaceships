#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "defs.h"
#include "io_utils.h"
#include "TurnActions.h"

class Level {

	string name;
	string printName;
	string fullPath;
	unsigned int levelNum;
	bool hasSolution;
	string nameOfSolver;
	vector<TurnActions> solution;

	string convertToPrintableName(string fileName);

public:
	unsigned int turnSolved;

	Level():hasSolution(false), turnSolved(0) {};
	const string getName() { return name; };
	const string getPrintName() { return printName; };
	const string getFullPath() { return fullPath; };
	const unsigned int getNum() const { return levelNum; }
	const bool hasBeenSolved() { return hasSolution; };
	unsigned int getTurnSolved(string workingDir);
	vector<TurnActions> &getSolution() { return solution; };
	void buildSolution(string workingDir);
	void setName(const string newName) { name = newName; };
	void setPrintName(string printableName) { printName = printableName; };
	void setFullPath(const string newPath) { fullPath = newPath; };
	void setNum(const unsigned int newNum) { levelNum = newNum; };
	void setSolution(const bool b) { hasSolution = b; };
	void printInfo();
	static vector<TurnActions> readSolutionFromFile(ifstream &fileName);
	static bool sortByNum(const Level &a, const Level &b) { return a.getNum() < b.getNum(); };
	bool loadFromFile(string workingDir, string fileName, vector<string> &errors);	
};

#endif