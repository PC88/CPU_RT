#pragma once
#include "RT1W/texture.h"
#include "RT1W/pdf.h"

struct scatter_record 
{
	ray specular_ray;
	bool is_specular;
	color attenuation;
	shared_ptr<pdf> pdf_ptr;
};

class material
{
public:
	// not pure virtual: non enforced interface
	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
	{
		return color(0, 0, 0);
	}

	// re-factored 6.11
	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const
	{
		return false;
	}

	// re-factored 6.11
	virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const 
	{
		return 0;
	}
};

double schlick(double cosine, double ref_idx) 
{
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

class diffuse_light : public material 
{
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
	{
		return false;
	}

	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v,
		const point3& p) const override
	{
		if (rec.front_face)
			return emit->value(u, v, p);
		else
			return color(0, 0, 0);
	}

public:
	shared_ptr<texture> emit;
};

class isotropic : public material 
{
public:
	isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
	isotropic(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
	{
		// const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf

		srec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time()); // ray
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p); // colours
		return true;
	}

public:
	shared_ptr<texture> albedo;
};