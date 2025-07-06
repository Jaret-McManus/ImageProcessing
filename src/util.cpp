#include <iostream>
#include <fstream>
#include <ios>
#include <memory>
#include <format>
#include "util.h"

std::unique_ptr<BitmapHeader> read_bitmap_header(std::ifstream& file) {
	// 2 bytes header field 0x42 0x4D
	char first_char  = file.get();
	char second_char = file.get();
	if(first_char != 0x42 || second_char != 0x4D) {
		std::cerr << "File doesn't contain valid bit map header!" << std::endl;
		return nullptr;
	}

	// 4 bytes for the size of BMP in bytes
	uint32_t size = read_four_bytes(file);

	// 2 bytes reserved, two in a row
	uint16_t reserved1 = read_two_bytes(file);
	uint16_t reserved2 = read_two_bytes(file);

	// 4 bytes of the offset where image pixel data starts
	uint32_t offset = read_four_bytes(file);

	BitmapHeader bm_hdr = {
		.header_field = std::format("{}{}", first_char, second_char),
		.size = size,
		.reserved1 = reserved1,
		.reserved2 = reserved2,
		.offset = offset
	};
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

void set_pixel_array(std::vector<std::vector<Pixel24_t>>& pixel_array, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr, std::ifstream& file) {
	pixel_array.reserve(bm_info_hdr->height); // reserve pixel columns

	// seek to start of pixel data
	file.seekg(bm_hdr->offset, std::ios::beg);

	// start reading
	for (uint i=0; i<bm_info_hdr->height; i++) {
		uint32_t bytes_read = 0;

		pixel_array.emplace_back();
		auto& pixel_row = pixel_array.back();
		pixel_row.reserve(bm_info_hdr->width); // reserve pixel row

		for (uint j=0; j<bm_info_hdr->width; j++) {
			uint8_t  blue = read_byte(file);
			uint8_t green = read_byte(file);
			uint8_t   red = read_byte(file);

			Pixel24_t pixel(red, green, blue);

			pixel_row.push_back(pixel);
			bytes_read += 3; // 3 bytes per pixel
		}

		// skip and discard padding bits
		if (bytes_read % 4)
			skip_n_bytes(file, 4 - (bytes_read % 4));
	}
}

void write_headers(std::ofstream& out, std::ifstream& in, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr) {
	in.seekg(0, std::ios::beg); // reset cursor

	std::cout << std::format("in file at the start of write headers: {}\n", in.good());
	
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

int skip_n_bytes(std::ifstream& file, int num_bytes) {
	std::vector<char> buf(num_bytes);

	file.read(buf.data(), num_bytes);
	if (file.gcount() != num_bytes) {
		std::cerr << std::format("Failed to read {} bytes in skip_n_bytes, only read {}\n", num_bytes, file.gcount());
		return 1;
	}

	return 0;
}

void write_n_bytes(std::ofstream& out, std::ifstream& in, int num_bytes) {
	std::vector<char> buf(num_bytes);

	in.read(buf.data(), num_bytes);
	if (in.gcount() != num_bytes) {
		std::cerr << std::format("Failed to read {} bytes, only read {}\n", num_bytes, in.gcount());
		return;
	}

	out.write(buf.data(), num_bytes);
	if (!out.good()) {
		std::cerr << std::format("failed to write {} bytes\n", num_bytes);
		return;
	}
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

void write_pixel_array_grayscale(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& pixel_array) {
	int height = pixel_array.size();
	int width  = pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = gray_value;
			row_buffer[bytes_written + 1] = gray_value;
			row_buffer[bytes_written + 2] = gray_value;

			bytes_written += 3;
		}

		// write row
		out.write(&row_buffer[0], width * 3);

		// add padding bits
		if (bytes_written % 4 != 0) {
			write_padding(out, 4 - (bytes_written % 4));
		}
	}
}

void write_pixel_array(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& pixel_array, std::function<Pixel24_t(int, int, std::vector<std::vector<Pixel24_t>>&)> calculate_pixel) {
	int height = pixel_array.size();
	int width  = pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = calculate_pixel(i, j, pixel_array);

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = pixel.blue;
			row_buffer[bytes_written + 1] = pixel.green;
			row_buffer[bytes_written + 2] = pixel.red;

			bytes_written += 3;
		}

		// write row
		out.write(&row_buffer[0], width * 3);

		// add padding bits
		if (bytes_written % 4 != 0) {
			write_padding(out, 4 - (bytes_written % 4));
		}
	}
}

