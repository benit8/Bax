/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Parser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/AST.hpp"
#include "Bax/Lexer.hpp"
#include "Bax/Scope.hpp"
#include <array>
#include <functional>
#include <stack>
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
	bool is(AST::Node* stmt) {
		if (std::is_void<T>::value)
			return false;
		return dynamic_cast<T*>(stmt) != nullptr
			? true
			: is<Ts...>(stmt);
	}
}

/**
 * Takes a stream of tokens and transform it into an AST.
 */
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
		std::function<AST::Expression*(Parser*, const Token&)> prefix;
		std::function<AST::Expression*(Parser*, const Token&, AST::Expression*)> infix;
	};

private:
	static const std::unordered_map<Token::Type, GrammarRule> grammar_rules;
	static const std::array<Token::Type, 1> declaration_tokens;
	static const std::array<Token::Type, 6> statement_tokens;

	Lexer m_lexer;
	Token m_current_token;
	std::stack<Scope*> m_scope_stack;

public:
	Parser(Lexer lexer);
	~Parser();

	AST::BlockStatement* parse();

private:
	const Token& peek() const;
	bool peek(Token::Type) const;
	Token consume();
	bool consume(Token::Type);
	bool must_consume(Token::Type);
	bool done() const;

	AST::Statement* top_level_statement();
	AST::Statement* any_statement();
	AST::Declaration* declaration();
	AST::Statement* statement();
	AST::Expression* expression(Precedence = Precedence::Lowest);

	AST::Identifier* identifier();
	AST::Identifier* namespace_express();

	AST::BlockStatement* block_statement(std::vector<AST::Identifier*> parameters = {});
	AST::ExpressionStatement* expression_statement();
	AST::IfStatement* if_statement();
	AST::ReturnStatement* return_statement();
	AST::WhileStatement* while_statement();

	AST::VariableDeclaration* variable_declaration();

	// Grammar rules -----------------------------------------------------------

	AST::Null* null(const Token&);
	AST::Boolean* boolean(const Token&);
	AST::Glyph* glyph(const Token&);
	AST::Number* number(const Token&);
	AST::String* string(const Token&);

	AST::ArrayExpression* array(const Token&);
	AST::AssignmentExpression* assignment(const Token&, AST::Expression*);
	AST::BinaryExpression* binary(const Token&, AST::Expression*);
	AST::CallExpression* call(const Token&, AST::Expression*);
	AST::FunctionExpression* function(const Token&);
	AST::Expression* group(const Token&);
	AST::Identifier* identifier(const Token&);
	AST::MatchExpression* match(const Token&);
	AST::MemberExpression* member(const Token&, AST::Expression*);
	AST::ObjectExpression* object(const Token&);
	AST::SubscriptExpression* subscript(const Token&, AST::Expression*);
	AST::TernaryExpression* ternary(const Token&, AST::Expression*);
	AST::UnaryExpression* unary(const Token&);
	AST::UpdateExpression* update(const Token&, AST::Expression* = nullptr);

	// Helpers -----------------------------------------------------------------

	Scope* current_scope() const;
	void push_scope(std::vector<AST::Identifier*> parameters = {});
	Scope* pop_scope();

	uint64_t parse_escape_sequence(std::string_view::const_iterator&);
	bool parse_argument_list(std::vector<AST::Expression*>&, Token::Type stop);
	bool parse_parameter_list(std::vector<AST::Identifier*>&, Token::Type stop);
};

}
