#ifndef _TURNACTIONS_H_
#define _TURNACTIONS_H_

class TurnActions {

	char smallAction, largeAction;
	bool changedPushMode;
	unsigned int turnNum;

public:
	TurnActions():turnNum(0), smallAction(NULL), largeAction(NULL), changedPushMode(false) {};
	char getSmall() { return smallAction; };
	char getLarge() { return largeAction; };
	bool wasModeChanged() { return changedPushMode; };
	unsigned int getTurnNum() { return turnNum; };
	void setSmall(char newSmall) { smallAction = newSmall; };
	void setLarge(char newLarge) { largeAction = newLarge; };
	void modeChanged() { changedPushMode = true; };
	void setNum(unsigned int newNum) { turnNum = newNum; };
};

#endif