#pragma once
#include "RT1W/vec3.h"
#include "RT1W/hittable.h"

class pdf 
{
public:
	virtual ~pdf() {}

	virtual double value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};



class cosine_pdf : public pdf 
{
public:
	cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

	virtual double value(const vec3& direction) const override 
	{
		auto cosine = dot(unit_vector(direction), uvw.w());
		return (cosine <= 0) ? 0 : cosine / pi;
	}

	virtual vec3 generate() const override 
	{
		return uvw.local(random_cosine_direction());
	}

public:
	onb uvw;
};

class hittable_pdf : public pdf 
{
public:
	hittable_pdf(shared_ptr<hittable> p, const point3& origin) : ptr(p), o(origin) {}

	virtual double value(const vec3& direction) const override 
	{
		return ptr->pdf_value(o, direction);
	}

	virtual vec3 generate() const override 
	{
		return ptr->random(o);
	}

public:
	point3 o;
	shared_ptr<hittable> ptr;
};