#include "pixel.h"

// color functions
std::function<Pixel24_t(int,int,Matrix_t<Pixel24_t>&)> color_ratio(float r, float g, float b) {
	return [=] (int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
		Pixel24_t pixel = pixel_array[i][j];

		return Pixel24_t(
			static_cast<int>(std::round(pixel.red * r)),
			static_cast<int>(std::round(pixel.green * g)),
			static_cast<int>(std::round(pixel.blue * b))
		);
	};
}

Pixel24_t red(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	return color_ratio(1.0, 0.0, 0.0)(i, j, pixel_array);
}

Pixel24_t green(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	return color_ratio(0.0, 1.0, 0.0)(i, j, pixel_array);
}

Pixel24_t blue(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	return color_ratio(0.0, 0.0, 1.0)(i, j, pixel_array);
}

Pixel24_t grayscale(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	// using this algorithm for nice grayscale to human eye:
	//		value = 0.3*red + 0.59*green + 0.11*blue
    Pixel24_t pixel = pixel_array[i][j];
	float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
	uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

	return Pixel24_t(gray_value, gray_value, gray_value);
}

// blur funcs
std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> box_blur_radius(int radius) {
	
	return [=](int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
		double red_total   = 0;
		double green_total = 0;
		double blue_total  = 0;

		uint32_t num_pixels = 0;
		for (int32_t x=-radius; x<=radius; x++) {
			for (int32_t y=-radius; y<=radius; y++) {
				int32_t x_offset = static_cast<int32_t>(i) + x;
				int32_t y_offset = static_cast<int32_t>(j) + y;

				bool x_out_of_bounds = x_offset < 0 || x_offset >= std::ssize(pixel_array);
				bool y_out_of_bounds = y_offset < 0 || y_offset >= std::ssize(pixel_array[i]);
				if (x_out_of_bounds || y_out_of_bounds) {
					// skip to avoid segfault
					continue;
				}

				Pixel24_t pixel = pixel_array[i+x][j+y];
				red_total   += pixel.red;
				green_total += pixel.green;
				blue_total  += pixel.blue;
				num_pixels++;
			}
		}

		return Pixel24_t(
			static_cast<uint8_t>(red_total / num_pixels),
			static_cast<uint8_t>(green_total / num_pixels),
			static_cast<uint8_t>(blue_total / num_pixels)
		);
	};
}

Pixel24_t box_blur(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	uint16_t red_total   = 0;
	uint16_t green_total = 0;
	uint16_t blue_total  = 0;

	for (int32_t x=-4; x<=4; x++) {
		for (int32_t y=-4; y<=4; y++) {
			int32_t x_offset = static_cast<int32_t>(i) + x;
			int32_t y_offset = static_cast<int32_t>(j) + y;
			if (
				(x_offset < 0 || x_offset >= std::ssize(pixel_array)) ||
				(y_offset < 0 || y_offset >= std::ssize(pixel_array[i]))
			) {
				// pretend outside image is black
				continue;
			}

			Pixel24_t pixel = pixel_array[i+x][j+y];
			red_total   += pixel.red;
			green_total += pixel.green;
			blue_total  += pixel.blue;
		}
	}

	return Pixel24_t(red_total / 81, green_total / 81, blue_total / 81);
}

Pixel24_t box_blur_err(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	uint16_t red_total   = 0;
	uint16_t green_total = 0;
	uint16_t blue_total  = 0;

	for (int32_t x=-4; x<=4; x++) {
		for (int32_t y=-4; y<=4; y++) {
			int32_t x_offset = static_cast<int32_t>(i) + x;
			int32_t y_offset = static_cast<int32_t>(j) + y;
			if (
				(x_offset < 0 || x_offset >= std::ssize(pixel_array)) ||
				(y_offset < 0 || y_offset >= std::ssize(pixel_array[i]))
			) {
				// pretend outside image is black
				continue;
			}

			Pixel24_t pixel = pixel_array[i+x][j+y];
			red_total   += pixel.red;
			green_total += pixel.green;
			blue_total  += pixel.blue;
		}
	}

	return Pixel24_t(red_total / 9, green_total / 9, blue_total / 9);
}

// matrix functions