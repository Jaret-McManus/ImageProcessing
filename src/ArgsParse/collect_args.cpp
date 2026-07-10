#include <cstdint>
#include <iostream>
#include <string>

#include "ArgsParse/collect_args.h"

void ArgsParse::collect_args(int argc, const char* argv[]) {
	for (int32_t arg_index = 1; arg_index < argc; arg_index++ ) {
		std::string arg{argv[arg_index]};
		if (arg.starts_with("--")) {
			std::cout << "Starts with --\n";
		}
	}
}