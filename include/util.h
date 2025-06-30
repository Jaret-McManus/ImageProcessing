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

typedef struct {
	uint32_t hdr_size;
} BitmapInfoHeader;

void print_hex(int value);
std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file);
std::unique_ptr<BitmapInfoHeader> read_bitmap_info_header(std::ifstream& file);

// reading functions
uint8_t read_byte(std::ifstream& file);
uint16_t read_two_bytes(std::ifstream& file);
uint32_t read_four_bytes(std::ifstream& file);
void skip_n_bytes(std::ifstream& file, int bytes);


