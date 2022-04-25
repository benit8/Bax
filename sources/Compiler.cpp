/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.cpp
*/

#include "Bax/Compiler.hpp"
#include "Common/Log.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

Compiler::Compiler()
{}

Compiler::~Compiler()
{}

bool Compiler::compile(AST::Node* ast)
{
	m_function = new Function();

	generate(ast);
	return true;
}

// -----------------------------------------------------------------------------

void Compiler::generate_identifier(AST::Identifier*)
{}

void Compiler::generate_array_expression(AST::ArrayExpression*)
{}

void Compiler::generate_assignment_expression(AST::AssignmentExpression*)
{}

void Compiler::generate_binary_expression(AST::BinaryExpression*)
{}

void Compiler::generate_call_expression(AST::CallExpression*)
{}

void Compiler::generate_function_expression(AST::FunctionExpression*)
{}

void Compiler::generate_match_expression(AST::MatchExpression*)
{}

void Compiler::generate_member_expression(AST::MemberExpression*)
{}

void Compiler::generate_object_expression(AST::ObjectExpression*)
{}

void Compiler::generate_subscript_expression(AST::SubscriptExpression*)
{}

void Compiler::generate_ternary_expression(AST::TernaryExpression*)
{}

void Compiler::generate_unary_expression(AST::UnaryExpression*)
{}

void Compiler::generate_update_expression(AST::UpdateExpression*)
{}

void Compiler::generate_null(AST::Null*)
{
	Log::info("PUSH NULL");
}

void Compiler::generate_boolean(AST::Boolean* b)
{
	Log::info("PUSH {}", b->value ? "TRUE" : "FALSE");
}

void Compiler::generate_glyph(AST::Glyph*)
{
	// Add to the constant set, get index
	Log::info("PUSH CONSTANT G");
}

void Compiler::generate_number(AST::Number*)
{
	// Add to the constant set, get index
	Log::info("PUSH CONSTANT N");
}

void Compiler::generate_string(AST::String*)
{
	// Add to the constant set, get index
	Log::info("PUSH CONSTANT S");
}

void Compiler::generate_block_statement(AST::BlockStatement* bs)
{
	for (auto stmt : bs->statements)
		generate(stmt);
}

void Compiler::generate_expression_statement(AST::ExpressionStatement* es)
{
	generate(es->expression);
}

void Compiler::generate_if_statement(AST::IfStatement*)
{}

void Compiler::generate_return_statement(AST::ReturnStatement*)
{}

void Compiler::generate_while_statement(AST::WhileStatement*)
{}

void Compiler::generate_variable_declaration(AST::VariableDeclaration* vd)
{
	// Compile the initializer
	generate(vd->value);

	// Add name to the map of locals of the current scope
}

void Compiler::generate(AST::Node* node)
{
	if (auto n = dynamic_cast<AST::Identifier*>(node))
		return generate_identifier(n);
	else if (auto n = dynamic_cast<AST::ArrayExpression*>(node))
		return generate_array_expression(n);
	else if (auto n = dynamic_cast<AST::AssignmentExpression*>(node))
		return generate_assignment_expression(n);
	else if (auto n = dynamic_cast<AST::BinaryExpression*>(node))
		return generate_binary_expression(n);
	else if (auto n = dynamic_cast<AST::CallExpression*>(node))
		return generate_call_expression(n);
	else if (auto n = dynamic_cast<AST::FunctionExpression*>(node))
		return generate_function_expression(n);
	else if (auto n = dynamic_cast<AST::MatchExpression*>(node))
		return generate_match_expression(n);
	else if (auto n = dynamic_cast<AST::MemberExpression*>(node))
		return generate_member_expression(n);
	else if (auto n = dynamic_cast<AST::ObjectExpression*>(node))
		return generate_object_expression(n);
	else if (auto n = dynamic_cast<AST::SubscriptExpression*>(node))
		return generate_subscript_expression(n);
	else if (auto n = dynamic_cast<AST::TernaryExpression*>(node))
		return generate_ternary_expression(n);
	else if (auto n = dynamic_cast<AST::UnaryExpression*>(node))
		return generate_unary_expression(n);
	else if (auto n = dynamic_cast<AST::UpdateExpression*>(node))
		return generate_update_expression(n);
	else if (auto n = dynamic_cast<AST::Null*>(node))
		return generate_null(n);
	else if (auto n = dynamic_cast<AST::Boolean*>(node))
		return generate_boolean(n);
	else if (auto n = dynamic_cast<AST::Glyph*>(node))
		return generate_glyph(n);
	else if (auto n = dynamic_cast<AST::Number*>(node))
		return generate_number(n);
	else if (auto n = dynamic_cast<AST::String*>(node))
		return generate_string(n);
	else if (auto n = dynamic_cast<AST::BlockStatement*>(node))
		return generate_block_statement(n);
	else if (auto n = dynamic_cast<AST::ExpressionStatement*>(node))
		return generate_expression_statement(n);
	else if (auto n = dynamic_cast<AST::IfStatement*>(node))
		return generate_if_statement(n);
	else if (auto n = dynamic_cast<AST::ReturnStatement*>(node))
		return generate_return_statement(n);
	else if (auto n = dynamic_cast<AST::WhileStatement*>(node))
		return generate_while_statement(n);
	else if (auto n = dynamic_cast<AST::VariableDeclaration*>(node))
		return generate_variable_declaration(n);
}

}
