/*
** Bax, 2022
** Benoît Lormeau <blormeau@outlook.com>
** CLI entry point
*/

#include "Bax/Runtime/Interpreter.hpp"
#include "Bax/Runtime/VM.hpp"
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
	opt.add_argument(entrypoint, "file", "Parse and execute <file>", true);
	opt.add_argument(args, "args", "Arguments passed to <file>", false);
	if (!opt.parse(argc, argv))
		return EXIT_FAILURE;

	// Make a data structure of the environment.
	std::unordered_map<std::string, std::string> env;
	for (auto it = envp; *it != nullptr; ++it) {
		auto e = *it;
		auto p = strchr(e, '=');
		env.emplace(std::string(e, p), std::string(p + 1));
	}

#if 1
	// The VM will run code
	Bax::VM runner(args, env);
#else
	// The interpreter that will run code
	Bax::Interpreter runner(args, env);
#endif

	Bax::Value result;
	if (!run_inline.empty())
		result = runner.interpret(run_inline);
	else if (!entrypoint.empty())
		result = runner.interpret_file(entrypoint);
	else
		result = runner.interpret_stream(std::cin);

	fmt::print("result = {}\n", result);
	// if (!ok) {
	// 	fmt::print(stderr, "Interpretation failed\n");
	// 	return EXIT_FAILURE;
	// }

	return EXIT_SUCCESS;
}
