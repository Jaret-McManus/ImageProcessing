#include "ArgsParse/CollectedArgs.h"
#include "FileHandler.h"
#include <cstdlib>
#include <ios>
#include <iostream>

int main(int argc, const char* argv[]) {
	ArgsParse::CollectedArgs collected_args = ArgsParse::collect_args_or_fail(argc, argv);
	std::cout << "collected_args = " << collected_args << std::endl;
	
	if ( collected_args.help_flagged ) {
		ArgsParse::display_help();
		exit(0);
	}

	collected_args.validate_flags_or_fail();

	std::fstream input_file = FileHandler::open_file_or_fail(collected_args.input_filename, std::ios_base::in);

	return 0;
}
