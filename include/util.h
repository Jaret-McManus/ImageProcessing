#include <iostream>
#include <cstdint>
#include <memory>

typedef struct BitmapHeader {
	// char header_field[2];
	uint32_t size;
	// int16_t reserved1;
	// int16_t reserved2;
	uint32_t offset;
} BitmapHeader;

void print_hex(int value);
std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file);
uint32_t read_four_bytes(std::ifstream& file);
void skip_n_bytes(std::ifstream& file, int bytes);


