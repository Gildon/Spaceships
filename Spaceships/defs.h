#ifndef _DEFS_H_
#define _DEFS_H_

#include <set>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#define Y_MAX 23
#define X_MAX 79
#define Y_MIN 0
#define X_MIN 0
#define X_RANGE (X_MAX+1)
#define Y_RANGE (Y_MAX+1)
#define X_WRAP(x) ((x + X_RANGE) % X_RANGE)
#define Y_WRAP(y) ((y + Y_RANGE) % Y_RANGE)
#define ESC 27
#define ENTER 13
#define DIRECTION int

#define HORIZONTAL 1
#define VERTICAL -1

#define BOMB_BLAST_RADIUS 4

enum { UP = 1, LEFT, RIGHT, DOWN };

#endif