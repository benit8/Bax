/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/AST.hpp"
#include <istream>
#include <string>
#include <string_view>

// -----------------------------------------------------------------------------

namespace Bax
{

class Compiler
{
	Ptr<AST::Node> m_ast;

public:
	Compiler();
	~Compiler();

	bool do_istream(std::istream& input);
	bool do_file(const std::string& filename);
	bool do_string(std::string_view source);

private:
	bool run(std::string_view source);
};

}
