#include <iostream>
#include <fstream>
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

	std::cout << "Size: " << bm_hdr->size << "\n";
	std::cout << "Offset: " << bm_hdr->offset << std::endl;

	file_stream.close();
	return 0;
}