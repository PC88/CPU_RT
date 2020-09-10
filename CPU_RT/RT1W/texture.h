#pragma once

#include "RT1W\rtweekend.h"
#include "RT1W\perlin.h"

class texture 
{
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture 
{
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}

	solid_color(double red, double green, double blue)
		: solid_color(color(red, green, blue)) {}

	virtual color value(double u, double v, const vec3& p) const override 
	{
		return color_value;
	}

private:
	color color_value;
};

class noise_texture : public texture 
{
public:
	noise_texture() {}
	noise_texture(double sc) : scale(sc) {}

	virtual color value(double u, double v, const point3& p) const override 
	{
		return color(1, 1, 1) * noise.noise(scale * p);
	}

public:
	perlin noise;
	double scale;
};
