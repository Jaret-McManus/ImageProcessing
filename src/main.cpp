#include "ArgsParse/CollectedArgs.h"

int main(int argc, const char* argv[]) {
	ArgsParse::CollectedArgs collected_args = ArgsParse::collect_args_or_fail(argc, argv);
	(void) collected_args;
	
	return 0;
}
