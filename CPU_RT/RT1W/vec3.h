#pragma once
#include <cmath>
#include <iostream>
#include "rtweekend.h"

using std::sqrt;

// include commented out above due to errors, the linkage of globals is global, meaning:
// the definition of the function can be linked, but the declaration cannot - hence the second definition here - PC
double random_double();
double random_double(double min, double max);


class vec3
{
public:

public:
	vec3() : e{ 0,0,0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3 &v) 
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(const double t) 
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(const double t) 
	{
		return *this *= 1 / t;
	}

	double length() const 
	{
		return sqrt(length_squared());
	}

	double length_squared() const 
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline static vec3 random()
	{
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max)
	{
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	// book excludes this function this one is a decent fix
	bool near_zero()
	{
		if (e[0] < 0.1 &&  e[0] >= -0.1)
		{
			if (e[1] < 0.1 &&  e[1] >= -0.1)
			{
				if (e[2] < 0.1 &&  e[2] >= -0.1)
				{
					return true;
				}
			}
		}
		return false;
	}

public:
	double e[3];

};

// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const vec3 &v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v)
{
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t)
{
	return t * v;
}

inline vec3 operator/(vec3 v, double t)
{
	return (1 / t) * v;
}

inline double dot(const vec3 &u, const vec3 &v)
{
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v)
{
	return v / v.length();
}


vec3 random_in_unit_sphere()
{
	while (true)
	{
		auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

vec3 random_unit_vector() 
{
	// this is not optimal definition, but it will do for now
	const double pi = 3.1415926535897932385;

	auto a = random_double(0, 2 * pi);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);
	return vec3(r*cos(a), r*sin(a), z);
}

vec3 random_in_hemisphere(const vec3& normal) 
{
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

// "The reflected ray direction in red is just v+2b. In our design"
vec3 reflect(const vec3& v, const vec3& n) 
{
	return v - 2 * dot(v, n)*n;
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) 
{
	auto cos_theta = dot(-uv, n);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

vec3 random_in_unit_disk() 
{
	while (true) 
	{
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline vec3 random_cosine_direction()
{
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi*r1;
	auto x = cos(phi)*sqrt(r2);
	auto y = sin(phi)*sqrt(r2);

	return vec3(x, y, z);
}


// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB 