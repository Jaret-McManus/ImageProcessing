#include <fstream>

namespace FileHandler {

std::fstream open_file_or_fail(std::string filename, std::ios_base::openmode mode);

}