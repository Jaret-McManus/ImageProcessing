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

	std::cout << "file size: " << get_file_size(file_name) << std::endl;

	int had_error = 0;
	read_bitmap_header(file_stream, &had_error);


	file_stream.close();
	return 0;
}