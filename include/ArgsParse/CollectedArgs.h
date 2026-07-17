#include <set>
#include <string>
#include <optional>

namespace ArgsParse {
	struct CollectedArgs {
		bool all_flags_valid = true;
		bool help_flagged = false;
		bool input_filename_flagged = false;
		std::string input_filename{};
		bool output_filename_flagged = false;
		std::string output_filename{};

		void validate_flags_or_fail();
		friend std::ostream& operator<<(std::ostream& os, const CollectedArgs &collected);
	};

	struct RawArgContext {
		const int argc;
		const char **argv;
		int &arg_index;
	};

	CollectedArgs collect_args_or_fail(int argc, const char* argv[]);
	void check_single_flags(RawArgContext raw_arg_context, CollectedArgs& collected_args, std::set<char>& invalid_flags);
	void check_long_flags(RawArgContext raw_arg_context, CollectedArgs& collected_args, std::set<std::string>& invalid_flags);
	void print_invalid_flags(const std::set<char>& invalid_single_flags, const std::set<std::string>& invalid_long_flags);
	std::optional<std::string> collect_filename_optional(RawArgContext &raw_arg_context);

	void display_help();
}
