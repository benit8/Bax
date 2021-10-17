/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.cpp
*/

#include "Bax/Compiler/Compiler.hpp"
#include "Bax/Compiler/Parser.hpp"
#include "Common/Log.hpp"
#include <fstream>
#include <streambuf>
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{

Compiler::Compiler()
{}

Compiler::~Compiler()
{}

bool Compiler::do_istream(std::istream& input)
{
	std::string source { std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
	return do_string(source);
}

bool Compiler::do_file(const std::string& filename)
{
	Log::debug("do_file(\"{}\")", filename);
	std::ifstream file(filename);
	return do_istream(file);
}

bool Compiler::do_string(std::string_view source)
{
	Log::debug("do_string(\"{}\")", source);
	return run(source);
}

bool Compiler::run(std::string_view source)
{
	auto parser = Parser(Lexer(source));
	m_ast = parser.run();
	if (!m_ast)
		return false;

	m_ast->dump();
	return true;
}

}
