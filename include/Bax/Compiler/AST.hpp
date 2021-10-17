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

		/// 0. Basics ----------------------------------------------------------

		struct Node
		{
			virtual ~Node() {}
			virtual const char* class_name() const { return "Node"; }
			virtual void dump(int i = 0) const {
				priv::print(i, "{}\n", class_name());
			}
		};

		/// 1. Expressions -----------------------------------------------------

		struct Expression : public Node
		{
			virtual const char* class_name() const { return "Expression"; }
		};

		struct Identifier final : public Expression
		{
			std::string name;

			Identifier(std::string n)
			: name(std::move(n))
			{}

			const char* class_name() const { return "Identifier"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), name);
			}
		};

		struct ArrayExpression final : public Expression
		{
			std::vector<Ptr<Expression>> elements;

			ArrayExpression(std::vector<Ptr<Expression>> els)
			: elements(std::move(els))
			{}

			const char* class_name() const { return "ArrayExpression"; }
			void dump(int i = 0) const {
				Node::dump(i);
				for (auto &el : elements)
					el->dump(i + 1);
			}
		};

		struct AssignmentExpression final : public Expression
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

			Ptr<Expression> lhs, rhs;

			AssignmentExpression(Operators o, Ptr<Expression> l, Ptr<Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			const char* class_name() const { return "AssignmentExpression"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				lhs->dump(i + 1);
				rhs->dump(i + 1);
			}
		};

		struct BinaryExpression final : public Expression
		{
			enum class Operators {
				Access,
				Add,
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

			Ptr<Expression> lhs, rhs;

			BinaryExpression(Operators o, Ptr<Expression> l, Ptr<Expression> r)
			: op(o)
			, lhs(std::move(l))
			, rhs(std::move(r))
			{}

			const char* class_name() const { return "BinaryExpression"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				lhs->dump(i + 1);
				rhs->dump(i + 1);
			}
		};

		struct CallExpression final : public Expression
		{
			Ptr<Expression> lhs;
			std::vector<Ptr<Expression>> arguments;

			CallExpression(Ptr<Expression> l, std::vector<Ptr<Expression>> args)
			: lhs(std::move(l))
			, arguments(std::move(args))
			{}

			const char* class_name() const { return "CallExpression"; }
			void dump(int i = 0) const {
				Node::dump(i);
				lhs->dump(i + 1);
				for (auto &arg : arguments)
					arg->dump(i + 1);
			}
		};

		struct ObjectExpression final : public Expression
		{
			std::map<Ptr<Identifier>, Ptr<Expression>> members;

			ObjectExpression(std::map<Ptr<Identifier>, Ptr<Expression>> mems)
			: members(std::move(mems))
			{}

			const char* class_name() const { return "ObjectExpression"; }
			void dump(int i = 0) const {
				Node::dump(i);
				for (auto &mem : members) {
					mem.first->dump(i + 1);
					mem.second->dump(i + 2);
				}
			}
		};

		struct SubscriptExpression final : public Expression
		{
			Ptr<Expression> lhs, rhs;

			SubscriptExpression(Ptr<Expression> l, Ptr<Expression> r = nullptr)
			: lhs(l)
			, rhs(r)
			{}

			const char* class_name() const { return "SubscriptExpression"; }
			void dump(int i = 0) const {
				priv::print(i, "{}\n", class_name());
				lhs->dump(i + 1);
				if (rhs)
					rhs->dump(i + 1);
				else
					priv::print(i + 1, "(empty subscript)\n");
			}
		};

		struct TernaryExpression final : public Expression
		{
			Ptr<Expression> condition, consequent, alternate;

			TernaryExpression(Ptr<Expression> cond, Ptr<Expression> cons, Ptr<Expression> alt)
			: condition(std::move(cond))
			, consequent(std::move(cons))
			, alternate(std::move(alt))
			{}

			const char* class_name() const { return "TernaryExpression"; }
			void dump(int i = 0) const {
				Node::dump(i);
				condition->dump(i + 1);
				consequent->dump(i + 1);
				alternate->dump(i + 1);
			}
		};

		struct UnaryExpression final : public Expression
		{
			enum class Operators {
				BitwiseNot,
				BooleanNot,
				Decrement,
				Increment,
				Negative,
				Positive,
			} op;

			Ptr<Expression> rhs;

			UnaryExpression(Operators o, Ptr<Expression> r)
			: op(o)
			, rhs(std::move(r))
			{}

			const char* class_name() const { return "UnaryExpression"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), (int)op);
				rhs->dump(i + 1);
			}
		};

		/// 1. A. Literals -----------------------------------------------------

		struct Literal : public Expression
		{
			virtual const char* class_name() const { return "Literal"; }
		};

		struct Null final : public Literal
		{
			const char* class_name() const { return "Null"; }
		};

		struct Boolean final : public Literal
		{
			bool value;

			Boolean(bool v)
			: value(v)
			{}

			const char* class_name() const { return "Boolean"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({})\n", class_name(), value);
			}
		};

		struct Glyph final : public Literal
		{
			uint32_t value;

			Glyph(uint32_t v)
			: value(v)
			{}

			const char* class_name() const { return "Glyph"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({:c}|{:d})\n", class_name(), value, value);
			}
		};

		struct Number final : public Literal
		{
			double value;

			Number(double v)
			: value(v)
			{}

			const char* class_name() const { return "Number"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({:g})\n", class_name(), value);
			}
		};

		struct String final : public Literal
		{
			std::string value;

			String(std::string v)
			: value(std::move(v))
			{}

			const char* class_name() const { return "String"; }
			void dump(int i = 0) const {
				priv::print(i, "{}({:s})\n", class_name(), value);
			}
		};

		/// 2. Statements ------------------------------------------------------

		struct Statement : public Node
		{
			virtual const char* class_name() const { return "Statement"; }
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
