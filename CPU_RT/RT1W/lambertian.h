#pragma once
#include "RT1W/material.h"

struct hit_record;

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();
		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo;
		return true;
	}

public:
	color albedo;
};

