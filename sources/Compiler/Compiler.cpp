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

bool Compiler::do_file(const std::string& filename)
{
	Log::debug("do_file(\"{}\")", filename);
	std::ifstream ifs(filename);
	std::string source { std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
	return do_string(source);
}

bool Compiler::do_string(std::string_view source)
{
	Log::debug("do_string(\"{}\")", source);
	return run(source);
}

bool Compiler::run(std::string_view source)
{
	Parser parser(source);
	m_ast = parser.run();
	if (m_ast == nullptr)
		return false;

	m_ast->dump();
	delete m_ast;
	return true;
}

}
