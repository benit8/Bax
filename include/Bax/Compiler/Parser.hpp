/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/AST.hpp"
#include "Bax/Compiler/Lexer.hpp"
#include <string_view>

// -----------------------------------------------------------------------------

namespace Bax
{

class Parser
{
public:
	enum class Precedence {
		Lowest,
		Assign,        // = += -= *= **= /= |= &= ^= >>= <<= ??=
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
		Ptr<AST::Expression> (Parser::*prefix)(const Token&);
		Ptr<AST::Expression> (Parser::*infix)(const Token&, Ptr<AST::Expression>);
	};

private:
	static const std::unordered_map<Token::Type, GrammarRule> grammar_rules;

	Lexer m_lexer;
	Token m_current_token;

public:
	Parser(const std::string_view& source);
	~Parser();

	Ptr<AST::Node> run();

private:
	const Token& peek() const;
	bool peek(Token::Type) const;
	Token consume();
	bool consume(Token::Type);

	Ptr<AST::Expression> expression(Precedence = Precedence::Lowest);
	Ptr<AST::Expression> literal(const Token&);
	Ptr<AST::Expression> identifier(const Token&);
	Ptr<AST::Expression> unary(const Token&);
	Ptr<AST::Expression> binary(const Token&, Ptr<AST::Expression>);
};

}
