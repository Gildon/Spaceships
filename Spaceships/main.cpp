#include "io_utils.h"
#include "Game.h"

int main(int argc, char *argv[]) 
{
	hideCursor();
	hideScrollBar();
	setColor(WHITE);

	string directory;

	if (argc > 1)
		directory = argv[1];

	Game(directory).play();

	return 0;
}