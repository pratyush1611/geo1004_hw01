#include "Point.h"
#include "Rows.h"

struct Bbox {
	Point min, max;
	float size_x() { return max.x - min.x; }
	float size_y() { return max.y - min.y; }
	float size_z() { return max.z - min.z; }

	Bbox() {
		min = Point();
		max = Point();
	}

	Bbox(float minx, float miny, float minz, float maxx, float maxy, float maxz) {
		this->min.x = minx;
		this->min.y = miny;
		this->min.z = minz;
		this->min.x = minx;
		this->min.y = miny;
		this->min.z = minz;
	}

	Bbox(std::vector<Point> points) {
		if (points.size() < 1) throw("Failed to construct Bbox from empty point vector.");
		min = points[0];
		max = points[0];
		for (Point i : points)
		{
			if (i.x < min.x) min.x = i.x;
			if (i.y < min.y) min.y = i.y;
			if (i.z < min.z) min.z = i.z;

			if (i.x > max.x) max.x = i.x;
			if (i.y > max.y) max.y = i.y;
			if (i.z > max.z) max.z = i.z;
		}
	}
};