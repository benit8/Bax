/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** AST.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

namespace Bax::AST
{

namespace priv {
	template <typename... Args>
	constexpr void print(int indent, fmt::string_view format, Args&&... args) {
		for (int i = 0; i < indent; ++i)
			fmt::print("  ");
		fmt::vprint(format, fmt::make_args_checked<Args...>(format, args...));
	}
}

struct Node
{
	virtual const char* name() const { return "Node"; }
	virtual void dump(int i = 0) const {
		priv::print(i, "{}\n", name());
	}
};

struct Expression : public Node
{
	virtual const char* name() const { return "Expression"; }
	virtual void dump(int i = 0) const { Node::dump(i); }
};

struct Number : public Expression
{
	double value;

	Number(double v)
	: value(v)
	{}

	virtual const char* name() const { return "Number"; }
	virtual void dump(int i = 0) const {
		priv::print(i, "{}({:g})\n", name(), value);
	}
};

struct UnaryExpression : public Expression
{
	enum class Operators {
		Negative,
		Not,
		Positive,
	} op;
	AST::Expression* rhs;

	UnaryExpression(Operators o, AST::Expression* r)
	: op(o)
	, rhs(r)
	{}

	virtual const char* name() const { return "UnaryExpression"; }
	virtual void dump(int i = 0) const {
		Node::dump(i);
		priv::print(i + 1, "{}\n", (int)op);
		rhs->dump(i + 1);
	}
};

struct BinaryExpression : public Expression
{
	enum class Operators {
		Add,
		Divide,
		Multiply,
		Substract,
	} op;
	AST::Expression* lhs;
	AST::Expression* rhs;

	BinaryExpression(Operators o, AST::Expression* l, AST::Expression* r)
	: op(o)
	, lhs(l)
	, rhs(r)
	{}

	virtual const char* name() const { return "BinaryExpression"; }
	virtual void dump(int i = 0) const {
		Node::dump(i);
		priv::print(i + 1, "{}\n", (int)op);
		lhs->dump(i + 1);
		rhs->dump(i + 1);
	}
};

}

template <>
struct fmt::formatter<Bax::AST::Node> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::AST::Node& n, FormatContext& ctx) {
		return format_to(ctx.out(), "{}", n.name());
	}
};
