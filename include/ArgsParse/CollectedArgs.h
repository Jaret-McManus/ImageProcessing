#include <set>
#include <string>

namespace ArgsParse {
	struct CollectedArgs {
		bool all_flags_valid = true;
		bool help_flagged = false;
	};

	CollectedArgs collect_args_or_fail(int argc, const char* argv[]);
	void check_single_flags(int& arg_index, const char* argv[], CollectedArgs& collected_args, std::set<char>& invalid_flags);
	void check_long_flags(int& arg_index, const char* argv[], CollectedArgs& collected_args, std::set<std::string>& invalid_flags);
	void print_invalid_flags(const std::set<char>& invalid_single_flags, const std::set<std::string>& invalid_long_flags);
}
