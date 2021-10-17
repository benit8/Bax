/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** AST.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "fmt/format.h"
#include <map>
#include <memory>
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{
	template <typename T>
	using Own = std::unique_ptr<T>;

	template <typename T>
	using Ptr = std::shared_ptr<T>;

	template <typename T, typename... Args>
	Ptr<T> makeNode(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	namespace AST
	{
		namespace priv
		{
			template <typename... Args>
			constexpr void print(int indent, fmt::string_view format, Args&&... args) {
				for (int i = 0; i < indent; ++i)
					fmt::print("  ");
				fmt::vprint(format, fmt::make_args_checked<Args...>(format, args...));
			}
		}

		struct Node
		{
			virtual ~Node() {}
			virtual const char* class_name() const { return "Node"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}\n", class_name());
			}
		};

		struct Statement : public Node
		{
			virtual const char* class_name() const { return "Statement"; }
		};

		struct Expression : public Node
		{
			virtual const char* class_name() const { return "Expression"; }
		};

		struct Identifier : public Expression
		{
			std::string name;

			Identifier(std::string n)
			: name(std::move(n))
			{}

			virtual const char* class_name() const { return "Identifier"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), name);
			}
		};

		struct Literal : public Expression
		{
			virtual const char* class_name() const { return "Literal"; }
		};

		struct Null : public Literal
		{
			virtual const char* class_name() const { return "Null"; }
		};

		struct Boolean : public Literal
		{
			bool value;

			Boolean(bool v)
			: value(v)
			{}

			virtual const char* class_name() const { return "Boolean"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), value);
			}
		};

		struct Glyph : public Literal
		{
			uint32_t value;

			Glyph(uint32_t v)
			: value(v)
			{}

			virtual const char* class_name() const { return "Glyph"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({:c}|{:d})\n", class_name(), value, value);
			}
		};

		struct Number : public Literal
		{
			double value;

			Number(double v)
			: value(v)
			{}

			virtual const char* class_name() const { return "Number"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({:g})\n", class_name(), value);
			}
		};

		struct String : public Literal
		{
			std::string value;

			String(std::string v)
			: value(std::move(v))
			{}

			virtual const char* class_name() const { return "String"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({:s})\n", class_name(), value);
			}
		};

		struct Array : public Expression
		{
			std::vector<Ptr<AST::Expression>> elements;

			Array(std::vector<Ptr<AST::Expression>> els)
			: elements(std::move(els))
			{}

			virtual const char* class_name() const { return "Array"; }
			virtual void dump(int i = 0) const {
				Node::dump(i);
				for (auto &el : elements)
					el->dump(i + 1);
			}
		};

		struct Object : public Expression
		{
			std::map<Ptr<AST::Identifier>, Ptr<AST::Expression>> members;

			Object(std::map<Ptr<AST::Identifier>, Ptr<AST::Expression>> mems)
			: members(std::move(mems))
			{}

			virtual const char* class_name() const { return "Object"; }
			virtual void dump(int i = 0) const {
				Node::dump(i);
				for (auto &mem : members) {
					mem.first->dump(i + 1);
					mem.second->dump(i + 2);
				}
			}
		};

		struct Call : public Expression
		{
			Ptr<AST::Expression> lhs;
			std::vector<Ptr<AST::Expression>> arguments;

			Call(Ptr<AST::Expression> l, std::vector<Ptr<AST::Expression>> args)
			: lhs(std::move(l))
			, arguments(std::move(args))
			{}

			virtual const char* class_name() const { return "Call"; }
			virtual void dump(int i = 0) const {
				Node::dump(i);
				lhs->dump(i + 1);
				for (auto &arg : arguments)
					arg->dump(i + 1);
			}
		};

		struct UnaryExpression : public Expression
		{
			enum class Operators {
				BitwiseNot,
				BooleanNot,
				Decrement,
				Increment,
				Negative,
				Positive,
			} op;

			Ptr<AST::Expression> rhs;

			UnaryExpression(Operators o, Ptr<AST::Expression> r)
			: op(o)
			, rhs(std::move(r))
			{}

			virtual const char* class_name() const { return "UnaryExpression"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				rhs->dump(i + 1);
			}
		};

		struct Assignment : public Expression
		{
			enum class Operators {
				Add,
				Assign,
				BitwiseAnd,
				BitwiseLeftShift,
				BitwiseOr,
				BitwiseRightShift,
				BitwiseXor,
				BooleanAnd,
				BooleanOr,
				Coalesce,
				Divide,
				Modulo,
				Multiply,
				Power,
				Substract,
			} op;

			Ptr<AST::Expression> lhs, rhs;

			Assignment(Operators o, Ptr<AST::Expression> l, Ptr<AST::Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			virtual const char* class_name() const { return "Assignment"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				lhs->dump(i + 1);
				rhs->dump(i + 1);
			}
		};

		struct BinaryExpression : public Expression
		{
			enum class Operators {
				Access,
				Add,
				Assign,
				BitwiseAnd,
				BitwiseLeftShift,
				BitwiseOr,
				BitwiseRightShift,
				BitwiseXor,
				BooleanAnd,
				BooleanOr,
				Coalesce,
				Divide,
				Equals,
				GreaterThan,
				GreaterThanOrEquals,
				Inequals,
				LessThan,
				LessThanOrEquals,
				Modulo,
				Multiply,
				NullsafeAccess,
				Power,
				Substract,
				Ternary,
			} op;

			Ptr<AST::Expression> lhs, rhs;

			BinaryExpression(Operators o, Ptr<AST::Expression> l, Ptr<AST::Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			virtual const char* class_name() const { return "BinaryExpression"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				lhs->dump(i + 1);
				rhs->dump(i + 1);
			}
		};

		struct Subscript : public Expression
		{
			Ptr<AST::Expression> lhs, rhs;

			Subscript(Ptr<AST::Expression> l, Ptr<AST::Expression> r = nullptr)
			: lhs(l)
			, rhs(r)
			{}

			virtual const char* class_name() const { return "Subscript"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}\n", class_name());
				lhs->dump(i + 1);
				if (rhs)
					rhs->dump(i + 1);
				else
					priv::print(i + 1, "(empty subscript)\n");
			}
		};

		struct TernaryExpression : public Expression
		{
			Ptr<AST::Expression> condition, consequent, alternate;

			TernaryExpression(Ptr<AST::Expression> cond, Ptr<AST::Expression> cons, Ptr<AST::Expression> alt)
			: condition(std::move(cond))
			, consequent(std::move(cons))
			, alternate(std::move(alt))
			{}

			virtual const char* class_name() const { return "TernaryExpression"; }
			virtual void dump(int i = 0) const {
				Node::dump(i);
				condition->dump(i + 1);
				consequent->dump(i + 1);
				alternate->dump(i + 1);
			}
		};
	}
}

template <>
struct fmt::formatter<Bax::AST::Node> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::AST::Node& n, FormatContext& ctx) {
		return format_to(ctx.out(), "{}", n.class_name());
	}
};
