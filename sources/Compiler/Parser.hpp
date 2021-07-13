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
	enum class Precedence {
		Lowest,
		Assign,        // = += -= *= **= /= |= &= ^= >>= <<=
		Ternary,       // ?
		Or,            // ||
		And,           // &&
		Coalesce,      // ?? ?:
		Term,          // * ** / % &
		Factor,        // + - | ^
		Shift,         // << >>
		Unary,         // + - ! ~
		Property,      // . ( [
	};

	struct GrammarRule {
		Precedence precedence;
		AST::Expression* (Parser::*prefix)(const Token&);
		AST::Expression* (Parser::*infix)(const Token&, AST::Expression*);
	};

private:
	static const std::unordered_map<Token::Type, GrammarRule> grammar_rules;

	Lexer m_lexer;
	Token m_current_token;

public:
	Parser(const std::string_view& source);
	~Parser();

	AST::Node* run();

private:
	const Token& peek() const;
	Token consume();

	AST::Expression* expression(Precedence = Precedence::Lowest);
	AST::Expression* literal(const Token&);
	AST::Expression* unary(const Token&);
	AST::Expression* binary(const Token&, AST::Expression*);
};

}
