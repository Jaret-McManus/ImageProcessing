#include <iostream>
#include <fstream>
#include <ios>
#include <memory>
#include <format>
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
	uint32_t    width = read_four_bytes(file);
	uint32_t   height = read_four_bytes(file);

	uint16_t planes         = read_two_bytes(file);
	uint16_t bits_per_pixel = read_two_bytes(file);

	uint32_t      compression = read_four_bytes(file);
	uint32_t       image_size = read_four_bytes(file);
	uint32_t   x_pixels_per_m = read_four_bytes(file);
	uint32_t   y_pixels_per_m = read_four_bytes(file);
	uint32_t      colors_used = read_four_bytes(file);
	uint32_t important_colors = read_four_bytes(file);

	BitmapInfoHeader bm_info_hdr = {
		.hdr_size = hdr_size,
		.width = width,
		.height = height,
		.planes = planes,
		.bits_per_pixel = bits_per_pixel,
		.compression = compression,
		.image_size = image_size,
		.x_pixels_per_m = x_pixels_per_m,
		.y_pixels_per_m = y_pixels_per_m,
		.colors_used = colors_used,
		.important_colors = important_colors
	};
	return std::make_unique<BitmapInfoHeader>(bm_info_hdr);
}

void set_raw_pixel_array(std::vector<std::vector<Pixel24_t>>& raw_pixel_array, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr, std::ifstream& file) {
	raw_pixel_array.reserve(bm_info_hdr->height); // reserve pixel columns

	// seek to start of pixel data
	file.seekg(bm_hdr->offset, std::ios::beg);

	// start reading
	for (uint i=0; i<bm_info_hdr->height; i++) {
		uint32_t bytes_read = 0;

		raw_pixel_array.emplace_back();
		auto& pixel_row = raw_pixel_array.back();
		pixel_row.reserve(bm_info_hdr->width); // reserve pixel row

		for (uint j=0; j<bm_info_hdr->width; j++) {
			uint8_t  blue = read_byte(file);
			uint8_t green = read_byte(file);
			uint8_t   red = read_byte(file);
			Pixel24_t pixel = {
				.red = red,
				.green = green,
				.blue = blue
			};

			pixel_row.push_back(pixel);
			bytes_read += 3; // 3 bytes per pixel
		}

		// skip and discard padding bits
		skip_n_bytes(file, bytes_read % 4);
	}
}

void write_headers(std::ofstream& out, std::ifstream& in, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr) {
	in.seekg(0, std::ios::beg); // reset cursor

	// write header
	const int HEADER_BUF_SIZE = 14;
	write_n_bytes(out, in, HEADER_BUF_SIZE); // Header is 14 bytes

	// write info header
	write_n_bytes(out, in, bm_info_hdr->hdr_size);

	// copy rest until pixel data
	const int REST_AFTER_HEADERS = bm_hdr->offset - HEADER_BUF_SIZE - bm_info_hdr->hdr_size;
	write_n_bytes(out, in, REST_AFTER_HEADERS);

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

void write_n_bytes(std::ofstream& out, std::ifstream& in, int num_bytes) {
	char buf[num_bytes];

	in.read(&buf[0], num_bytes);
	out.write(&buf[0], num_bytes);
}

void write_padding(std::ofstream& out, int padding_len) {
	char buf[padding_len];
	memset(buf, 0, padding_len);

	out.write(&buf[0], padding_len);
}

void print_hex(int value) {
	std::cout << std::hex;

	std::cout << value;

	std::cout << std::dec;
}

std::string pixel_to_str(Pixel24_t& pixel) {
	return std::format("({}, {}, {})", pixel.red, pixel.green, pixel.blue);
}

std::string pixel_to_hex_str(Pixel24_t& pixel) {
	return std::format("({:x}, {:x}, {:x})", pixel.red, pixel.green, pixel.blue);
}

void write_pixel_array_grayscale(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	std::cout << "in gray" << std::endl;

	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();

	std::cout << std::format("height: {}, width: {}", height, width) << std::endl;
	std::cout << std::format("{} {} {} {}", pixel_to_str(raw_pixel_array[0][0]), pixel_to_str(raw_pixel_array[0][1]), pixel_to_str(raw_pixel_array[0][2]), pixel_to_str(raw_pixel_array[0][3])) << std::endl;
	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			out.put(gray_value);
			out.put(gray_value);
			out.put(gray_value);

			bytes_written += 3;
		}

		// add padding bits
		if (bytes_written % 4 != 0) {
			write_padding(out, 4 - (bytes_written % 4));
		}
	}
}