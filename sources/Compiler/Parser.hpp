/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include <string_view>
#include "AST.hpp"
#include "Lexer.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

class Parser
{
public:
	Parser(const std::string_view& source);
	~Parser();

	AST::Node* run();

private:
	Lexer m_lexer;
};

}
