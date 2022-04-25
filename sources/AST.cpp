/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** AST.cpp
*/

#include "Bax/AST.hpp"
#include "fmt/format.h"

// -----------------------------------------------------------------------------

template <typename... Args>
static constexpr void print(int indent, fmt::string_view format, Args&&... args)
{
	for (int i = 0; i < indent; ++i)
		fmt::print("  ");
	fmt::vprint(format, fmt::make_args_checked<Args...>(format, args...));
}

// -----------------------------------------------------------------------------

namespace Bax::AST
{

void Node::dump(int i) const
{
	print(i, "{}\n", class_name());
}

void Identifier::dump(int i) const
{
	print(i, "{}({})\n", class_name(), name);
}

void ArrayExpression::dump(int i) const
{
	Expression::dump(i);
	for (auto &el : elements)
		el->dump(i + 1);
}

void AssignmentExpression::dump(int i) const
{
	print(i, "{}({})\n", class_name(), (int)op);
	lhs->dump(i + 1);
	rhs->dump(i + 1);
}

void BinaryExpression::dump(int i) const
{
	print(i, "{}({})\n", class_name(), (int)op);
	lhs->dump(i + 1);
	rhs->dump(i + 1);
}

void CallExpression::dump(int i) const
{
	Expression::dump(i);
	lhs->dump(i + 1);
	for (auto &arg : arguments)
		arg->dump(i + 1);
}

void FunctionExpression::dump(int i) const
{
	Expression::dump(i);
	for (auto &param : parameters)
		param->dump(i + 1);
	body->dump(i + 1);
}

void MatchExpression::dump(int i) const
{
	Expression::dump(i);
	subject->dump(i + 1);
	for (auto& c : cases) {
		for (auto& ce : c.first) {
			if (ce)
				ce->dump(i + 1);
			else
				print(i + 1, "default\n");
		}
		c.second->dump(i + 1);
	}
}

void MemberExpression::dump(int i) const
{
	print(i, "{}({})\n", class_name(), (int)op);
	lhs->dump(i + 1);
	rhs->dump(i + 1);
}

void ObjectExpression::dump(int i) const
{
	Expression::dump(i);
	for (auto &mem : members) {
		mem.first->dump(i + 1);
		mem.second->dump(i + 2);
	}
}

void SubscriptExpression::dump(int i) const
{
	Expression::dump(i);
	lhs->dump(i + 1);
	if (rhs)
		rhs->dump(i + 1);
	else
		print(i + 1, "(empty subscript)\n");
}

void TernaryExpression::dump(int i) const
{
	Expression::dump(i);
	condition->dump(i + 1);
	consequent->dump(i + 1);
	alternate->dump(i + 1);
}

void UnaryExpression::dump(int i) const
{
	print(i, "{}({})\n", class_name(), (int)op);
	rhs->dump(i + 1);
}

void UpdateExpression::dump(int i) const
{
	print(i, "{}{}({})\n", is_prefix_update ? "Pre" : "Post", class_name(), (int)op);
	expr->dump(i + 1);
}

void Boolean::dump(int i) const
{
	print(i, "{}({})\n", class_name(), value);
}

void Glyph::dump(int i) const
{
	print(i, "{}({:c}|{:d})\n", class_name(), value, value);
}

void Number::dump(int i) const
{
	print(i, "{}({:g})\n", class_name(), value);
}

void String::dump(int i) const
{
	print(i, "{}({:s})\n", class_name(), value);
}

void BlockStatement::dump(int i) const
{
	Statement::dump(i);
	for (auto &stmt : statements)
		stmt->dump(i + 1);
}

void ExpressionStatement::dump(int i) const
{
	Statement::dump(i);
	expression->dump(i + 1);
}

void IfStatement::dump(int i) const
{
	Statement::dump(i);
	condition->dump(i + 1);
	consequent->dump(i + 1);
	if (alternate)
		alternate->dump(i + 1);
}

void ReturnStatement::dump(int i) const
{
	Statement::dump(i);
	if (value)
		value->dump(i + 1);
}

void WhileStatement::dump(int i) const
{
	Statement::dump(i);
	condition->dump(i + 1);
	body->dump(i + 1);
}

void VariableDeclaration::dump(int i) const
{
	Declaration::dump(i);
	name->dump(i + 1);
	value->dump(i + 1);
}

}
