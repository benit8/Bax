/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/AST.hpp"
#include "Bax/Scope.hpp"
#include "Bax/Runtime/Function.hpp"
#include "Bax/Runtime/Runner.hpp"
#include <optional>
#include <string>
#include <string_view>

// -----------------------------------------------------------------------------

namespace Bax
{

/**
 * Takes an AST and transforms it into bytecode.
 */
class Compiler
{
private:
	Runner& m_parent;

public:
	explicit Compiler(Runner&);
	~Compiler();

	std::optional<Chunk> compile(AST::BlockStatement* ast);

private:
	// Generators dispatcher
	void generate(Chunk&, Scope*, AST::Node*);

	void generate_identifier(Chunk&, Scope*, AST::Identifier*);
	void generate_array_expression(Chunk&, Scope*, AST::ArrayExpression*);
	void generate_assignment_expression(Chunk&, Scope*, AST::AssignmentExpression*);
	void generate_binary_expression(Chunk&, Scope*, AST::BinaryExpression*);
	void generate_call_expression(Chunk&, Scope*, AST::CallExpression*);
	void generate_function_expression(Chunk&, Scope*, AST::FunctionExpression*);
	void generate_match_expression(Chunk&, Scope*, AST::MatchExpression*);
	void generate_member_expression(Chunk&, Scope*, AST::MemberExpression*);
	void generate_object_expression(Chunk&, Scope*, AST::ObjectExpression*);
	void generate_subscript_expression(Chunk&, Scope*, AST::SubscriptExpression*);
	void generate_ternary_expression(Chunk&, Scope*, AST::TernaryExpression*);
	void generate_unary_expression(Chunk&, Scope*, AST::UnaryExpression*);
	void generate_update_expression(Chunk&, Scope*, AST::UpdateExpression*);
	void generate_null(Chunk&, Scope*, AST::Null*);
	void generate_boolean(Chunk&, Scope*, AST::Boolean*);
	void generate_glyph(Chunk&, Scope*, AST::Glyph*);
	void generate_number(Chunk&, Scope*, AST::Number*);
	void generate_string(Chunk&, Scope*, AST::String*);
	void generate_block_statement(Chunk&, Scope*, AST::BlockStatement*);
	void generate_expression_statement(Chunk&, Scope*, AST::ExpressionStatement*);
	void generate_if_statement(Chunk&, Scope*, AST::IfStatement*);
	void generate_return_statement(Chunk&, Scope*, AST::ReturnStatement*);
	void generate_while_statement(Chunk&, Scope*, AST::WhileStatement*);
	void generate_variable_declaration(Chunk&, Scope*, AST::VariableDeclaration*);
};

}
