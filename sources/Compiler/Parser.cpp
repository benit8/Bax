/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.cpp
*/

#include "Parser.hpp"
#include <iostream>

// -----------------------------------------------------------------------------

namespace Bax
{

Parser::Parser(const std::string_view& source)
: m_lexer(source)
{}

Parser::~Parser()
{}

AST::Node* Parser::run()
{
	for (Token t = m_lexer.next(); t.type != Token::Type::Eof; t = m_lexer.next())
		std::cout << t << std::endl;

	return nullptr;
}

}
