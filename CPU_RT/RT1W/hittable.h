#pragma once
#include "RT1W/AABB.h"
#include "RT1W/ray.h"

class material;

struct hit_record 
{
	point3 p;
	vec3 normal;
	std::shared_ptr<material> mat_ptr;
	double t;

	double u;
	double v;

	bool front_face;

	inline void set_face_normal(const ray& r, const vec3& outward_normal) 
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(double t0, double t1, AABB& output_box) const = 0;
};

class translate : public hittable 
{
public:
	translate(shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, AABB& output_box) const override;

public:
	shared_ptr<hittable> ptr;
	vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(moved_r, t_min, t_max, rec))
		return false;

	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);

	return true;
}

bool translate::bounding_box(double t0, double t1, AABB& output_box) const 
{
	if (!ptr->bounding_box(t0, t1, output_box))
		return false;

	output_box = AABB(
		output_box.min() + offset,
		output_box.max() + offset);

	return true;
}