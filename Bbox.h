#include "Point.h"

struct Bbox {
	Point min, max;
	float size_x() { return max.x - min.x; }
	float size_y() { return max.y - min.y; }
	float size_z() { return max.z - min.z; }

	Bbox() {
		min = Point();
		max = Point();
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

	Bbox intersection(Bbox _other) {
		Bbox other = Bbox();
		other.min = _other.min;
		other.max = _other.max;

		if (max.x < other.max.x) other.max.x = max.x;
		if (max.y < other.max.y) other.max.y = max.y;
		if (max.z < other.max.z) other.max.z = max.z;
		if (min.x > other.min.x) other.min.x = min.x;
		if (min.y > other.min.y) other.min.y = min.y;
		if (min.z > other.min.z) other.min.z = min.z;

		return other;
	}
};