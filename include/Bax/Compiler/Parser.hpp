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
		Assigns,     // = += -= *= **= /= |= &= ^= <<= >>= ??=
		Ternary,     // ?
		Coalesce,    // ?? ?:
		BooleanOr,   // ||
		BooleanAnd,  // &&
		BitwiseOr,   // |
		BitwiseXor,  // ^
		BitwiseAnd,  // &
		Equalities,  // == !=
		Comparisons, // < <= > >=
		Shifts,      // << >>
		Terms,       // + -
		Factors,     // * / %
		Power,       // **
		Unaries,     // ! + - ~
		Increments,  // ++ --
		Properties,  // . ?. ( [
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

	Ptr<AST::Expression> array(const Token&);
	Ptr<AST::Expression> glyph(const Token&);
	Ptr<AST::Expression> group(const Token&);
	Ptr<AST::Expression> identifier(const Token&);
	Ptr<AST::Expression> literal(const Token&);
	Ptr<AST::Expression> number(const Token&);
	Ptr<AST::Expression> object(const Token&);
	Ptr<AST::Expression> string(const Token&);
	Ptr<AST::Expression> unary(const Token&);
	Ptr<AST::Expression> assign(const Token&, Ptr<AST::Expression>);
	Ptr<AST::Expression> binary(const Token&, Ptr<AST::Expression>);
	Ptr<AST::Expression> call(const Token&, Ptr<AST::Expression>);
	Ptr<AST::Expression> index(const Token&, Ptr<AST::Expression>);

	uint32_t parse_escape_sequence(std::string_view::const_iterator&);
};

}
