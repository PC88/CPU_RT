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
#include "RT1W/moving_sphere.h"
#include "RT1W/checker_texture.h"

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

// cover image function
hittable_list random_scene() 
{
	hittable_list world;

	auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++) 
	{
		for (int b = -11; b < 11; b++) 
		{
			auto choose_mat = random_double();
			point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) 
			{
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) 
				{
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					auto center2 = center + vec3(0, random_double(0, .5), 0);
					world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) 
				{
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else 
				{
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

hittable_list two_spheres() 
{
	hittable_list objects;

	auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

	return objects;
}

hittable_list two_perlin_spheres() 
{
	hittable_list objects;

	auto pertext = make_shared<noise_texture>();
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	return objects;
}

int main()
{
	// Image
	
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;
	int samples_per_pixel = 100;
	const int max_depth = 50;

	// World

	hittable_list world;

	point3 lookfrom;
	point3 lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;

	switch (0) 
	{
	case 1:
		world = random_scene();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0;
		aperture = 0.1;
		break;

	default:
	case 2:
		world = two_spheres();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0;
		break;
	case 3:
		world = two_perlin_spheres();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0;
		break;
	}

	// Camera

	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	int image_height = static_cast<int>(image_width / aspect_ratio);

	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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