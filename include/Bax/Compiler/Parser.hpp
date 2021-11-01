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
#include <type_traits>
#include <vector>

// -----------------------------------------------------------------------------

#define MUST_CONSUME(TT) if (!must_consume(TT)) return nullptr;
#define CONTAINS(ARR, VAL) (std::find((ARR).begin(), (ARR).end(), (VAL)) != (ARR).end())

// -----------------------------------------------------------------------------

namespace Bax
{

namespace detail {
	template <typename T = void, typename... Ts>
	bool is(const Ptr<AST::Node>& stmt) {
		if (std::is_void<T>::value)
			return false;
		return std::dynamic_pointer_cast<T>(stmt) != nullptr
			? true
			: is<Ts...>(stmt);
	}
}

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
	static const std::array<Token::Type, 6> declaration_tokens;
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
	bool done() const;

	Ptr<AST::Statement> top_level_statement();
	Ptr<AST::Statement> any_statement();
	Ptr<AST::Declaration> declaration();
	Ptr<AST::Statement> statement();
	Ptr<AST::Expression> expression(Precedence = Precedence::Lowest);

	Ptr<AST::Identifier> identifier();
	Ptr<AST::Identifier> namespace_express();

	Ptr<AST::BlockStatement> block_statement();
	Ptr<AST::ExpressionStatement> expression_statement();
	Ptr<AST::IfStatement> if_statement();
	Ptr<AST::ReturnStatement> return_statement();
	Ptr<AST::WhileStatement> while_statement();

	Ptr<AST::ClassDeclaration> class_declaration();
	Ptr<AST::VariableDeclaration> variable_declaration();

	// Grammar rules -----------------------------------------------------------

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

	// Helpers -----------------------------------------------------------------

	uint32_t parse_escape_sequence(std::string_view::const_iterator&);
	bool parse_argument_list(std::vector<Ptr<AST::Expression>>&, Token::Type stop);
	bool parse_parameter_list(std::vector<Ptr<AST::Expression>>&, Token::Type stop);
};

}
