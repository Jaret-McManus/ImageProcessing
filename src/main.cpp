#include <iostream>
#include <fstream>
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
		std::cerr << "Unable to open file" << std::endl;
		return 1;
	}

	// choose function
	std::function<Pixel24_t(int, int, Matrix_t<Pixel24_t>&)> func;
	std::string function_arg = argv[3] ? std::string(argv[3]) : "";
	int curr_arg = 4; // just read the 4rd arg, possibly more 
	if (function_arg == "--box-blur") {
		if (argc >= 5) {
			char *end;
			int radius = strtol(argv[4], &end, 10);
			if (end == argv[4] || *end != '\0' || errno == ERANGE) {
				std::cerr << std::format("Error converting '{}' to an integer for box blur radius!\n", argv[4]);
				return 1;
			}

			func = box_blur_radius(radius);
			curr_arg++;
		} else {
			func = box_blur;
		}
	} else if (function_arg == "--box-blur-err") {
		func = box_blur_err;
	} else if (function_arg == "--gray") {
		func = grayscale;
	} else if (function_arg == "--blue") {
		func = blue;
	} else if (function_arg == "--green") {
		func = green;
	} else if (function_arg == "--red") {
		func = red;
	} else if (function_arg == "--identity") {
		func = identity;
	} else {
		std::cerr << std::format("Invalid function flag!: ({})\n", argv[3]);
		return 1;
	}

	// read header
	auto bm_hdr = read_bitmap_header(file_stream);
	if (bm_hdr == nullptr) return 1;

	auto bm_info_hdr = read_bitmap_info_header(file_stream);
	if (bm_info_hdr == NULL) return 1;

	// std::cout << std::format("BM Header:\n{}\nBM info hdr:\n{}\n", bm_hdr->to_str(), bm_info_hdr->to_str());

	// set pixel array
	Matrix_t<Pixel24_t> pixel_array;
	set_pixel_array(pixel_array, bm_hdr, bm_info_hdr, file_stream);

	// write new file with headers
	std::ofstream out_stream(output_file_name, std::ios_base::binary | std::ios::out);
	write_headers(out_stream, file_stream, bm_hdr, bm_info_hdr);

	// write pixel data
	bool no_progress = false;
	if (no_progress) {
		write_pixel_array(out_stream, pixel_array, func);
	} else {
		write_pixel_array_progress(out_stream, pixel_array, func);
	}

	file_stream.close();
	out_stream.close();

	return 0;
}