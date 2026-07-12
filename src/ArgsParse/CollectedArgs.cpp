#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <set>

#include "ArgsParse/CollectedArgs.h"

namespace ArgsParse {

CollectedArgs collect_args_or_fail(int argc, const char* argv[]) {
	int32_t arg_index = 1;
	std::set<char> invalid_single_flags{};
	std::set<std::string> invalid_long_flags{};
	CollectedArgs collected_args{};

	while ( arg_index < argc ) {
		std::string arg{ argv[arg_index] };

		ArgsParse::RawArgContext raw_arg_context{argc, argv, arg_index};
		if ( arg.starts_with("--") ) {
			check_long_flags(raw_arg_context, collected_args, invalid_long_flags);
		} else if ( arg.starts_with("-")) {
			check_single_flags(raw_arg_context, collected_args, invalid_single_flags);
		}
		arg_index++;
	}

	if ( !collected_args.all_flags_valid ) {
		print_invalid_flags(invalid_single_flags, invalid_long_flags);
		exit(-1);
	}



	return collected_args;
}

void check_single_flags(RawArgContext raw_arg_context, CollectedArgs &collected_args, std::set<char> &invalid_flags) {
	const char* c_string = raw_arg_context.argv[raw_arg_context.arg_index];
	std::string string{c_string + 1}; // "-" has a length of 1, skip it
	
	for( auto& ch : string ) {
		switch ( ch ) {
			case 'h':
				collected_args.help_flagged = true;
				break;
			case 'i':
				collected_args.input_filename_flagged = true;
				if ( auto filename = collect_filename_optional(raw_arg_context) ) 
					collected_args.input_filename = *filename;
				break;
			case 'o':
				collected_args.output_filename_flagged = true;
				if ( auto filename = collect_filename_optional(raw_arg_context) ) 
					collected_args.output_filename = *filename;
				break;
			default:
				collected_args.all_flags_valid = false;
				invalid_flags.insert(ch);
		}
	}
}

void check_long_flags(RawArgContext raw_arg_context, CollectedArgs &collected_args, std::set<std::string> &invalid_flags) {
	const char* c_string = raw_arg_context.argv[raw_arg_context.arg_index];
	std::string string{ c_string + 2 }; // "--" has a length of 2, skip it
	
	if ( string.compare("help") == 0 ) 
	{
		collected_args.help_flagged = true;
	} 
	else if ( string.compare("input") == 0 ) 
	{
		collected_args.input_filename_flagged = true;
		if ( auto filename = collect_filename_optional(raw_arg_context) ) 
			collected_args.input_filename = *filename;
	} 
	else if ( string.compare("output") == 0 ) 
	{
		collected_args.output_filename_flagged = true;
		if ( auto filename = collect_filename_optional(raw_arg_context) )
			collected_args.output_filename = *filename;
	} 
	else 
	{
		collected_args.all_flags_valid = false;
		invalid_flags.insert( string );
	}
}

void print_invalid_flags(const std::set<char> &invalid_single_flags, const std::set<std::string> &invalid_long_flags) {
	for ( char flag : invalid_single_flags ) {
		std::cerr << std::format("Invalid flag provided: '-{}'\n", flag);
	}
	for ( std::string flag : invalid_long_flags ) {
		std::cerr << std::format("Invalid flag provided: --{}\n", flag);
	}
}

std::ostream &operator<<(std::ostream &os, const ArgsParse::CollectedArgs &collected) { 
	os << "{\n"
	   << std::format("\tall_flags_valid: {}\n", collected.all_flags_valid)
	   << std::format("\thelp_flagged: {}\n", collected.help_flagged)
	   << std::format("\tinput_filename_flagged: {}\n", collected.input_filename_flagged)
	   << std::format("\tinput_filename: \"{}\"\n", collected.input_filename)
	   << std::format("\toutput_filename_flagged: {}\n", collected.output_filename_flagged)
	   << std::format("\toutput_filename: \"{}\"\n", collected.output_filename)
	   << "}";
	
	return os;
}

std::optional<std::string> collect_filename_optional(RawArgContext &raw_arg_context) {
	if ( raw_arg_context.arg_index + 1 >= raw_arg_context.argc ) {
		return {};
	}

	raw_arg_context.arg_index++;
	std::string filename{ raw_arg_context.argv[raw_arg_context.arg_index] };

	return filename;
}


} // end of ArgsParse namespace