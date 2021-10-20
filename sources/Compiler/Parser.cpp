/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.cpp
*/

#include "Bax/Compiler/Parser.hpp"
#include "Common/Assertions.hpp"
#include "Common/Log.hpp"
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------

#define MUST_CONSUME(TT) if (!must_consume(TT)) return nullptr;
#define CONTAINS(ARR, VAL) (std::find((ARR).begin(), (ARR).end(), (VAL)) != (ARR).end())

// -----------------------------------------------------------------------------

namespace Bax
{

#define PREFIX(F) [] (Parser* p, const Token& t) { return p->F(t); }
#define INFIX(F) [] (Parser* p, const Token& t, Ptr<AST::Expression> l) { return p->F(t, l); }

const std::unordered_map<Token::Type, Parser::GrammarRule> Parser::grammar_rules = {
	{ Token::Type::Ampersand,                { Precedence::BitwiseAnd,  Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::AmpersandAmpersand,       { Precedence::BooleanAnd,  Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::AmpersandAmpersandEquals, { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::AmpersandEquals,          { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Asterisk,                 { Precedence::Factors,     Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::AsteriskAsterisk,         { Precedence::Power,       Associativity::Right, nullptr,            INFIX(binary)     } },
	{ Token::Type::AsteriskAsteriskEquals,   { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::AsteriskEquals,           { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Backslash,                { Precedence::Properties,  Associativity::Left,  nullptr,            INFIX(member)     } },
	{ Token::Type::Caret,                    { Precedence::BitwiseXor,  Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::CaretEquals,              { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::ColonColon,               { Precedence::Properties,  Associativity::Left,  nullptr,            INFIX(member)     } },
	{ Token::Type::Dot,                      { Precedence::Properties,  Associativity::Left,  nullptr,            INFIX(member)     } },
	{ Token::Type::Equals,                   { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::EqualsEquals,             { Precedence::Equalities,  Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::Exclamation,              { Precedence::Unaries,     Associativity::Right, PREFIX(unary),      nullptr           } },
	{ Token::Type::ExclamationEquals,        { Precedence::Equalities,  Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::False,                    { Precedence::Lowest,      Associativity::Right, PREFIX(boolean),    nullptr           } },
	{ Token::Type::Glyph,                    { Precedence::Lowest,      Associativity::Right, PREFIX(glyph),      nullptr           } },
	{ Token::Type::Greater,                  { Precedence::Comparisons, Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::GreaterEquals,            { Precedence::Comparisons, Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::GreaterGreater,           { Precedence::Shifts,      Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::GreaterGreaterEquals,     { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Identifier,               { Precedence::Lowest,      Associativity::Right, PREFIX(identifier), nullptr           } },
	{ Token::Type::LeftBrace,                { Precedence::Properties,  Associativity::Right, PREFIX(object),     nullptr           } },
	{ Token::Type::LeftBracket,              { Precedence::Properties,  Associativity::Left,  PREFIX(array),      INFIX(subscript)  } },
	{ Token::Type::LeftParenthesis,          { Precedence::Properties,  Associativity::Left,  PREFIX(group),      INFIX(call)       } },
	{ Token::Type::Less,                     { Precedence::Comparisons, Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::LessEquals,               { Precedence::Comparisons, Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::LessLess,                 { Precedence::Shifts,      Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::LessLessEquals,           { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Minus,                    { Precedence::Terms,       Associativity::Left,  PREFIX(unary),      INFIX(binary)     } },
	{ Token::Type::MinusEquals,              { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::MinusMinus,               { Precedence::Updates,     Associativity::Right, PREFIX(update),     INFIX(update)     } },
	{ Token::Type::Null,                     { Precedence::Lowest,      Associativity::Right, PREFIX(null),       nullptr           } },
	{ Token::Type::Number,                   { Precedence::Lowest,      Associativity::Right, PREFIX(number),     nullptr           } },
	{ Token::Type::Percent,                  { Precedence::Factors,     Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::PercentEquals,            { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Pipe,                     { Precedence::BitwiseOr,   Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::PipeEquals,               { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::PipePipe,                 { Precedence::BooleanOr,   Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::PipePipeEquals,           { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Plus,                     { Precedence::Terms,       Associativity::Left,  PREFIX(unary),      INFIX(binary)     } },
	{ Token::Type::PlusEquals,               { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::PlusPlus,                 { Precedence::Updates,     Associativity::Right, PREFIX(update),     INFIX(update)     } },
	{ Token::Type::Question,                 { Precedence::Ternary,     Associativity::Right, nullptr,            INFIX(ternary)    } },
	{ Token::Type::QuestionColon,            { Precedence::Coalesce,    Associativity::Right, nullptr,            INFIX(binary)     } },
	{ Token::Type::QuestionDot,              { Precedence::Properties,  Associativity::Left,  nullptr,            INFIX(member)     } },
	{ Token::Type::QuestionQuestion,         { Precedence::Coalesce,    Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::QuestionQuestionEquals,   { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::Slash,                    { Precedence::Factors,     Associativity::Left,  nullptr,            INFIX(binary)     } },
	{ Token::Type::SlashEquals,              { Precedence::Assigns,     Associativity::Right, nullptr,            INFIX(assignment) } },
	{ Token::Type::String,                   { Precedence::Lowest,      Associativity::Right, PREFIX(string),     nullptr           } },
	{ Token::Type::Tilde,                    { Precedence::Unaries,     Associativity::Right, PREFIX(unary),      nullptr           } },
	{ Token::Type::True,                     { Precedence::Lowest,      Associativity::Right, PREFIX(boolean),    nullptr           } },
};

#undef PREFIX
#undef INFIX

const std::array<Token::Type, 3> Parser::declaration_tokens = {
	Token::Type::Class,
	Token::Type::Const,
	Token::Type::Let,
};

const std::array<Token::Type, 6> Parser::statement_tokens = {
	Token::Type::For,
	Token::Type::Identifier,
	Token::Type::If,
	Token::Type::LeftBrace,
	Token::Type::Return,
	Token::Type::While,
};

// -----------------------------------------------------------------------------

Parser::Parser(Lexer lexer)
: m_lexer(std::move(lexer))
, m_current_token(m_lexer.next())
{}

Parser::~Parser()
{}

const Token& Parser::peek() const
{
	return m_current_token;
}

bool Parser::peek(Token::Type type) const
{
	return m_current_token.type == type;
}

Token Parser::consume()
{
	Token t = m_current_token;
	m_current_token = m_lexer.next();
	return t;
}

bool Parser::consume(Token::Type type)
{
	if (peek(type)) {
		consume();
		return true;
	}
	return false;
}

bool Parser::must_consume(Token::Type type)
{
	auto token = consume();
	if (token.type != type) {
		Log::error("Unexpected token {}, expected {}", token, Token::type_to_string(type));
		return false;
	}
	return true;
}

Ptr<AST::Node> Parser::run()
{
	// for (Token t = consume(); t.type != Token::Type::Eof; t = consume()) Log::debug("{}", t);
	return statement();
}

// -----------------------------------------------------------------------------

Ptr<AST::Declaration> Parser::declaration()
{
	switch (m_current_token.type) {
		// case Token::Type::Class: return class_declaration();
		default:
			Log::error("Unexpected token {}, expected declaration", m_current_token);
			return nullptr;
	}
}

Ptr<AST::Statement> Parser::statement()
{
	switch (m_current_token.type) {
		case Token::Type::Identifier: return expression_statement();
		case Token::Type::If:         return if_statement();
		case Token::Type::LeftBrace:  return block_statement();
		case Token::Type::While:      return while_statement();
		default:
			Log::error("Unexpected token {}, expected statement", m_current_token);
			break;
	}
	return nullptr;
}

Ptr<AST::Expression> Parser::expression(Parser::Precedence prec)
{
	auto token = consume();
	if (token.type == Token::Type::Eof) {
		Log::error("Unexpected end of file, expected expression");
		return nullptr;
	}

	auto it = grammar_rules.find(token.type);
	if (it == grammar_rules.end()) {
		Log::error("No grammar rule for operator {}", token);
		return nullptr;
	}
	auto rule = it->second;
	if (rule.prefix == nullptr) {
		Log::error("Unexpected token {}, expected prefix", token);
		return nullptr;
	}

	auto node = (rule.prefix)(this, token);

	while (1) {
		auto next = peek();
		if (next.type == Token::Type::Eof)
			break;
		auto it_ = grammar_rules.find(next.type);
		if (it_ == grammar_rules.end()
		 || it_->second.precedence < prec
		 || (it_->second.precedence == prec && it_->second.associativity == Associativity::Left)) {
			break;
		}

		if (it_->second.infix == nullptr) {
			Log::error("Unexpected token {}, expected infix", next);
			return nullptr;
		}

		token = consume();
		node = (it_->second.infix)(this, token, std::move(node));
		if (!node) return nullptr;
	}

	return node;
}

// -----------------------------------------------------------------------------

Ptr<AST::Identifier> Parser::identifier(const Token& token)
{
	return makeNode<AST::Identifier>(std::string(token.trivia.data(), token.trivia.length()));
}

Ptr<AST::Null> Parser::null(const Token&)
{
	return makeNode<AST::Null>();
}

Ptr<AST::Boolean> Parser::boolean(const Token& token)
{
	switch (token.type) {
		case Token::Type::False: return makeNode<AST::Boolean>(false);
		case Token::Type::True:  return makeNode<AST::Boolean>(true);
		default:
			break;
	}
	ASSERT_NOT_REACHED();
}

Ptr<AST::Glyph> Parser::glyph(const Token& token)
{
	auto t = token.trivia.cbegin();
	uint32_t value = (*t == '\\') ? parse_escape_sequence(++t) : *t;

	if (++t != token.trivia.cend()) {
		Log::error("Invalid multi-glyph expression: {}", token);
		return nullptr;
	}

	return makeNode<AST::Glyph>(value);
}

Ptr<AST::Number> Parser::number(const Token& token)
{
	static auto base_digits = "0123456789abcdef";
	static const std::unordered_map<char, int> base_prefixes = {
		{ 'b',  2 },
		{ 'o',  8 },
		{ 'x', 16 },
	};

	auto t = token.trivia.cbegin();
	int base = 10;
	double result = 0;

	if (isdigit(*t)) {
		if (*t == '0' && base_prefixes.contains(*(++t))) {
			base = base_prefixes.at(*t++);
		}
		for (; *t != '\0'; ++t) {
			auto p = strchr(base_digits, tolower(*t));
			if (p == nullptr)
				break;
			int digit = p - base_digits;
			if (digit >= base)
				break;
			result = result * base + digit;
		}
	}

	if (base == 10) {
		if (*t == '.') {
			int fraction = 0;
			unsigned divider = 1;
			for (++t; isdigit(*t); divider *= 10, ++t)
				fraction = fraction * 10 + (*t - '0');
			result += fraction / (double)divider;
		}

		if (tolower(*t) == 'e') {
			++t;
			bool is_exponent_negative = false;
			if (*t == '+') {
				++t;
			} else if (*t == '-') {
				++t;
				is_exponent_negative = true;
			}
			int exponent = 0;
			for (; isdigit(*t); ++t)
				exponent = exponent * 10 + (*t - '0');
			result *= pow(10, is_exponent_negative ? -exponent : exponent);
		}
	}

	return makeNode<AST::Number>(result);
}

Ptr<AST::String> Parser::string(const Token& token)
{
	std::ostringstream oss;

	for (auto it = token.trivia.cbegin(); it != token.trivia.cend(); ++it) {
		oss << (*it == '\\' ? (char)parse_escape_sequence(++it) : *it);
	}

	return makeNode<AST::String>(oss.str());
}

// -----------------------------------------------------------------------------

Ptr<AST::ArrayExpression> Parser::array(const Token&)
{
	std::vector<Ptr<AST::Expression>> elements;

	while (!peek(Token::Type::RightBracket)) {
		auto expr = expression();
		if (!expr) return nullptr;

		elements.push_back(std::move(expr));

		if (!consume(Token::Type::Comma))
			break;
	}
	MUST_CONSUME(Token::Type::RightBracket);

	return makeNode<AST::ArrayExpression>(std::move(elements));
}

Ptr<AST::AssignmentExpression> Parser::assignment(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::AssignmentExpression::Operators> assign_operators = {
		{ Token::Type::AmpersandAmpersandEquals, AST::AssignmentExpression::Operators::BooleanAnd        },
		{ Token::Type::AmpersandEquals,          AST::AssignmentExpression::Operators::BitwiseAnd        },
		{ Token::Type::AsteriskAsteriskEquals,   AST::AssignmentExpression::Operators::Power             },
		{ Token::Type::AsteriskEquals,           AST::AssignmentExpression::Operators::Multiply          },
		{ Token::Type::CaretEquals,              AST::AssignmentExpression::Operators::BitwiseXor        },
		{ Token::Type::Equals,                   AST::AssignmentExpression::Operators::Assign            },
		{ Token::Type::GreaterGreaterEquals,     AST::AssignmentExpression::Operators::BitwiseLeftShift  },
		{ Token::Type::LessLessEquals,           AST::AssignmentExpression::Operators::BitwiseRightShift },
		{ Token::Type::MinusEquals,              AST::AssignmentExpression::Operators::Substract         },
		{ Token::Type::PercentEquals,            AST::AssignmentExpression::Operators::Modulo            },
		{ Token::Type::PipeEquals,               AST::AssignmentExpression::Operators::BitwiseOr         },
		{ Token::Type::PipePipeEquals,           AST::AssignmentExpression::Operators::BooleanOr         },
		{ Token::Type::PlusEquals,               AST::AssignmentExpression::Operators::Add               },
		{ Token::Type::QuestionQuestionEquals,   AST::AssignmentExpression::Operators::Coalesce          },
		{ Token::Type::SlashEquals,              AST::AssignmentExpression::Operators::Divide            },
	};

	auto rhs = expression(Precedence::Assigns);
	if (!rhs) return nullptr;

	return makeNode<AST::AssignmentExpression>(
		assign_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

Ptr<AST::BinaryExpression> Parser::binary(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::BinaryExpression::Operators> binary_operators = {
		{ Token::Type::Ampersand,          AST::BinaryExpression::Operators::BitwiseAnd          },
		{ Token::Type::AmpersandAmpersand, AST::BinaryExpression::Operators::BooleanAnd          },
		{ Token::Type::Asterisk,           AST::BinaryExpression::Operators::Multiply            },
		{ Token::Type::AsteriskAsterisk,   AST::BinaryExpression::Operators::Power               },
		{ Token::Type::Caret,              AST::BinaryExpression::Operators::BitwiseXor          },
		{ Token::Type::EqualsEquals,       AST::BinaryExpression::Operators::Equals              },
		{ Token::Type::ExclamationEquals,  AST::BinaryExpression::Operators::Inequals            },
		{ Token::Type::Greater,            AST::BinaryExpression::Operators::GreaterThan         },
		{ Token::Type::GreaterEquals,      AST::BinaryExpression::Operators::GreaterThanOrEquals },
		{ Token::Type::GreaterGreater,     AST::BinaryExpression::Operators::BitwiseRightShift   },
		{ Token::Type::Less,               AST::BinaryExpression::Operators::LessThan            },
		{ Token::Type::LessEquals,         AST::BinaryExpression::Operators::LessThanOrEquals    },
		{ Token::Type::LessLess,           AST::BinaryExpression::Operators::BitwiseLeftShift    },
		{ Token::Type::Minus,              AST::BinaryExpression::Operators::Substract           },
		{ Token::Type::Percent,            AST::BinaryExpression::Operators::Modulo              },
		{ Token::Type::Pipe,               AST::BinaryExpression::Operators::BitwiseOr           },
		{ Token::Type::PipePipe,           AST::BinaryExpression::Operators::BooleanOr           },
		{ Token::Type::Plus,               AST::BinaryExpression::Operators::Add                 },
		{ Token::Type::QuestionColon,      AST::BinaryExpression::Operators::Ternary             },
		{ Token::Type::QuestionQuestion,   AST::BinaryExpression::Operators::Coalesce            },
		{ Token::Type::Slash,              AST::BinaryExpression::Operators::Divide              },
	};

	auto rhs = expression(grammar_rules.at(token.type).precedence);
	if (!rhs) return nullptr;

	return makeNode<AST::BinaryExpression>(
		binary_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

Ptr<AST::CallExpression> Parser::call(const Token&, Ptr<AST::Expression> lhs)
{
	std::vector<Ptr<AST::Expression>> arguments;

	while (!peek(Token::Type::RightParenthesis)) {
		if (!arguments.empty() && !must_consume(Token::Type::Comma))
			return nullptr;

		auto arg = expression();
		if (!arg) return nullptr;

		arguments.push_back(std::move(arg));
	}
	MUST_CONSUME(Token::Type::RightParenthesis);

	return makeNode<AST::CallExpression>(std::move(lhs), std::move(arguments));
}

Ptr<AST::Expression> Parser::group(const Token&)
{
	auto expr = expression();
	MUST_CONSUME(Token::Type::RightParenthesis);
	return expr;
}

Ptr<AST::MemberExpression> Parser::member(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::MemberExpression::Operators> member_operators = {
		{ Token::Type::Backslash,   AST::MemberExpression::Operators::Namespace },
		{ Token::Type::ColonColon,  AST::MemberExpression::Operators::Static    },
		{ Token::Type::Dot,         AST::MemberExpression::Operators::Member    },
		{ Token::Type::QuestionDot, AST::MemberExpression::Operators::Nullsafe  },
	};

	static const std::array<std::string, 2> allowed_lhs = {
		"Identifier",
		"MemberExpression",
	};

	if (!CONTAINS(allowed_lhs, lhs->class_name())) {
		Log::error("Left-hand side of member expressions must be an identifier or another member expression, found {} instead.", lhs->class_name());
		return nullptr;
	}

	auto rhs = expression(Precedence::Properties);
	if (!rhs) return nullptr;

	if (strcmp(rhs->class_name(), "Identifier") != 0) {
		Log::error("Right-hand side of member expressions must be an identifier, found {} instead.", rhs->class_name());
		return nullptr;
	}

	return makeNode<AST::MemberExpression>(
		member_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

Ptr<AST::ObjectExpression> Parser::object(const Token&)
{
	std::map<Ptr<AST::Identifier>, Ptr<AST::Expression>> members;

	while (!peek(Token::Type::RightBrace)) {
		auto id_token = consume();
		if (id_token.type != Token::Type::Identifier) {
			Log::error("Unexpected token {}, expected indentifier", id_token);
			return nullptr;
		}

		auto id = identifier(id_token);
		if (!id) return nullptr;

		MUST_CONSUME(Token::Type::Colon);

		auto expr = expression();
		if (!expr) return nullptr;

		members.emplace(std::move(id), std::move(expr));

		if (!consume(Token::Type::Comma))
			break;
	}
	MUST_CONSUME(Token::Type::RightBrace);

	return makeNode<AST::ObjectExpression>(std::move(members));
}

Ptr<AST::SubscriptExpression> Parser::subscript(const Token&, Ptr<AST::Expression> lhs)
{
	// Allow empty subscript expressions (eg. `expr[]`)
	if (consume(Token::Type::RightBracket)) {
		return makeNode<AST::SubscriptExpression>(lhs);
	}

	auto expr = expression();
	if (!expr) return nullptr;

	MUST_CONSUME(Token::Type::RightBracket);

	return makeNode<AST::SubscriptExpression>(lhs, std::move(expr));
}

Ptr<AST::TernaryExpression> Parser::ternary(const Token&, Ptr<AST::Expression> lhs)
{
	auto consequent = expression(Precedence::Ternary);
	if (!consequent) return nullptr;

	MUST_CONSUME(Token::Type::Colon);

	auto alternate = expression(Precedence::Ternary);
	if (!alternate) return nullptr;

	return makeNode<AST::TernaryExpression>(
		std::move(lhs),
		std::move(consequent),
		std::move(alternate)
	);
}

Ptr<AST::UnaryExpression> Parser::unary(const Token& token)
{
	static const std::unordered_map<Token::Type, AST::UnaryExpression::Operators> unary_operators = {
		{ Token::Type::Exclamation, AST::UnaryExpression::Operators::BooleanNot },
		{ Token::Type::Plus,        AST::UnaryExpression::Operators::Positive   },
		{ Token::Type::Minus,       AST::UnaryExpression::Operators::Negative   },
		{ Token::Type::Tilde,       AST::UnaryExpression::Operators::BitwiseNot },
	};

	auto rhs = expression(Precedence::Unaries);
	if (!rhs) return nullptr;

	return makeNode<AST::UnaryExpression>(
		unary_operators.at(token.type),
		std::move(rhs)
	);
}

Ptr<AST::UpdateExpression> Parser::update(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::UpdateExpression::Operators> update_operators = {
		{ Token::Type::PlusPlus,   AST::UpdateExpression::Operators::Increment },
		{ Token::Type::MinusMinus, AST::UpdateExpression::Operators::Decrement },
	};

	static const std::array<std::string, 2> allowed_expressions = {
		"Identifier",
		"MemberExpression",
	};

	bool is_prefix_update = false;

	// `lhs` will be a `nullptr` in the case of an infix {in,de}crement.
	if (!lhs) {
		is_prefix_update = true;
		lhs = expression(Precedence::Updates);
		if (!lhs) return nullptr;
	}
	if (!CONTAINS(allowed_expressions, lhs->class_name())) {
		Log::error(
			"{}-hand side of update operator must be an identifier or a member expression, found {} instead.",
			is_prefix_update ? "Right" : "Left",
			lhs->class_name()
		);
		return nullptr;
	}

	return makeNode<AST::UpdateExpression>(
		update_operators.at(token.type),
		std::move(lhs),
		is_prefix_update
	);
}

// -----------------------------------------------------------------------------

Ptr<AST::BlockStatement> Parser::block_statement()
{
	std::vector<Ptr<AST::Statement>> statements;

	MUST_CONSUME(Token::Type::LeftBrace);

	while (!peek(Token::Type::Eof) && !peek(Token::Type::RightBrace)) {
		if (CONTAINS(declaration_tokens, m_current_token.type)) {
			auto decl = declaration();
			if (!decl) return nullptr;
			statements.push_back(std::move(decl));
		}
		else if (CONTAINS(statement_tokens, m_current_token.type)) {
			auto stmt = statement();
			if (!stmt) return nullptr;
			statements.push_back(std::move(stmt));
		}
		else {
			Log::error("Unexpected token {}, expected statement or declaration", m_current_token);
			return nullptr;
		}
	}

	MUST_CONSUME(Token::Type::RightBrace);

	return makeNode<AST::BlockStatement>(std::move(statements));
}

Ptr<AST::ExpressionStatement> Parser::expression_statement()
{
	auto expr = expression();
	if (!expr) return nullptr;

	MUST_CONSUME(Token::Type::Semicolon);

	/// FIXME: Check against actual types, not strings
	static const std::array<std::string, 3> allowed_expressions = {
		"AssignmentExpression",
		"CallExpression",
		"UpdateExpression",
	};
	if (!CONTAINS(allowed_expressions, expr->class_name())) {
		Log::error("Expression of type {} is not allowed as a statement", expr->class_name());
		return nullptr;
	}

	return makeNode<AST::ExpressionStatement>(std::move(expr));
}

Ptr<AST::IfStatement> Parser::if_statement()
{
	MUST_CONSUME(Token::Type::If);
	MUST_CONSUME(Token::Type::LeftParenthesis);

	auto condition = expression();
	if (!condition) return nullptr;

	MUST_CONSUME(Token::Type::RightParenthesis);

	auto consequent = statement();
	if (!consequent) return nullptr;

	Ptr<AST::Statement> alternate = nullptr;
	if (consume(Token::Type::Else)) {
		alternate = statement();
		if (!alternate) return nullptr;
	}

	return makeNode<AST::IfStatement>(
		std::move(condition),
		std::move(consequent),
		std::move(alternate)
	);
}

Ptr<AST::WhileStatement> Parser::while_statement()
{
	MUST_CONSUME(Token::Type::While);
	MUST_CONSUME(Token::Type::LeftParenthesis);

	auto condition = expression();
	if (!condition) return nullptr;

	MUST_CONSUME(Token::Type::RightParenthesis);

	auto body = statement();
	if (!body) return nullptr;

	return makeNode<AST::WhileStatement>(
		std::move(condition),
		std::move(body)
	);
}

// -----------------------------------------------------------------------------

uint32_t Parser::parse_escape_sequence(std::string_view::const_iterator& it)
{
	// Assume the '\' is already skipped

	switch (*it) {
		case 'a': return '\a';
		case 'b': return '\b';
		case 'e': return '\e';
		case 'f': return '\f';
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case 'v': return '\v';
		case '\\': return '\\';
		case '\'': return '\'';
		case '"': return '"';
		case '?': return '?';

		case 'u': {
			// Must be composed of exactly 4 hex characters
			bool valid = true;
			for (auto i = 1; i <= 4; ++i) {
				if (!isxdigit(*(it + i))) {
					valid = false;
					break;
				}
			}
			if (!valid)
				break;

			uint32_t value = 0;
			for (++it; isxdigit(*it); ++it)
				value = value * 16 + (tolower(*it) - (isdigit(*it) ? '0' : 'a' - 10));
			--it;
			return value;
		}

		default: break;
	}
	return *--it;
}

}
