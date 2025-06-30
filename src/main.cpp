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

	auto bm_info_hdr = read_bitmap_info_header(file_stream);
	if (bm_info_hdr == NULL) return 1;

	std::cout << "\nHdr size: " << bm_info_hdr->hdr_size << "\n";
	std::cout << "Width: " << bm_info_hdr->width << "\n";
	std::cout << "Height: " << bm_info_hdr->height << "\n";
	std::cout << "Planes: " << bm_info_hdr->planes << "\n";
	std::cout << "Bits per pixel: " << bm_info_hdr->bits_per_pixel << "\n";
	std::cout << "Compression: " << bm_info_hdr->compression << "\n";
	std::cout << "Image size: " << bm_info_hdr->image_size << "\n";
	std::cout << "Horizontal resolution (px/m): " << bm_info_hdr->x_pixels_per_m << "\n";
	std::cout << "Vertical resolution (px/m): " << bm_info_hdr->y_pixels_per_m << "\n";
	std::cout << "Colors used: " << bm_info_hdr->colors_used << "\n";
	std::cout << "Important colors: " << bm_info_hdr->important_colors << "\n";

	file_stream.close();
	return 0;
}