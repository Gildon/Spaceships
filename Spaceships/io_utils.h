#ifndef _IO_UTILS_H_
#define _IO_UTILS_H_

// you must have a config file, both for windows and for compiling on Linux (MAMA)
// BUT, you need to change the content of the config!
#include "config.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

enum Color
{
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHTGRAY = 7,
	DARKGRAY = 8,
	LIGHTBLUE = 9,
	LIGHTGREEN = 10,
	LIGHTCYAN = 11,
	LIGHTRED = 12,
	LIGHTMAGENTA = 13,
	YELLOW = 14,
	WHITE = 15
};

void gotoxy(int x, int y);
void hideCursor();
void hideScrollBar();
void setColor(Color newColor);
vector<string> findFiles(wchar_t *fileType);
vector<string> findFiles(const string &path);

#ifndef WINDOWS
	int _getch(void);
	int _kbhit(void);
	void Sleep(unsigned long);
	double sqrt(double num);
	double pow(double base, double exponent);
	char *_fullpath(char *,const char *,size_t);
	#define MAX_PATH 260
	#define VK_UP 0
	#define VK_DOWN 0
#else
	#include <conio.h>
	#include <windows.h>
#endif

#endif