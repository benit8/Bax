/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** CLI entry point
*/

#include "Bax/Compiler/Compiler.hpp"
#include "Bax/VM/VM.hpp"
#include "Common/Log.hpp"
#include "Common/OptionParser.hpp"
#include "fmt/format.h"
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

int main(int argc, char** argv, char** envp)
{
	// bool run_cli = false;
	bool only_lint = false;
	std::string run_inline;
	std::string entrypoint;
	std::vector<std::string> args;

	OptionParser opt;
	// opt.add_option(run_cli, 'a', nullptr, "Run interactively");
	opt.add_option(run_inline, 'i', "inline", "Run an inline string of code", "code");
	opt.add_option(only_lint, 'l', "lint", "Syntax check only (lint)");
	opt.add_argument(entrypoint, "file", "Parse and execute <file>", false);
	opt.add_argument(args, "args", "Arguments passed to <file>", false);
	if (!opt.parse(argc, argv))
		return EXIT_FAILURE;

	// The VM will run compiled code
	Bax::VM vm(envp);
	// The compiler will compile such code
	Bax::Compiler compiler;

	bool ok = false;
	if (!run_inline.empty())
		ok = compiler.do_string(run_inline);
	else if (!entrypoint.empty())
		ok = compiler.do_file(entrypoint);
	else
		ok = compiler.do_istream(std::cin);

	if (!ok) {
		fmt::print(stderr, "Compilation failed\n");
		return EXIT_FAILURE;
	}

	if (only_lint)
		fmt::print("OK\n");
	else {
		// TODO: Transfer from the compiler to the VM
		// vm.run(/*main_closure*/, args);
	}

	return EXIT_SUCCESS;
}
