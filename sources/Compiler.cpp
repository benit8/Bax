/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Compiler.cpp
*/

#include "Bax/Compiler.hpp"
#include "Bax/Op.hpp"
#include "Bax/Runtime/String.hpp"
#include "Common/Log.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

Compiler::Compiler(Runner& parent)
: m_parent(parent)
{}

Compiler::~Compiler()
{
}

std::optional<Chunk> Compiler::compile(AST::BlockStatement* root)
{
	try {
		Chunk chunk;
		generate(chunk, root->scope, root);
		return chunk;
	} catch (const std::exception &e) {
		Log::error("{}", e.what());
		return {};
	}
}

// -----------------------------------------------------------------------------

void Compiler::generate_identifier(Chunk& chunk, Scope* scope, AST::Identifier* node)
{
	auto res = scope->find_local(node->name);
	if (!res) {
		Value constant(new String(node->name));
		auto idx = chunk.add_constant(std::move(constant));
		chunk.emit(Op::Code::PushConstant, idx);
		chunk.emit(Op::Code::GetGlobal);
	} else {
		auto [ index, is_up_value ] = *res;
		chunk.emit(is_up_value ? Op::Code::LoadUpValue : Op::Code::Load, index);
	}
}

void Compiler::generate_array_expression(Chunk& chunk, Scope* scope, AST::ArrayExpression* node)
{
	for (auto it = node->elements.rbegin(); it != node->elements.rend(); ++it) {
		generate(chunk, scope, *it);
	}

	chunk.emit(Op::Code::MakeArray, node->elements.size());
}

