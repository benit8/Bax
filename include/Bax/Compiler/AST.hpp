/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** AST.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "fmt/format.h"
#include <memory>
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{
	template <typename T>
	using Ptr = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	Ptr<T> makeNode(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
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

		struct Literal : public Expression
		{
			virtual const char* class_name() const { return "Literal"; }
		};

		struct Glyph : public Literal
		{
			uint32_t value;

			Glyph(uint32_t v)
			: value(v)
			{}

			virtual ~Glyph() override {}
			virtual const char* class_name() const { return "Glyph"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({:c}/{:u})\n", class_name(), value, value);
			}
		};

		struct Number : public Literal
		{
			double value;

			Number(double v)
			: value(v)
			{}

			virtual ~Number() override {}
			virtual const char* class_name() const { return "Number"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({:g})\n", class_name(), value);
			}
		};

		struct String : public Literal
		{
			std::string value;

			String(std::string v)
			: value(v)
			{}

			virtual ~String() override {}
			virtual const char* class_name() const { return "String"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), value);
			}
		};

		struct Identifier : public Expression
		{
			std::string name;

			Identifier(std::string n)
			: name(std::move(n))
			{}

			virtual ~Identifier() override {}
			virtual const char* class_name() const { return "Identifier"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), name);
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

			virtual ~UnaryExpression() override {}
			virtual const char* class_name() const { return "UnaryExpression"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				rhs->dump(i + 1);
			}
		};

		struct AssignExpression : public Expression
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

			Ptr<AST::Expression> lhs;
			Ptr<AST::Expression> rhs;

			AssignExpression(Operators o, Ptr<AST::Expression> l, Ptr<AST::Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			virtual ~AssignExpression() override {}
			virtual const char* class_name() const { return "AssignExpression"; }
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

			Ptr<AST::Expression> lhs;
			Ptr<AST::Expression> rhs;

			BinaryExpression(Operators o, Ptr<AST::Expression> l, Ptr<AST::Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			virtual ~BinaryExpression() override {}
			virtual const char* class_name() const { return "BinaryExpression"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				lhs->dump(i + 1);
				rhs->dump(i + 1);
			}
		};

		struct CallExpression : public Expression
		{
			Ptr<AST::Expression> lhs;
			std::vector<Ptr<AST::Expression>> arguments;

			CallExpression(Ptr<AST::Expression> l, std::vector<Ptr<AST::Expression>> args)
			: lhs(std::move(l))
			, arguments(std::move(args))
			{}

			virtual ~CallExpression() override {}
			virtual const char* class_name() const { return "CallExpression"; }
			virtual void dump(int i = 0) const {
				Node::dump(i);
				lhs->dump(i + 1);
				for (auto &arg : arguments)
					arg->dump(i + 1);
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
