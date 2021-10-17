/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/AST.hpp"
#include "Bax/Compiler/Lexer.hpp"
#include <functional>
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
		std::function<Ptr<AST::Expression>(Parser*, const Token&)> prefix;
		std::function<Ptr<AST::Expression>(Parser*, const Token&, Ptr<AST::Expression>)> infix;
	};

private:
	static const std::unordered_map<Token::Type, GrammarRule> grammar_rules;

	Lexer m_lexer;
	Token m_current_token;

public:
	Parser(Lexer lexer);
	~Parser();

	Ptr<AST::Node> run();

private:
	const Token& peek() const;
	bool peek(Token::Type) const;
	Token consume();
	bool consume(Token::Type);

	Ptr<AST::Expression> expression(Precedence = Precedence::Lowest);

	Ptr<AST::Array> array(const Token&);
	Ptr<AST::Glyph> glyph(const Token&);
	Ptr<AST::Expression> group(const Token&);
	Ptr<AST::Identifier> identifier(const Token&);
	Ptr<AST::Literal> literal(const Token&);
	Ptr<AST::Number> number(const Token&);
	Ptr<AST::Object> object(const Token&);
	Ptr<AST::String> string(const Token&);
	Ptr<AST::UnaryExpression> unary(const Token&);
	Ptr<AST::Assignment> assign(const Token&, Ptr<AST::Expression>);
	Ptr<AST::BinaryExpression> binary(const Token&, Ptr<AST::Expression>);
	Ptr<AST::Call> call(const Token&, Ptr<AST::Expression>);
	Ptr<AST::Subscript> subscript(const Token&, Ptr<AST::Expression>);
	Ptr<AST::TernaryExpression> ternary(const Token&, Ptr<AST::Expression>);

	uint32_t parse_escape_sequence(std::string_view::const_iterator&);
};

}
