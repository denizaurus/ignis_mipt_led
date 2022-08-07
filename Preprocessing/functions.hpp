#pragma once

#include < map >
#include < string >
#include < fstream >
#include < algorithm >
#include < vector >
#include < sstream >
#include < iomanip >
#include < array >


struct RGB {
	uint8_t R; uint8_t G; uint8_t B;
	// all 0 to 255
};

struct HSV {
	uint8_t H; uint8_t S; uint8_t V;
	// also all 0 to 255. Usually HSV max values are 360/100/100, 
	// but FastLED uses 0-255 and so does the preprocessor.
};


uint16_t hs_16(HSV hsv)
{
	uint16_t C;

	if (hsv.S == 0) {
		hsv.V >= 128 ? C = 1 : C = 0; // white & black
	}
	else C = (hsv.H << 8) + hsv.S;
	return C;
}

uint8_t hs_8(HSV hsv)
{
	uint8_t C; // color
	uint8_t sat_sixths = 6 * hsv.S / 255;
	uint8_t sat_thirds = round(sat_sixths / 2.0f);
	// If I remember correctly, this was done in this way specifically to make a spread of values
	// 0, 1-2, 3-4, 5; which would correspond to 0-3 in sat_thirds
	// 0 is 0-16%, 1 is 16-50, 2 is 50-83, 3 is 83-100% saturation

	if (sat_thirds == 0) {
		hsv.V >= 128 ? C = 255 : C = 254;
		// 255 encodes WHITE, 254 encodes BLACK
	}
	else { 
		// we get rid of the modulus and replace it with the saturation component. 

		if      (hsv.H < 252)	C = (hsv.H / 3) * 3 + sat_thirds - 1;
		else if (hsv.H < 254)   C = 252 + sat_thirds - 1;
		else					C = sat_thirds - 1; // hues 254 and 255 round to red

		// Doesn't properly show a relatively small number of hues in the magenta-pink part of the spectrum,
		// an artifact of the algorithm
	}

	return C;
}

uint8_t hue_8(HSV hsv)
{
	return hsv.H;
}

/**
* @brief Self-explanatory, dumps the processed image data into a header file titled "Images.hpp" in the executable folder.
* TODO: add the option to set the name of the header file. 
*       The only reason why it wasn't done originally is bc it wasn't necessary.
*/
void dump(const std::map < std::string, std::string > & map,
	const std::map < std::string, std::array < size_t, 3 > > & info,
	const std::string & filename)
{
	std::ofstream stream("Images.hpp");

	stream << "#include \"image_processing.hpp\"\n\n";
	stream << "constexpr uint8_t image_count = " << map.size() << ";\n\n";

	std::stringstream image_map;
	std::stringstream pixel_arrays;
	std::stringstream image_info;

	auto it = map.begin(); auto m = info.at(it->first)[2]; 
	// checking to get the right template argument

	image_map << "constexpr entry ";
	
	if (m == 0) { image_map << "< uint16_t >"; }
	else { image_map << "< uint_8t >"; }
	
	image_map << " image_map[" << map.size() << "] {\n";

	image_info << "constexpr entry_info image_info[" << map.size() << "] {\n";

	size_t counter = 0;
	for (auto & i : map)
	{
		auto rows = info.at(i.first)[0];
		auto cols = info.at(i.first)[1];
		auto mode = info.at(i.first)[2];

		image_map  << "\t{ \"" << i.first << "\", " << "arr" << counter << " }";
		image_info << "\t{ \"" << i.first << "\", " << rows  << ", " << cols << ", " << mode << " }";

		if (counter < map.size() - 1)
		{
			image_map  << ",\n";
			image_info << ",\n";
		}

		if (mode == 0){
			pixel_arrays << "constexpr uint16_t ";
		}
		else {
			pixel_arrays << "constexpr uint8_t ";
		}

		pixel_arrays << "arr" << counter << "[" << rows * cols << "] = {\n" << i.second << "\n};\n\n";
		++counter;
	}

	stream << pixel_arrays.str();
	stream << image_map.str()    << "\n};\n\n";
	stream << image_info.str()   << "\n};\n\n";
}

/**
* @brief Spectrum-based RGB conversion algorithm.
*/
HSV rgb2hsv(RGB color)
{
	const float sth = 255 / 6.0f; // one sixth of the spectrum

	float r = color.R / 255.0f;
	float g = color.G / 255.0f;
	float b = color.B / 255.0f;

	float h, s;
	uint8_t v;

	auto  cmax = std::max({ color.R, color.G, color.B });
	auto  cmin = std::min({ color.R, color.G, color.B });
	auto  dif  = cmax - cmin;
	float diff = dif / 255.0f;

	if (dif == 0) h = 0;
	else if (cmax == color.R) {
		h = std::fmod((sth * ((g - b) / diff) + 6*sth), 6*sth);
	}
	else if (cmax == color.G){
		h = std::fmod((sth * ((b - r) / diff) + 2*sth), 6*sth);
	}
	else {
		h = std::fmod((sth * ((r - g) / diff) + 4*sth), 6*sth);
	}

	if (cmax == 0) s = 0;
	else		   s = (diff / (cmax / 255.0f)) * 255.0f;

	v = cmax;

	HSV hsv{ round(h), round(s), v };
	return hsv;
}

/**
* @brief Serialises a vector into a string of a c-style array, as well as formats it. 
*		 This is later passed to the dump function.
*/
std::string serialise_vector(std::vector < size_t > v_1d, size_t rows, size_t cols)
{
	std::stringstream flat;

	for (size_t row = 0; row < rows; ++row)
	{
		flat << "\t\t";

		for (size_t col = 0; col < cols; ++col)
		{
			flat << std::setw(5) << v_1d[row * cols + col];
			if (row != rows - 1 || col != cols - 1) { 
				flat << std::setw(0) << ", "; 
			}
		}
		if (row != rows - 1) flat << "\n";
	}

	return flat.str();
}