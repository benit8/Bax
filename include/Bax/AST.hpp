/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** AST.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Scope.hpp"
#include "Bax/Token.hpp"
#include "fmt/format.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax::AST
{
	/// 0. Basics --------------------------------------------------------------

	struct Node
	{
		virtual ~Node() = default;
		virtual void dump(int i = 0) const;
		virtual const char* class_name() const { return "Node"; }
	protected:
		Node() {}
	};

	struct Expression : public Node
	{
		virtual ~Expression() = default;
		virtual const char* class_name() const { return "Expression"; }
	};

	struct Literal : public Expression
	{
		virtual ~Literal() = default;
		virtual const char* class_name() const { return "Literal"; }
	};

	struct Statement : public Node
	{
		virtual ~Statement() = default;
		virtual const char* class_name() const { return "Statement"; }
	};

	struct Declaration : public Statement
	{
		virtual ~Declaration() = default;
		virtual const char* class_name() const { return "Declaration"; }
	};

	/// 1. Expressions ---------------------------------------------------------

	struct Identifier final : public Expression
	{
		Token name;

		Identifier(Token n)
		: name(std::move(n))
		{}

		~Identifier() {}

		void dump(int i = 0) const override;
		const char* class_name() const { return "Identifier"; }
	};

	struct ArrayExpression final : public Expression
	{
		std::vector<Expression*> elements;

		ArrayExpression(std::vector<Expression*> els)
		: elements(std::move(els))
		{}

		~ArrayExpression() {
			for (auto el : elements)
				delete el;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "ArrayExpression"; }
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

		Expression* lhs;
		Expression* rhs;

		AssignmentExpression(Operators o, Expression* l, Expression* r)
		: op(o)
		, lhs(std::move(l))
		, rhs(std::move(r))
		{}

		~AssignmentExpression() {
			delete lhs;
			delete rhs;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "AssignmentExpression"; }
	};

	struct BinaryExpression final : public Expression
	{
		enum class Operators {
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
			Power,
			Substract,
			Ternary,
		} op;

		Expression* lhs;
		Expression* rhs;

		BinaryExpression(Operators o, Expression* l, Expression* r)
		: op(o)
		, lhs(std::move(l))
		, rhs(std::move(r))
		{}

		~BinaryExpression() {
			delete lhs;
			delete rhs;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "BinaryExpression"; }
	};

	struct CallExpression final : public Expression
	{
		Expression* lhs;
		std::vector<Expression*> arguments;

		CallExpression(Expression* l, std::vector<Expression*> args)
		: lhs(std::move(l))
		, arguments(std::move(args))
		{}

		~CallExpression() {
			delete lhs;
			for (auto arg : arguments)
				delete arg;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "CallExpression"; }
	};

	struct FunctionExpression final : public Expression
	{
		using Parameter = Identifier*;

		std::vector<Parameter> parameters;
		Statement* body;

		FunctionExpression(std::vector<Parameter> params, Statement* bd)
		: parameters(std::move(params))
		, body(std::move(bd))
		{}

		~FunctionExpression() {
			for (auto param : parameters)
				delete param;
			delete body;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "FunctionExpression"; }
	};

	struct MatchExpression final : public Expression
	{
		using CasesType = std::vector<std::pair<std::vector<Expression*>, Expression*>>;

		Expression* subject;
		CasesType cases;

		MatchExpression(Expression* s, CasesType c)
		: subject(std::move(s))
		, cases(std::move(c))
		{}

		~MatchExpression() {
			delete subject;
			for (auto cas : cases) {
				for (auto candidate : cas.first)
					delete candidate;
				delete cas.second;
			}
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "MatchExpression"; }
	};

	struct MemberExpression final : public Expression
	{
		enum class Operators {
			Member,
			Namespace,
			Nullsafe,
			Static,
		} op;

		Expression* lhs;
		Expression* rhs;

		MemberExpression(Operators o, Expression* l, Expression* r)
		: op(o)
		, lhs(std::move(l))
		, rhs(std::move(r))
		{}

		~MemberExpression() {
			delete lhs;
			delete rhs;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "MemberExpression"; }
	};

	struct ObjectExpression final : public Expression
	{
		std::map<Identifier*, Expression*> members;

		ObjectExpression(std::map<Identifier*, Expression*> mems)
		: members(std::move(mems))
		{}

		~ObjectExpression() {
			for (auto [id, expr] : members) {
				delete id;
				delete expr;
			}
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "ObjectExpression"; }
	};

	struct SubscriptExpression final : public Expression
	{
		Expression* lhs;
		Expression* rhs;

		SubscriptExpression(Expression* l, Expression* r = nullptr)
		: lhs(l)
		, rhs(r)
		{}

		~SubscriptExpression() {
			delete lhs;
			delete rhs;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "SubscriptExpression"; }
	};

	struct TernaryExpression final : public Expression
	{
		Expression* condition;
		Expression* consequent;
		Expression* alternate;

		TernaryExpression(Expression* cond, Expression* cons, Expression* alt)
		: condition(std::move(cond))
		, consequent(std::move(cons))
		, alternate(std::move(alt))
		{}

		~TernaryExpression() {
			delete condition;
			delete consequent;
			delete alternate;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "TernaryExpression"; }
	};

	struct UnaryExpression final : public Expression
	{
		enum class Operators {
			BitwiseNot,
			BooleanNot,
			Negative,
			Positive,
		} op;

		Expression* rhs;

		UnaryExpression(Operators o, Expression* r)
		: op(o)
		, rhs(std::move(r))
		{}

		~UnaryExpression() {
			delete rhs;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "UnaryExpression"; }
	};

	struct UpdateExpression final : public Expression
	{
		enum class Operators {
			Increment,
			Decrement,
		} op;

		Expression* expr; // Neither a `lhr` nor `rhs`
		bool is_prefix_update;

		UpdateExpression(Operators o, Expression* r, bool pre)
		: op(o)
		, expr(std::move(r))
		, is_prefix_update(pre)
		{}

		~UpdateExpression() {
			delete expr;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "UpdateExpression"; }
	};

	/// 1. A. Literals ---------------------------------------------------------

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

		void dump(int i = 0) const override;
		const char* class_name() const { return "Boolean"; }
	};

	struct Glyph final : public Literal
	{
		uint64_t value;

		Glyph(uint64_t v)
		: value(v)
		{}

		void dump(int i = 0) const override;
		const char* class_name() const { return "Glyph"; }
	};

	struct Number final : public Literal
	{
		double value;

		Number(double v)
		: value(v)
		{}

		void dump(int i = 0) const override;
		const char* class_name() const { return "Number"; }
	};

	struct String final : public Literal
	{
		std::string value;

		String(std::string v)
		: value(std::move(v))
		{}

		void dump(int i = 0) const override;
		const char* class_name() const { return "String"; }
	};

	/// 2. Statements ----------------------------------------------------------

	struct BlockStatement final : public Statement
	{
		Scope* scope;
		std::vector<Statement*> statements;

		BlockStatement(Scope* sc, std::vector<Statement*> st)
		: scope(sc)
		, statements(std::move(st))
		{}

		~BlockStatement() {
			// delete scope;
			for (auto stmt : statements)
				delete stmt;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "BlockStatement"; }
	};

	struct ExpressionStatement final : public Statement
	{
		Expression* expression;

		ExpressionStatement(Expression* expr)
		: expression(std::move(expr))
		{}

		~ExpressionStatement() {
			delete expression;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "ExpressionStatement"; }
	};

	struct IfStatement final : public Statement
	{
		Expression* condition;
		Statement* consequent;
		Statement* alternate;

		IfStatement(Expression* cond, Statement* cons, Statement* alt = nullptr)
		: condition(std::move(cond))
		, consequent(std::move(cons))
		, alternate(std::move(alt))
		{}

		~IfStatement() {
			delete condition;
			delete consequent;
			delete alternate;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "IfStatement"; }
	};

	struct ReturnStatement final : public Statement
	{
		Expression* value;

		ReturnStatement(Expression* val)
		: value(std::move(val))
		{}

		~ReturnStatement() {
			if (value != nullptr)
				delete value;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "ReturnStatement"; }
	};

	struct WhileStatement final : public Statement
	{
		Expression* condition;
		Statement* body;

		WhileStatement(Expression* cond, Statement* bd)
		: condition(std::move(cond))
		, body(std::move(bd))
		{}

		~WhileStatement() {
			delete condition;
			delete body;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "WhileStatement"; }
	};

	/// 2. A. Declarations -----------------------------------------------------

	struct VariableDeclaration final : public Declaration
	{
		Identifier* name;
		Expression* value;

		VariableDeclaration(Identifier* n, Expression* v)
		: name(std::move(n))
		, value(std::move(v))
		{}

		~VariableDeclaration() {
			delete name;
			delete value;
		}

		void dump(int i = 0) const override;
		const char* class_name() const { return "VariableDeclaration"; }
	};
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
