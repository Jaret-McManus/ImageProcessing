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

void set_raw_pixel_array(std::vector<std::vector<Pixel24_t>>& raw_pixel_array, std::unique_ptr<BitmapHeader>& bm_hdr, std::unique_ptr<BitmapInfoHeader>& bm_info_hdr, std::ifstream& file) {
	raw_pixel_array.reserve(bm_info_hdr->height); // reserve pixel columns

	// seek to start of pixel data
	file.seekg(bm_hdr->offset, std::ios::beg);
	std::cout << std::format("tellg after seek to offset: {}\n", static_cast<long>(file.tellg()));


	long last_good_g = 0;
	long iter = 0;
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

			if(!file.good()) {
				std::cout << std::format("File not good at {}, {}\ntellg: {}, iter: {}\n", i, j, last_good_g, iter);
				abort();
			}
			last_good_g = static_cast<long>(file.tellg());
			iter++;
			Pixel24_t pixel = {
				.red = red,
				.green = green,
				.blue = blue
			};

			if(i== 0 && j == 0){
				std::cout << std::format("First pixel in func: {}\n", pixel.to_str_hex());
			}

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

void write_pixel_array_grayscale(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

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
void write_pixel_array_red(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = 0;
			row_buffer[bytes_written + 1] = 0;
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

void write_pixel_array_green(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = 0;
			row_buffer[bytes_written + 1] = pixel.green;
			row_buffer[bytes_written + 2] = 0;

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

void write_pixel_array_blue(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = pixel.blue;
			row_buffer[bytes_written + 1] = 0;
			row_buffer[bytes_written + 2] = 0;

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
void write_pixel_array_invert(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = ~pixel.blue;
			row_buffer[bytes_written + 1] = ~pixel.green;
			row_buffer[bytes_written + 2] = ~pixel.red;

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
void write_pixel_array_bgr(std::ofstream& out, std::vector<std::vector<Pixel24_t>>& raw_pixel_array) {
	int height = raw_pixel_array.size();
	int width = raw_pixel_array.front().size();


	for (int i=0; i<height; i++) {
		uint32_t bytes_written = 0;
		char row_buffer[width * 3];
		for (int j=0; j<width; j++) {
			Pixel24_t pixel = raw_pixel_array[i][j];

			// using this algorithm for nice grayscale to human eye:
			//		value = 0.3*red + 0.59*green + 0.11*blue
			float gray_value_f = 0.3 * pixel.red + 0.59 * pixel.green + 0.11 * pixel.blue;
			uint8_t gray_value = static_cast<int>(std::round(gray_value_f));

			// output gray bytes for 3 channels
			row_buffer[bytes_written + 0] = pixel.red;
			row_buffer[bytes_written + 1] = pixel.green;
			row_buffer[bytes_written + 2] = pixel.blue;

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