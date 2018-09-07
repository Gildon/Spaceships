#include "io_utils.h"

#ifndef WINDOWS
	void gotoxy(int x, int y) {}
	int _getch(void){return 0;}
	int _kbhit(void){return 0;}
	void Sleep(unsigned long){}
	double sqrt(double num) { return num; }
	double pow(double num, double exponent) { return num; }
	vector<string> findFiles(wchar_t *fileType) { return vector<string>(); }
	vector<string> findFiles(const string &path) { return vector<string>(); }
	void hideCursor() {}
	void hideScrollBar() {}
	void setColor(Color newColor) {}
	char *_fullpath(char *,const char *,size_t) { return 0; }
#else
	void gotoxy(int x, int y) {

		HANDLE hConsoleOutput;
		COORD dwCursorPosition;
		cout.flush();
		dwCursorPosition.X = x;
		dwCursorPosition.Y = y;
		hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(hConsoleOutput,dwCursorPosition);
	}
	
	vector<string> findFiles(wchar_t *fileType) {

		WIN32_FIND_DATA ffd;
		HANDLE hFind;

		vector<string> files;

		hFind = FindFirstFile(fileType, &ffd);

		if (hFind == INVALID_HANDLE_VALUE)
			return files;

		char name[MAX_PATH];

		do {
			wcstombs(name, ffd.cFileName, sizeof(name));
			files.push_back(name);
		} while (FindNextFile(hFind, &ffd) != 0);

		return files;
	}

	vector<string> findFiles(const string &path) {
		wchar_t wpath[MAX_PATH];
		mbstowcs(wpath, path.c_str(), MAX_PATH);
		return findFiles(wpath);
	}

	void hideCursor() {

		HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO CURSOR;
		CURSOR.dwSize = 1;
		CURSOR.bVisible = FALSE;
		SetConsoleCursorInfo(myconsole, &CURSOR); //second argument needs pointer
	}

	void hideScrollBar() {

		HANDLE hOut;
		CONSOLE_SCREEN_BUFFER_INFO SBInfo;
		COORD NewSBSize;
		int Status;

		hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		SMALL_RECT r;
		r.Left =
		r.Top = 0;
		r.Right = 80 - 1;
		r.Bottom = 25 - 1;
		SetConsoleWindowInfo(hOut, TRUE, &r);

		GetConsoleScreenBufferInfo(hOut, &SBInfo);
		NewSBSize.X = 80;
		NewSBSize.Y = 25;

		Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
		if (Status == 0) {
			Status = GetLastError();
			cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
			Sleep(1000);
			exit(Status);
		}
	}

	void setColor(Color newColor) {

		HANDLE hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, newColor);
	}

#endif