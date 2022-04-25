/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/AST.hpp"
#include "Bax/Runtime/Function.hpp"
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
public:
	Compiler();
	~Compiler();

	bool compile(AST::Node* ast);

private:
	Function* m_function;

private:
	// Generators dispatcher
	void generate(AST::Node*);

	void generate_identifier(AST::Identifier*);
	void generate_array_expression(AST::ArrayExpression*);
	void generate_assignment_expression(AST::AssignmentExpression*);
	void generate_binary_expression(AST::BinaryExpression*);
	void generate_call_expression(AST::CallExpression*);
	void generate_function_expression(AST::FunctionExpression*);
	void generate_match_expression(AST::MatchExpression*);
	void generate_member_expression(AST::MemberExpression*);
	void generate_object_expression(AST::ObjectExpression*);
	void generate_subscript_expression(AST::SubscriptExpression*);
	void generate_ternary_expression(AST::TernaryExpression*);
	void generate_unary_expression(AST::UnaryExpression*);
	void generate_update_expression(AST::UpdateExpression*);
	void generate_null(AST::Null*);
	void generate_boolean(AST::Boolean*);
	void generate_glyph(AST::Glyph*);
	void generate_number(AST::Number*);
	void generate_string(AST::String*);
	void generate_block_statement(AST::BlockStatement*);
	void generate_expression_statement(AST::ExpressionStatement*);
	void generate_if_statement(AST::IfStatement*);
	void generate_return_statement(AST::ReturnStatement*);
	void generate_while_statement(AST::WhileStatement*);
	void generate_variable_declaration(AST::VariableDeclaration*);
};

}
