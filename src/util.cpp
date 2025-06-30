#include <iostream>
#include <fstream>
#include <memory>
#include "util.h"

std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file) {
	// 2 bytes header field 0x42 0x4D
	if(file.get() != 0x42 || file.get() != 0x4D) {
		std::cerr << "File doesn't contain valid bit map header!" << std::endl;
		return NULL;
	}

	// 4 bytes for the size of BMP in bytes
	uint32_t size = read_four_bytes(file);

	// 2 bytes reserved
	skip_n_bytes(file, 2);

	// 2 bytes reserved
	skip_n_bytes(file, 2);

	// 4 bytes of the offset where image pixel data starts
	uint32_t offset = read_four_bytes(file);

	BitmapHeader bm_hdr = { .size = size, .offset = offset};
	return std::make_unique<BitmapHeader>(bm_hdr);
}

std::unique_ptr<BitmapInfoHeader> read_bitmap_info_header(std::ifstream& file) {
	uint32_t hdr_size = read_four_bytes(file);

	BitmapInfoHeader bm_info_hdr = {
		.hdr_size = hdr_size
	};
	return std::make_unique<BitmapInfoHeader>(bm_info_hdr);
}

// read in little endian order
uint32_t read_four_bytes(std::ifstream& file) {
	uint32_t read_value = 0;
	for (int i=0; i<4; i++) {
		// bit shift by i * 8 needed to convert little endian
		read_value += file.get() << i * 8; // add read byte
	}

	return read_value;
}

uint8_t read_byte(std::ifstream& file) {
	uint8_t read_value = file.get();
	return read_value;
}

uint16_t read_two_bytes(std::ifstream& file) {
	uint16_t read_value = 0;
	for (int i=0; i<2; i++) {
		// bit shift by i * 8 needed to convert little endian
		read_value += file.get() << i * 8; // add read byte
	}

	return read_value;
}

void skip_n_bytes(std::ifstream& file, int bytes) {
	for (int i=0; i<bytes; i++) {
		file.get(); //discard
	}
}

void print_hex(int value) {
	std::cout << std::hex;

	std::cout << value;

	std::cout << std::dec;
}