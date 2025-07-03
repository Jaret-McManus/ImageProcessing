#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include "util.h"

int main (int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Invalid syntax: must provide source bitmap and output filename!" << std::endl;
		return 1;
	}

	std::string file_name = argv[1];
	std::string output_file_name = argv[2];
	std::ifstream file_stream (file_name, std::ios_base::binary | std::ios::in);

	if (!file_stream.is_open()) {
		std::cout << "Unable to open file" << std::endl;
		return 1;
	}

	// read header
	auto bm_hdr = read_bitmap_header(file_stream);
	if (bm_hdr == nullptr) return 1;

	auto bm_info_hdr = read_bitmap_info_header(file_stream);
	if (bm_info_hdr == NULL) return 1;

	// set pixel array
	std::vector<std::vector<Pixel24_t>> raw_pixel_array;
	set_raw_pixel_array(raw_pixel_array, bm_hdr, bm_info_hdr, file_stream);

	std::ofstream gray_out_stream(output_file_name, std::ios_base::binary | std::ios::out);
	write_headers(gray_out_stream, file_stream, bm_hdr, bm_info_hdr);

	write_pixel_array_grayscale(gray_out_stream, raw_pixel_array);

	file_stream.close();
	gray_out_stream.close();

	return 0;
}