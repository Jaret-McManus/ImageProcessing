#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include "util.h"

int main () {
	std::string file_name = "images/blurry_oscar.bmp";
	std::ifstream file_stream (file_name);

	if (!file_stream.is_open()) {
		std::cout << "Unable to open file" << std::endl;
		return 1;
	}

	// read header
	auto bm_hdr = read_bitmap_header(file_stream);
	if (bm_hdr == NULL) return 1;

	auto bm_info_hdr = read_bitmap_info_header(file_stream);
	if (bm_info_hdr == NULL) return 1;

	uint32_t width = bm_info_hdr->width, height = bm_info_hdr->height;
	uint32_t bits_per_pixel = bm_info_hdr->bits_per_pixel;

	std::cout << std::format("{}x{}", width, height) << std::endl;

	std::vector<std::vector<Pixel24_t>> raw_pixel_array;
	set_raw_pixel_array(raw_pixel_array, bm_hdr, bm_info_hdr, file_stream);

	std::ofstream out_stream("./images/new_img.bmp", std::ios_base::binary | std::ios::out);
	write_headers(out_stream, file_stream, bm_hdr, bm_info_hdr);

	write_pixel_array_grayscale(out_stream, raw_pixel_array);
	file_stream.close();
	out_stream.close();
	return 0;
}