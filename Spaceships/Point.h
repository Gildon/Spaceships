#ifndef _POINT_H_
#define _POINT_H_

class Point {

	int x, y;

public:
	Point() { x = 0; y = 0; };
	Point(int x1, int y1) { x = x1; y = y1; };
	void setX(int newX) { x = newX; };
	void setY(int newY) { y = newY; };
	int getX() { return x; }
	int getY() { return y; }
	bool operator<(const Point& p) const { // needed for using Point as a key in std::map
		return y < p.y || (y == p.y && x < p.x);
	};
	bool operator==(const Point &p) const {
		return x == p.x && y == p.y;
	}
};

#endif