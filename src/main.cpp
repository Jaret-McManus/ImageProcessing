#include "ArgsParse/CollectedArgs.h"
#include <iostream>

int main(int argc, const char* argv[]) {
	ArgsParse::CollectedArgs collected_args = ArgsParse::collect_args_or_fail(argc, argv);
	std::cout << "collected_args = " << collected_args << std::endl;
	
	return 0;
}
