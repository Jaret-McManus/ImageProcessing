#include <iostream>
#include <cstdint>

void print_hex(int value);
void read_bitmap_header(std::ifstream& file, int *had_error);
uint32_t read_four_bytes(std::ifstream& file);
void skip_n_bytes(std::ifstream& file, int bytes);
