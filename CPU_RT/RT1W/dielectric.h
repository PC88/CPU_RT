#pragma once
#include "RT1W\material.h"

class dielectric :
	public material
{

public:
	dielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
	{
		attenuation = color(1.0, 1.0, 1.0);
		double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

		vec3 unit_direction = unit_vector(r_in.direction());
		vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = ray(rec.p, refracted);
		return true;
	}

	double ref_idx;
};
