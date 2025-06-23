#include <iostream>
#include <fstream>
#include <sys/stat.h>


void read_bitmap_header(std::ifstream& file, int *had_error) {
	// 2 bytes header field 0x42 0x4D
	if(file.get() != 0x42 || file.get() != 0x4D) {
		std::cerr << "File doesn't contain valid bit map header!" << std::endl;
		*had_error = 1;
		return;
	}

	// 4 bytes for the size of BMP in bytes

	// 2 bytes reserved

	// 2 bytes reserved

	// 4 bytes of the offset where image pixel data starts
}

void print_hex(int value) {
	std::cout << std::hex;

	std::cout << value;

	std::cout << std::dec;
}

long get_file_size(std::string filename) {
	struct stat stat_buf;
	int ret = stat(filename.c_str(), &stat_buf);
	return ret == 0 ? stat_buf.st_size : -1;
}