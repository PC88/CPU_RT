#include "iostream"
#include "RT1W/vec3.h"
#include "Vendor/glm/glm.hpp"

/// This will be an evolving merge of my attempts to understand much of ray tracing by
/// merging my own understanding, with that of Peter Shirley`s RT in one weekend series,
/// and much of the ideas in Ray Tracing from the ground Up --- with a  focus on static CPU side
/// Ray Tracing.

int main()
{
	// Image
	const int image_width = 256;
	const int image_height = 256;

	// Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) 
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) 
		{
			auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}

	std::cerr << "\nDone.\n";
	
	return 0;
}