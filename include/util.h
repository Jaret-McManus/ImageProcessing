#include <iostream>

void print_hex(int value);
long get_file_size(std::string filename);
void read_bitmap_header(std::ifstream& file, int *had_error);