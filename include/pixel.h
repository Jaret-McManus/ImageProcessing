#include <functional>
#include <cstdint>
#include <string>
#include <format>
#include <cmath>

template<class T>
using Matrix_t = std::vector<std::vector<T>>;

struct Pixel24_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	Pixel24_t(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

	std::string to_str() {
		return std::format("({}, {}, {})", red, green, blue);
	}

	std::string to_str_hex() {
		return std::format("({:x}, {:x}, {:x})", red, green, blue);
	}
};

// color ratios
std::function<Pixel24_t(int,int,Matrix_t<Pixel24_t>&)> color_ratio(float r, float g, float b);
Pixel24_t red(int i, int j, Matrix_t<Pixel24_t>& pixel_array);
Pixel24_t green(int i, int j, Matrix_t<Pixel24_t>& pixel_array);
Pixel24_t blue(int i, int j, Matrix_t<Pixel24_t>& pixel_array);
Pixel24_t grayscale(int i, int j, Matrix_t<Pixel24_t>& pixel_array);

// blurs
std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> box_blur_radius(int radius);
Pixel24_t box_blur(int i, int j, Matrix_t<Pixel24_t>& pixel_array);
Pixel24_t box_blur_err(int i, int j, Matrix_t<Pixel24_t>& pixel_array);

// matrix funcs
std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> apply_matrix_extend(Matrix_t<double> kernel);
Pixel24_t identity(int i, int j, Matrix_t<Pixel24_t>& pixel_array);