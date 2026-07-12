#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <set>

#include "ArgsParse/CollectedArgs.h"

ArgsParse::CollectedArgs ArgsParse::collect_args_or_fail(int argc, const char* argv[]) {
	int32_t arg_index = 1;
	std::set<char> invalid_single_flags{};
	std::set<std::string> invalid_long_flags{};
	CollectedArgs collected_args{};

	while ( arg_index < argc ) {
		std::string arg{ argv[arg_index] };

		if ( arg.starts_with("--") ) {
			check_long_flags(arg_index, argv, collected_args, invalid_long_flags);
		} else if ( arg.starts_with("-")) {
			check_single_flags(arg_index, argv, collected_args, invalid_single_flags);
		}
		arg_index++;
	}

	if ( !collected_args.all_flags_valid ) {
		print_invalid_flags(invalid_single_flags, invalid_long_flags);
	}

	return collected_args;
}

void ArgsParse::check_single_flags(int& arg_index, const char* argv[], CollectedArgs &collected_args, std::set<char> &invalid_flags) {
	const char* c_string = argv[arg_index];
	std::string string{c_string + 1}; // "-" has a length of 1, skip it
	
	for( auto& ch : string ) {
		switch ( ch ) {
			case 'h':
				collected_args.help_flagged = true;
				break;
			default:
				collected_args.all_flags_valid = false;
				invalid_flags.insert(ch);
		}
	}
}

void ArgsParse::check_long_flags(int& arg_index, const char* argv[], CollectedArgs &collected_args, std::set<std::string> &invalid_flags) {
	const char* c_string = argv[arg_index];
	std::string string{ c_string + 2 }; // "--" has a length of 2, skip it
	
	if ( string.compare("help") == 0 ) {
		collected_args.help_flagged = true;
	} else {
		collected_args.all_flags_valid = false;
		invalid_flags.insert( string );
	}
}

void ArgsParse::print_invalid_flags(const std::set<char> &invalid_single_flags, const std::set<std::string> &invalid_long_flags) {
	for ( char flag : invalid_single_flags ) {
		std::cerr << std::format("Invalid flag provided: '-{}'\n", flag);
	}
	for ( std::string flag : invalid_long_flags ) {
		std::cerr << std::format("Invalid flag provided: --{}\n", flag);
	}
}