/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/AST.hpp"
#include "Bax/Compiler/Lexer.hpp"
#include <array>
#include <functional>
#include <string_view>
#include <vector>

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
		Updates,     // ++ --
		Properties,  // . ?. :: \ ( [
	};

	enum class Associativity {
		Left,
		Right,
	};

	struct GrammarRule {
		Precedence precedence;
		Associativity associativity;
		std::function<Ptr<AST::Expression>(Parser*, const Token&)> prefix;
		std::function<Ptr<AST::Expression>(Parser*, const Token&, Ptr<AST::Expression>)> infix;
	};

private:
	static const std::unordered_map<Token::Type, GrammarRule> grammar_rules;
	static const std::array<Token::Type, 4> declaration_tokens;
	static const std::array<Token::Type, 6> statement_tokens;

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
	bool must_consume(Token::Type);

	Ptr<AST::Declaration> declaration();
	Ptr<AST::Statement> statement();
	Ptr<AST::Expression> expression(Precedence = Precedence::Lowest);

	Ptr<AST::Null> null(const Token&);
	Ptr<AST::Boolean> boolean(const Token&);
	Ptr<AST::Glyph> glyph(const Token&);
	Ptr<AST::Number> number(const Token&);
	Ptr<AST::String> string(const Token&);

	Ptr<AST::ArrayExpression> array(const Token&);
	Ptr<AST::AssignmentExpression> assignment(const Token&, Ptr<AST::Expression>);
	Ptr<AST::BinaryExpression> binary(const Token&, Ptr<AST::Expression>);
	Ptr<AST::CallExpression> call(const Token&, Ptr<AST::Expression>);
	Ptr<AST::FunctionExpression> function(const Token&);
	Ptr<AST::Expression> group(const Token&);
	Ptr<AST::Identifier> identifier(const Token&);
	Ptr<AST::MatchExpression> match(const Token&);
	Ptr<AST::MemberExpression> member(const Token&, Ptr<AST::Expression>);
	Ptr<AST::ObjectExpression> object(const Token&);
	Ptr<AST::SubscriptExpression> subscript(const Token&, Ptr<AST::Expression>);
	Ptr<AST::TernaryExpression> ternary(const Token&, Ptr<AST::Expression>);
	Ptr<AST::UnaryExpression> unary(const Token&);
	Ptr<AST::UpdateExpression> update(const Token&, Ptr<AST::Expression> = nullptr);

	Ptr<AST::BlockStatement> block_statement(const Token&);
	Ptr<AST::ExpressionStatement> expression_statement(const Token&);
	Ptr<AST::IfStatement> if_statement(const Token&);
	Ptr<AST::ReturnStatement> return_statement(const Token&);
	Ptr<AST::WhileStatement> while_statement(const Token&);

	Ptr<AST::VariableDeclaration> variable_declaration(const Token&);

	uint32_t parse_escape_sequence(std::string_view::const_iterator&);
	bool parse_argument_list(std::vector<Ptr<AST::Expression>>&, Token::Type stop);
	bool parse_parameter_list(std::vector<Ptr<AST::Expression>>&, Token::Type stop);
};

}
