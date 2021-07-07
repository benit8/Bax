/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Main entry point
*/

#include "Common/Log.hpp"
#include "Common/OptionParser.hpp"
#include "Compiler/Compiler.hpp"
#include "VM/VM.hpp"

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

	bool ok = !run_inline.empty()
		? compiler.do_string(run_inline)
		: compiler.do_file(entrypoint);

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