void Compiler::generate_assignment_expression(Chunk&, Scope*, AST::AssignmentExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_binary_expression(Chunk&, Scope*, AST::BinaryExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_call_expression(Chunk& chunk, Scope* scope, AST::CallExpression* node)
{
	for (auto arg : node->arguments)
		generate(chunk, scope, arg);
	generate(chunk, scope, node->lhs);

	chunk.emit(Op::Code::Call, node->arguments.size());
}

void Compiler::generate_function_expression(Chunk& chunk, Scope* scope, AST::FunctionExpression* node)
{
	auto fn = new Closure(&chunk, node->parameters.size());

	generate(*fn, scope, node->body);
	if (!fn->bytecode().empty() && fn->bytecode().rbegin()->code != Op::Code::Return)
		fn->emit(Op::Code::Return, 0);

	Value constant(fn);
	auto index = chunk.add_constant(std::move(constant));
	chunk.emit(Op::Code::PushConstant, index);

	// FIXME: load upvalues then close
}

void Compiler::generate_match_expression(Chunk&, Scope*, AST::MatchExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_member_expression(Chunk& chunk, Scope* scope, AST::MemberExpression* node)
{
	switch (node->op) {
		case AST::MemberExpression::Operators::Member:
			generate(chunk, scope, node->lhs);
			generate(chunk, scope, node->rhs);
			chunk.emit(Op::Code::GetProperty);
			break;
		default: ASSERT_NOT_REACHED();
	}
}

void Compiler::generate_object_expression(Chunk&, Scope*, AST::ObjectExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_subscript_expression(Chunk& chunk, Scope* scope, AST::SubscriptExpression* node)
{
	generate(chunk, scope, node->lhs);
	generate(chunk, scope, node->rhs);
	// chunk.emit(Op::Code::GetTable); or call function __at ?
}

void Compiler::generate_ternary_expression(Chunk&, Scope*, AST::TernaryExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_unary_expression(Chunk&, Scope*, AST::UnaryExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_update_expression(Chunk&, Scope*, AST::UpdateExpression*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_null(Chunk& chunk, Scope*, AST::Null*)
{
	chunk.emit(Op::Code::PushNull);
}

void Compiler::generate_boolean(Chunk& chunk, Scope*, AST::Boolean* node)
{
	chunk.emit(node->value ? Op::Code::PushTrue : Op::Code::PushFalse);
}

void Compiler::generate_glyph(Chunk& chunk, Scope*, AST::Glyph* node)
{
	Value constant(node->value);
	auto index = chunk.add_constant(std::move(constant));
	chunk.emit(Op::Code::PushConstant, index);
}

void Compiler::generate_number(Chunk& chunk, Scope*, AST::Number* node)
{
	Value constant(node->value);
	auto index = chunk.add_constant(std::move(constant));
	chunk.emit(Op::Code::PushConstant, index);
}

void Compiler::generate_string(Chunk& chunk, Scope*, AST::String* node)
{
	Value constant(new String(node->value));
	auto index = chunk.add_constant(std::move(constant));
	chunk.emit(Op::Code::PushConstant, index);
}

void Compiler::generate_block_statement(Chunk& chunk, Scope*, AST::BlockStatement* node)
{
	for (auto stmt : node->statements) {
		generate(chunk, node->scope, stmt);
	}
}

void Compiler::generate_expression_statement(Chunk& chunk, Scope* scope, AST::ExpressionStatement* node)
{
	generate(chunk, scope, node->expression);
}

void Compiler::generate_if_statement(Chunk&, Scope*, AST::IfStatement*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_return_statement(Chunk& chunk, Scope* scope, AST::ReturnStatement* node)
{
	if (node->value) {
		generate(chunk, scope, node->value);
		chunk.emit(Op::Code::Return, 1);
	} else {
		chunk.emit(Op::Code::Return);
	}
}

void Compiler::generate_while_statement(Chunk&, Scope*, AST::WhileStatement*)
{
	ASSERT_NOT_REACHED();
}

void Compiler::generate_variable_declaration(Chunk& chunk, Scope* scope, AST::VariableDeclaration* node)
{
	// Add name to the map of locals of the current scope
	generate(chunk, scope, node->name);
	auto index = scope->add_local(node->name->name);

	// Compile the initializer
	generate(chunk, scope, node->value);

	chunk.emit(Op::Code::Store, index);
}

void Compiler::generate(Chunk& c, Scope* s, AST::Node* node)
{
	if (auto n = dynamic_cast<AST::Identifier*>(node))
		return generate_identifier(c, s, n);
	else if (auto n = dynamic_cast<AST::ArrayExpression*>(node))
		return generate_array_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::AssignmentExpression*>(node))
		return generate_assignment_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::BinaryExpression*>(node))
		return generate_binary_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::CallExpression*>(node))
		return generate_call_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::FunctionExpression*>(node))
		return generate_function_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::MatchExpression*>(node))
		return generate_match_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::MemberExpression*>(node))
		return generate_member_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::ObjectExpression*>(node))
		return generate_object_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::SubscriptExpression*>(node))
		return generate_subscript_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::TernaryExpression*>(node))
		return generate_ternary_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::UnaryExpression*>(node))
		return generate_unary_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::UpdateExpression*>(node))
		return generate_update_expression(c, s, n);
	else if (auto n = dynamic_cast<AST::Null*>(node))
		return generate_null(c, s, n);
	else if (auto n = dynamic_cast<AST::Boolean*>(node))
		return generate_boolean(c, s, n);
	else if (auto n = dynamic_cast<AST::Glyph*>(node))
		return generate_glyph(c, s, n);
	else if (auto n = dynamic_cast<AST::Number*>(node))
		return generate_number(c, s, n);
	else if (auto n = dynamic_cast<AST::String*>(node))
		return generate_string(c, s, n);
	else if (auto n = dynamic_cast<AST::BlockStatement*>(node))
		return generate_block_statement(c, s, n);
	else if (auto n = dynamic_cast<AST::ExpressionStatement*>(node))
		return generate_expression_statement(c, s, n);
	else if (auto n = dynamic_cast<AST::IfStatement*>(node))
		return generate_if_statement(c, s, n);
	else if (auto n = dynamic_cast<AST::ReturnStatement*>(node))
		return generate_return_statement(c, s, n);
	else if (auto n = dynamic_cast<AST::WhileStatement*>(node))
		return generate_while_statement(c, s, n);
	else if (auto n = dynamic_cast<AST::VariableDeclaration*>(node))
		return generate_variable_declaration(c, s, n);
}

}
