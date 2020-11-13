#pragma once
#include "RT1W/material.h"
#include "RT1W/texture.h"

struct hit_record;

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
	lambertian(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& alb, ray& scattered, double& pdf) const override 
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();

		// Catch degenerate scatter direction 
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}
		scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());
		alb = albedo->value(rec.u, rec.v, rec.p);
		pdf = dot(rec.normal, scattered.direction()) / pi;
		return true;
	}

	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const 
	{
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine / pi;
	}
public:
	shared_ptr<texture> albedo;
};

