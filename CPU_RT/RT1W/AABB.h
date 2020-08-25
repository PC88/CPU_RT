#pragma once
#include "RT1W/rtweekend.h"

class AABB
{
public:
	AABB() {}
	AABB(const point3& a, const point3& b) { _min = a; _max = b; }

	point3 min() const { return _min; }
	point3 max() const { return _max; }

	inline bool hit(const ray& r, double tmin, double tmax) const
	{
		for (int a = 0; a < 3; a++)
		{
			auto invD = 1.0f / r.direction()[a];
			auto t0 = (min()[a] - r.origin()[a]) * invD;
			auto t1 = (max()[a] - r.origin()[a]) * invD;
			if (invD < 0.0f)
				std::swap(t0, t1);
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmax <= tmin)
				return false;
		}
		return true;
	}


	point3 _min;
	point3 _max;
};


AABB surrounding_box(AABB box0, AABB box1)
{
	point3 small(fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z()));

	point3 big(fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z()));

	return AABB(small, big);
}