#define NOMINMAX

#include < filesystem >
#include < iostream >

#include "functions.hpp"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/bmp.hpp>

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

namespace fs = std::filesystem;
namespace bg = boost::gil;

int main(int argc, char ** argv)
{
	size_t LEDCount, mode;
	std::map < std::string, std::string > pixel_arrays;
	std::map < std::string, std::array < size_t, 3 > > array_info;

	std::cout << "Diode count on one end (45/48/other): "; std::cin >> LEDCount;
	std::cout << "Mode of pixel encoding (0/1/2): ";	   std::cin >> mode;
	// If you need to use two or three different modes, you can just make a number of header files,
	// include them all into the .ino file and make ImageProcessor instances for each file.

	if (!(mode >= 0 && mode <= 2)) mode = 0;

	// add exception safety

	fs::path cur_directory = fs::current_path();
	const fs::path img_directory = cur_directory.append("images");

	// Images to be processed MUST be stores in the 'images' folder of the local directory. 
	// This can be changed without problematic repercussions, but little point in doing so.
	// The preprocessor finds all files of .bmp, .jpg, .jpeg and .png formats and converts them. Ignores everything else.
	
	for (auto & image : fs::directory_iterator(img_directory))
	{
		const auto filepath = image.path().string();
		const auto filename = image.path().filename().string();

		const auto ext = image.path().extension();

		bg::rgb8_image_t img_RGB;

		if		(ext == ".png")					  { bg::read_and_convert_image(filepath, img_RGB, bg::png_tag{}); }
		else if (ext == ".jpg" || ext == ".jpeg") { bg::read_and_convert_image(filepath, img_RGB, bg::jpeg_tag{}); }
		else if (ext == ".bmp")					  { bg::read_and_convert_image(filepath, img_RGB, bg::bmp_tag{}); }
		else continue;

		size_t width = img_RGB.width();
		size_t height = img_RGB.height();
		size_t new_width = float(LEDCount) / height * width;
		size_t new_height = LEDCount;

		std::cout << "Processing: " << filename << ", size: " << width << 'x' << height << std::endl;
		std::cout << "\tResizing to: " << new_width << 'x' << new_height << std::endl;

		// Resising to the LEDCount as column size and the appropriate row size.

		std::string flattened_pixels;	
		std::vector < size_t > values;

		bg::rgb8_image_t img_RGB_scaled(new_width, new_height);

		bg::resize_view(bg::const_view(img_RGB), bg::view(img_RGB_scaled), bg::bilinear_sampler{});
		auto view = bg::rotated90cw_view(bg::const_view(img_RGB_scaled));

		// The images in the header file are rotated 90 degrees clockwise, so each 'row' displayed on the staff
		// Or poi is actually a column. It was simpler to do this here.

		for (auto it = view.begin(); it < view.end(); ++it)
		{
			try
			{
				uint8_t R = bg::get_color(*it, bg::red_t{});
				uint8_t G = bg::get_color(*it, bg::green_t{});
				uint8_t B = bg::get_color(*it, bg::blue_t{});

				auto hsv = rgb2hsv({ R, G, B });

				switch (mode)
				{
				case 0:
					values.push_back(hs_16(hsv));
					break;
				case 1:
					values.push_back(hs_8(hsv));
					break;
				case 2:
					values.push_back(hue_8(hsv));
					break;
				}
			}
			catch (const boost::exception & e) {
				values.push_back(0);
			}
		}

		pixel_arrays[filename] = serialise_vector(values, new_width, new_height);
		array_info[filename] = { new_width, new_height, mode };
	}

	dump(pixel_arrays, array_info, "Images.hpp");

	system("pause");

	return EXIT_SUCCESS;
}
