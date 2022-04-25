/*
** Bax, 2021
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
	Log::debug("source = {}", source);

	auto parser = Parser(Lexer(source));
	auto ast = parser.parse();
	if (!ast) {
		Log::error("Parsing failed");
		return Value(false);
	}

	ast->dump();

	Compiler compiler;
	auto ok = compiler.compile(ast);
	if (!ok) {
		Log::error("Compilation failed");
		return Value(false);
	}

	delete ast;

	// auto main = compiler.symbol("main");
	// if (!main) {
	// 	Log::error("No main symbol was defined");
	// 	return false;
	// }

	return Value(true); // run(new Closure(main));
}

}
