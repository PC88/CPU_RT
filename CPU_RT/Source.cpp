#include "iostream"
#include "RT1W/vec3.h"
#include "Vendor/glm/glm.hpp"
#include "RT1W/ray.h"
#include "RT1W/hittable.h"
#include "RT1W/sphere.h"
#include "RT1W/hittable_list.h"
#include "RT1W/camera.h"
#include "RT1W/colour.h"
#include "RT1W/metal.h"
#include "RT1W/lambertian.h"
#include "RT1W/dielectric.h"

/// This will be an evolving merge of my attempts to understand much of ray tracing by
/// merging my own understanding, with that of Peter Shirley`s RT in one weekend series,
/// and much of the ideas in Ray Tracing from the ground Up --- with a  focus on static CPU side
/// Ray Tracing.

double hit_sphere(const point3& center, double radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	/// refactor 
	//auto a = dot(r.direction(), r.direction());
	//auto b = 2.0 * dot(oc, r.direction());
	//auto c = dot(oc, oc) - radius * radius;
	//auto discriminant = b * b - 4 * a*c;

	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant < 0) 
	{
		return -1.0;
	}
	else 
	{
		//return (-b - sqrt(discriminant)) / (2.0*a);
		return (-half_b - sqrt(discriminant)) / a;
	}
}

// depth is added here to stop the recursions from blowing the stack
color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	// the min value here being 0.001 fixes the subtle bug of shadow acne
	if (world.hit(r, 0.001, infinity, rec)) 
	{
		// the replacement of "random_in_unit_sphere()" with "random_unit_vector()" makes the lighting simulate correct lambertian lighting
		//point3 target = rec.p + rec.normal + random_unit_vector();

		// the above commented code is the optimal code, this is just an example of how the first ray tracing papers did it below
		//point3 target = rec.p + random_in_hemisphere(rec.normal);
		//return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);


		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5*(unit_direction.y() + 1.0);
	return (1.0 - t)*color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// World
	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.2, 0.2, 0.8));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left)); // added inner sphere to look hollow
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	camera cam;

	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) 
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) 
		{
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) 
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";
	
	return 0;
}