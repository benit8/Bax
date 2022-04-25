/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / VM.cpp
*/

#include "Bax/Compiler.hpp"
#include "Bax/Parser.hpp"
#include "Bax/Runtime/VM.hpp"
#include "Common/Log.hpp"
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{

VM::VM(const std::vector<std::string>& args, const std::unordered_map<std::string, std::string>& env)
: Runner(args, env)
{
}

Value VM::interpret(const std::string& source)
{
	auto parser = Parser(Lexer(source));
	auto ast = parser.parse();
	if (!ast) {
		Log::error("Parsing failed");
		return Value(false);
	}

	ast->dump();

	Compiler compiler(*this);
	auto chunk = compiler.compile(ast);
	if (!chunk) {
		Log::error("Compilation failed");
		return Value(false);
	}

	delete ast;

	// auto main = module("Main").symbol("main");
	// if (!main) {
	// 	Log::error("No main symbol was defined");
	// 	return Value(false);
	// }

	// push(new Array(args));
	// push(new Table(env));
	// return run(new Closure(main), 2);

	return Value(true);
}

}