Pixel24_t grayscale_pixel(int i, int j, std::vector<std::vector<Pixel24_t>>& pixel_array) {
	// using this algorithm for nice grayscale to human eye:
	//		value = 0.3*red + 0.59*green + 0.11*blue
	Pixel24_t pixel = pixel_array[i][j];
	float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
	uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

	return Pixel24_t(gray_value, gray_value, gray_value);
}

// Pixel24_t blur(int i, int j, std::vector<std::vector<Pixel24_t>>& pixel_array) {
// 	uint16_t red_total   = 0;
// 	uint16_t green_total = 0;
// 	uint16_t blue_total  = 0;

// 	for (int32_t x=-1; x<=1; x++) {
// 		for (int32_t y=-1; y<=1; y++) {
// 			int32_t x_offset = static_cast<int32_t>(i) + x;
// 			int32_t y_offset = static_cast<int32_t>(j) + y;
// 			if (
// 				(x_offset < 0 || x_offset >= std::ssize(pixel_array)) ||
// 				(y_offset < 0 || y_offset >= std::ssize(pixel_array[i]))
// 			) {
// 				// pretend outside image is black
// 				continue;
// 			}

// 			Pixel24_t pixel = pixel_array[i+x][j+y];
// 			red_total   += pixel.red;
// 			green_total += pixel.green;
// 			blue_total  += pixel.blue;
// 		}
// 	}

// 	return Pixel24_t(red_total / 9, green_total / 9, blue_total /9);
// }

Pixel24_t box_blur_err(int i, int j, std::vector<std::vector<Pixel24_t>>& pixel_array) {
	uint16_t red_total   = 0;
	uint16_t green_total = 0;
	uint16_t blue_total  = 0;

	for (int32_t x=-4; x<=4; x++) {
		for (int32_t y=-4; y<=4; y++) {
			int32_t x_offset = static_cast<int32_t>(i) + x;
			int32_t y_offset = static_cast<int32_t>(j) + y;
			if (
				(x_offset < 0 || x_offset >= std::ssize(pixel_array)) ||
				(y_offset < 0 || y_offset >= std::ssize(pixel_array[i]))
			) {
				// pretend outside image is black
				continue;
			}

			Pixel24_t pixel = pixel_array[i+x][j+y];
			red_total   += pixel.red;
			green_total += pixel.green;
			blue_total  += pixel.blue;
		}
	}

	return Pixel24_t(red_total / 9, green_total / 9, blue_total / 9);
}

Pixel24_t box_blur(int i, int j, std::vector<std::vector<Pixel24_t>>& pixel_array) {
	uint16_t red_total   = 0;
	uint16_t green_total = 0;
	uint16_t blue_total  = 0;

	for (int32_t x=-4; x<=4; x++) {
		for (int32_t y=-4; y<=4; y++) {
			int32_t x_offset = static_cast<int32_t>(i) + x;
			int32_t y_offset = static_cast<int32_t>(j) + y;
			if (
				(x_offset < 0 || x_offset >= std::ssize(pixel_array)) ||
				(y_offset < 0 || y_offset >= std::ssize(pixel_array[i]))
			) {
				// pretend outside image is black
				continue;
			}

			Pixel24_t pixel = pixel_array[i+x][j+y];
			red_total   += pixel.red;
			green_total += pixel.green;
			blue_total  += pixel.blue;
		}
	}

	return Pixel24_t(red_total / 81, green_total / 81, blue_total / 81);
}

Pixel24_t blue(int i, int j, std::vector<std::vector<Pixel24_t>>& pixel_array) {
	Pixel24_t pixel = pixel_array[i][j];
	return Pixel24_t(0, 0, pixel.blue);
}

// std::function<Pixel24_t(int,int,std::vector<std::vector<Pixel24_t>>&)> color_ratio(float r, float g, float b) {
// 	return [=,=,&] (int i, int j, std::vector<std::vector<Pixel24_t>>&) {
// 		Pixel24_t pixel = pixel_array[i][j];
// 		float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
// 		uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

// 		return Pixel24_t(gray_value, gray_value, gray_value);
// 	}
// }