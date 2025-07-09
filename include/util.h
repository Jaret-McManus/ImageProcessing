#include <iostream>
#include <cstdint>
#include <memory>
#include <format>
#include <cmath>
#include <cstring>
#include <chrono>
#include "pixel.h"

struct BitmapHeader {
	std::string header_field;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;

	std::string to_str() {
		return std::format("{{\n  header_field: {},\n  size: {},\n  reserved1: {},\n  reserved2: {},\n  offset: {}\n}}", header_field, size, reserved1, reserved2, offset);
	}
};

struct BitmapInfoHeader {
	uint32_t hdr_size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	uint32_t x_pixels_per_m;
	uint32_t y_pixels_per_m;
	uint32_t colors_used;
	uint32_t important_colors;

	std::string to_str() {
		return std::format(
			"{{\n  hdr_size: {},\n  width: {},\n  height: {},\n  planes: {}\n  bits_per_pixel: {},\n  compression: {},\n  image_size: {},\n  x_pixels_per_m: {},\n  y_pixels_per_m: {},\n  colors_used: {},\n  important_colors: {}\n}}",
			hdr_size, width, height,
			planes, bits_per_pixel, compression,
			image_size, x_pixels_per_m, y_pixels_per_m,
			colors_used, important_colors
		);
	}
};

std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file);
std::unique_ptr<BitmapInfoHeader> read_bitmap_info_header(std::ifstream& file);
void set_pixel_array(std::vector<std::vector<Pixel24_t>>& pixel_array, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr, std::ifstream& file);

// reading functions
uint8_t read_byte(std::ifstream& file);
uint16_t read_two_bytes(std::ifstream& file);
uint32_t read_four_bytes(std::ifstream& file);
int skip_n_bytes(std::ifstream& file, int bytes);

// writing functions
void write_headers(std::ofstream& out, std::ifstream& in, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr);
void write_n_bytes(std::ofstream& out, std::ifstream& in, int num_bytes);
void write_padding(std::ofstream& out, int padding_len);
void write_pixel_array_grayscale(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& pixel_array);
void write_pixel_array(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& pixel_array, std::function<Pixel24_t(int, int, std::vector<std::vector<Pixel24_t>>&)> calculate_pixel);
void write_pixel_array_progress(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& pixel_array, std::function<Pixel24_t(int, int, std::vector<std::vector<Pixel24_t>>&)> calculate_pixel);