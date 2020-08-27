#pragma once
#include "RT1W\colour.h"
#include "RT1W\vec3.h"

class checker_texture :
	public texture
{
public:
	checker_texture() {}

	checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1)
		: even(t0), odd(t1) {}

	checker_texture(color c1, color c2)
		: even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	virtual color value(double u, double v, const point3& p) const override 
	{
		auto sines = sin(10 * p.x())*sin(10 * p.y())*sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;

};
