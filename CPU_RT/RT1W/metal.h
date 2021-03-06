#pragma once
#include "RT1W\material.h"

struct hit_record;

class metal :
	public material
{
public:
	metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}


	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
	{
		// const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		srec.attenuation = albedo;
		return (dot(srec.specular_ray.direction(), rec.normal) > 0);
	}

public:
	color albedo;
	double fuzz;
};