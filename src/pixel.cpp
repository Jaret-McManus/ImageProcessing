#include <iostream>
#include "pixel.h"
#include <cassert>


Pixel24_t random_pixel(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	uint r_val = rand() % 256;
	uint g_val = rand() % 256;
	uint b_val = rand() % 256;
	return Pixel24_t(r_val, g_val, b_val);
}

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

Pixel24_t random_ratio(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return color_ratio(r, g, b)(i, j, pixel_array);
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
/* Convolve matrix: out of bounds pixels are retreived by extending the image's border pixels */
std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> apply_matrix_extend(Matrix_t<double> kernel) {
    int width = std::ssize(kernel);
    int height = std::ssize(kernel.front());
    int num_cells = width * height;

    return [=, &kernel] (int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
        int32_t img_width  = std::ssize(pixel_array);
        int32_t img_height = std::ssize(pixel_array.front());

        double red_total   = 0.0;
        double green_total = 0.0;
        double blue_total  = 0.0;
        for (int32_t row = 0; row < width; row++) {
            for (int32_t col = 0; col < height; col++) {
                // actual indices
                int32_t x = -width/2 + row + i, y = -height/2 + col + j;
                
                // handle edge cases
                x = (x < 0) ? 0 : x;                        // snap to left if x < 0
                x = (x >= img_width) ? (img_width - 1) : x; // snap to right

                y = (y < 0) ? 0 : y;                            // snap to top if y < 0
                y = (y >= img_height) ? (img_height - 1) : y;   //snap to bottom
            
                Pixel24_t pixel = pixel_array[x][y];
                red_total   += kernel[row][col] * pixel.red;
                green_total += kernel[row][col] * pixel.green;
                blue_total  += kernel[row][col] * pixel.blue;
            }
        }

        return Pixel24_t(
            static_cast<int>(std::round(red_total)), 
            static_cast<int>(std::round(green_total)), 
            static_cast<int>(std::round(blue_total))
        );
    };
}

Pixel24_t identity(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
    static Matrix_t<double> identity_matrix = {
        {0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0}
    };
    return apply_matrix_extend(identity_matrix)(i, j, pixel_array);
}

Pixel24_t edge_detect(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
    static Matrix_t<double> edge_matrix = {
        {-1.0, -1.0, -1.0},
        {-1.0, +8.0, -1.0},
        {-1.0, -1.0, -1.0}
    };
    return apply_matrix_extend(edge_matrix)(i, j, pixel_array);
}

// Pixel24_t edge_detect(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
//     static Matrix_t<double> edge_matrix = {
//         { 0.0, -1.0,  0.0},
//         {-1.0, +4.0, -1.0},
//         { 0.0, -1.0,  0.0}
//     };
//     return apply_matrix_extend(edge_matrix)(i, j, pixel_array);
// }

Pixel24_t gaussian_blur5x5(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
    static Matrix_t<double> gaussian_matrix = {
        {  1.0 / 256.0,  4.0 / 256.0,  6.0 / 256.0,  4.0 / 256.0,  1.0 / 256.0},
        {  4.0 / 256.0, 16.0 / 256.0, 24.0 / 256.0, 16.0 / 256.0,  4.0 / 256.0},
        {  6.0 / 256.0, 24.0 / 256.0, 36.0 / 256.0, 24.0 / 256.0,  6.0 / 256.0},
        {  4.0 / 256.0, 16.0 / 256.0, 24.0 / 256.0, 16.0 / 256.0,  4.0 / 256.0},
        {  1.0 / 256.0,  4.0 / 256.0,  6.0 / 256.0,  4.0 / 256.0,  1.0 / 256.0}
    };

    if(i == 0 && j == 0) {
        std::cout << std::endl;
        for(int x = 0; x<5;x++){
            std::cout << "[ ";
            for(int y = 0; y<5;y++){
                std::cout << std::format("{:<10} ", gaussian_matrix[x][y]);
            }
            std::cout << "]\n";
        }
    }
    return apply_matrix_extend(gaussian_matrix)(i, j, pixel_array);
}

Pixel24_t empty_cross(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	static double fourth = 1.0 / 4.0; 
	static Matrix_t<double> empty_cross_matrix = {
		{ 0.0,    fourth, 0.0    },
		{ fourth, 0.0,    fourth },
		{ 0.0,    fourth, 0.0    }
	};

	return apply_matrix_extend(empty_cross_matrix)(i, j, pixel_array);
}

Pixel24_t border_only(int i, int j, Matrix_t<Pixel24_t>& pixel_array) {
	static double eighth = 1.0 / 8.0; 
	static Matrix_t<double> empty_cross_matrix = {
		{ eighth, eighth, eighth },
		{ eighth, 0.0, eighth },
		{ eighth, eighth, eighth },
	};

	return apply_matrix_extend(empty_cross_matrix)(i, j, pixel_array);
}

std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> border_only_nxn(int radius) {
	assert(radius > 0);
	int32_t diameter = 2*radius + 1;
	int32_t num_cells = 4*diameter - 4;
	double value = 1.0 / num_cells; 
	Matrix_t<double> border_matrix(diameter);
	for (int32_t i = 0; i < diameter; i++) {
		std::vector<double> row(diameter);
		for (int32_t j = 0; j < diameter; j++) {
			if(i == 0 || i == diameter-1) { // first/last row all values
				row.push_back(value);
			} else if(j == 0 || j == diameter - 1) { //first column always filled 
				row.push_back(value);
			} else {
				row.push_back(0.0);
			}
		}
		border_matrix.push_back(row);
	}

	return [=, &border_matrix](int i, int j, Matrix_t<Pixel24_t>& pixel_array){ return apply_matrix_extend(border_matrix)(i, j, pixel_array); };
}