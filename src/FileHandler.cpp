#include <fstream>
#include <ios>
#include <print>
#include <string>

namespace FileHandler {

std::fstream open_file_or_fail(std::string filename, std::ios_base::openmode mode) {
    std::fstream file{ filename, mode };
    if ( !file.is_open() ) {
        std::println(stderr, "Couldn't open file: {}", filename);
    }

    return file;
}

} // end of FileHandler namespace