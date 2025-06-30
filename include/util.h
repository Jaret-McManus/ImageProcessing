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
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	uint32_t x_pixels_per_m;
	uint32_t y_pixels_per_m;
	uint32_t colors_used;
	uint32_t important_colors;
} BitmapInfoHeader;

void print_hex(int value);
std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file);
std::unique_ptr<BitmapInfoHeader> read_bitmap_info_header(std::ifstream& file);

// reading functions
uint8_t read_byte(std::ifstream& file);
uint16_t read_two_bytes(std::ifstream& file);
uint32_t read_four_bytes(std::ifstream& file);
void skip_n_bytes(std::ifstream& file, int bytes);


