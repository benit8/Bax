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

namespace Bax
{

#define PREFIX(F) [] (Parser* p, const Token& t) { return p->F(t); }
#define INFIX(F) [] (Parser* p, const Token& t, Ptr<AST::Expression> l) { return p->F(t, l); }

const std::unordered_map<Token::Type, Parser::GrammarRule> Parser::grammar_rules = {
	{ Token::Type::Ampersand,                { Precedence::BitwiseAnd,  nullptr,            INFIX(binary)    } },
	{ Token::Type::AmpersandAmpersand,       { Precedence::BooleanAnd,  nullptr,            INFIX(binary)    } },
	{ Token::Type::AmpersandAmpersandEquals, { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::AmpersandEquals,          { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Asterisk,                 { Precedence::Factors,     nullptr,            INFIX(binary)    } },
	{ Token::Type::AsteriskAsterisk,         { Precedence::Power,       nullptr,            INFIX(binary)    } },
	{ Token::Type::AsteriskAsteriskEquals,   { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::AsteriskEquals,           { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Caret,                    { Precedence::BitwiseXor,  nullptr,            INFIX(binary)    } },
	{ Token::Type::CaretEquals,              { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Dot,                      { Precedence::Properties,  nullptr,            INFIX(binary)    } },
	{ Token::Type::Equals,                   { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::EqualsEquals,             { Precedence::Equalities,  nullptr,            INFIX(binary)    } },
	{ Token::Type::Exclamation,              { Precedence::Unaries,     PREFIX(unary),      nullptr          } },
	{ Token::Type::ExclamationEquals,        { Precedence::Equalities,  nullptr,            INFIX(binary)    } },
	{ Token::Type::False,                    { Precedence::Lowest,      PREFIX(literal),    nullptr          } },
	{ Token::Type::Glyph,                    { Precedence::Lowest,      PREFIX(glyph),      nullptr          } },
	{ Token::Type::Greater,                  { Precedence::Comparisons, nullptr,            INFIX(binary)    } },
	{ Token::Type::GreaterEquals,            { Precedence::Comparisons, nullptr,            INFIX(binary)    } },
	{ Token::Type::GreaterGreater,           { Precedence::Shifts,      nullptr,            INFIX(binary)    } },
	{ Token::Type::GreaterGreaterEquals,     { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Identifier,               { Precedence::Lowest,      PREFIX(identifier), nullptr          } },
	{ Token::Type::LeftBrace,                { Precedence::Properties,  PREFIX(object),     nullptr          } },
	{ Token::Type::LeftBracket,              { Precedence::Properties,  PREFIX(array),      INFIX(subscript) } },
	{ Token::Type::LeftParenthesis,          { Precedence::Properties,  PREFIX(group),      INFIX(call)      } },
	{ Token::Type::Less,                     { Precedence::Comparisons, nullptr,            INFIX(binary)    } },
	{ Token::Type::LessEquals,               { Precedence::Comparisons, nullptr,            INFIX(binary)    } },
	{ Token::Type::LessLess,                 { Precedence::Shifts,      nullptr,            INFIX(binary)    } },
	{ Token::Type::LessLessEquals,           { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Minus,                    { Precedence::Terms,       PREFIX(unary),      INFIX(binary)    } },
	{ Token::Type::MinusEquals,              { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::MinusMinus,               { Precedence::Increments,  PREFIX(unary),      nullptr          } },
	{ Token::Type::Null,                     { Precedence::Lowest,      PREFIX(literal),    nullptr          } },
	{ Token::Type::Number,                   { Precedence::Lowest,      PREFIX(number),     nullptr          } },
	{ Token::Type::Percent,                  { Precedence::Factors,     nullptr,            INFIX(binary)    } },
	{ Token::Type::PercentEquals,            { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Pipe,                     { Precedence::BitwiseOr,   nullptr,            INFIX(binary)    } },
	{ Token::Type::PipeEquals,               { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::PipePipe,                 { Precedence::BooleanOr,   nullptr,            INFIX(binary)    } },
	{ Token::Type::PipePipeEquals,           { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Plus,                     { Precedence::Terms,       PREFIX(unary),      INFIX(binary)    } },
	{ Token::Type::PlusEquals,               { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::PlusPlus,                 { Precedence::Increments,  PREFIX(unary),      nullptr          } },
	{ Token::Type::Question,                 { Precedence::Ternary,     nullptr,            INFIX(ternary)   } },
	{ Token::Type::QuestionColon,            { Precedence::Coalesce,    nullptr,            INFIX(binary)    } },
	{ Token::Type::QuestionDot,              { Precedence::Properties,  nullptr,            INFIX(binary)    } },
	{ Token::Type::QuestionQuestion,         { Precedence::Coalesce,    nullptr,            INFIX(binary)    } },
	{ Token::Type::QuestionQuestionEquals,   { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::Slash,                    { Precedence::Factors,     nullptr,            INFIX(binary)    } },
	{ Token::Type::SlashEquals,              { Precedence::Assigns,     nullptr,            INFIX(assign)    } },
	{ Token::Type::String,                   { Precedence::Lowest,      PREFIX(string),     nullptr          } },
	{ Token::Type::Tilde,                    { Precedence::Unaries,     PREFIX(unary),      nullptr          } },
	{ Token::Type::True,                     { Precedence::Lowest,      PREFIX(literal),    nullptr          } },
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
	return expression();
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
		if (it_ == grammar_rules.end() || it_->second.precedence < prec)
			break;

		if (it_->second.infix == nullptr) {
			Log::error("Unexpected token {}, expected infix", next);
			return nullptr;
		}

		token = consume();
		node = (it_->second.infix)(this, token, std::move(node));
	}

	return node;
}

Ptr<AST::Array> Parser::array(const Token&)
{
	std::vector<Ptr<AST::Expression>> elements;

	while (!peek(Token::Type::RightBracket)) {
		auto expr = expression();
		if (!expr) return nullptr;

		elements.push_back(std::move(expr));

		if (!peek(Token::Type::Comma))
			break;
		consume(Token::Type::Comma);
	}

	return consume(Token::Type::RightBracket)
		? makeNode<AST::Array>(std::move(elements))
		: nullptr;
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

Ptr<AST::Expression> Parser::group(const Token&)
{
	auto expr = expression();
	return consume(Token::Type::RightParenthesis) ? std::move(expr) : nullptr;
}

Ptr<AST::Identifier> Parser::identifier(const Token& token)
{
	return makeNode<AST::Identifier>(std::string(token.trivia.data(), token.trivia.length()));
}

Ptr<AST::Literal> Parser::literal(const Token& token)
{
	switch (token.type) {
		case Token::Type::False: return makeNode<AST::Boolean>(false);
		case Token::Type::Null:  return makeNode<AST::Null>();
		case Token::Type::True:  return makeNode<AST::Boolean>(true);
		default:
			break;
	}
	ASSERT_NOT_REACHED();
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

Ptr<AST::Object> Parser::object(const Token&)
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

		if (!consume(Token::Type::Colon))
			return nullptr;

		auto expr = expression();
		if (!expr) return nullptr;

		members.emplace(std::move(id), std::move(expr));

		if (!peek(Token::Type::Comma))
			break;
		consume(Token::Type::Comma);
	}

	return consume(Token::Type::RightBrace)
		? makeNode<AST::Object>(std::move(members))
		: nullptr;
}

Ptr<AST::String> Parser::string(const Token& token)
{
	std::ostringstream oss;

	for (auto it = token.trivia.cbegin(); it != token.trivia.cend(); ++it) {
		oss << (*it == '\\' ? (char)parse_escape_sequence(++it) : *it);
	}

	return makeNode<AST::String>(oss.str());
}

Ptr<AST::UnaryExpression> Parser::unary(const Token& token)
{
	static const std::unordered_map<Token::Type, AST::UnaryExpression::Operators> unary_operators = {
		{ Token::Type::Exclamation, AST::UnaryExpression::Operators::BooleanNot },
		{ Token::Type::Plus,        AST::UnaryExpression::Operators::Positive   },
		{ Token::Type::PlusPlus,    AST::UnaryExpression::Operators::Increment  },
		{ Token::Type::Minus,       AST::UnaryExpression::Operators::Negative   },
		{ Token::Type::MinusMinus,  AST::UnaryExpression::Operators::Decrement  },
		{ Token::Type::Tilde,       AST::UnaryExpression::Operators::BitwiseNot },
	};

	auto rhs = expression(Precedence::Unaries);
	if (!rhs) return nullptr;

	return makeNode<AST::UnaryExpression>(
		unary_operators.at(token.type),
		std::move(rhs)
	);
}

Ptr<AST::Assignment> Parser::assign(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::Assignment::Operators> assign_operators = {
		{ Token::Type::AmpersandAmpersandEquals, AST::Assignment::Operators::BooleanAnd        },
		{ Token::Type::AmpersandEquals,          AST::Assignment::Operators::BitwiseAnd        },
		{ Token::Type::AsteriskAsteriskEquals,   AST::Assignment::Operators::Power             },
		{ Token::Type::AsteriskEquals,           AST::Assignment::Operators::Multiply          },
		{ Token::Type::CaretEquals,              AST::Assignment::Operators::BitwiseXor        },
		{ Token::Type::Equals,                   AST::Assignment::Operators::Assign            },
		{ Token::Type::GreaterGreaterEquals,     AST::Assignment::Operators::BitwiseLeftShift  },
		{ Token::Type::LessLessEquals,           AST::Assignment::Operators::BitwiseRightShift },
		{ Token::Type::MinusEquals,              AST::Assignment::Operators::Substract         },
		{ Token::Type::PercentEquals,            AST::Assignment::Operators::Modulo            },
		{ Token::Type::PipeEquals,               AST::Assignment::Operators::BitwiseOr         },
		{ Token::Type::PipePipeEquals,           AST::Assignment::Operators::BooleanOr         },
		{ Token::Type::PlusEquals,               AST::Assignment::Operators::Add               },
		{ Token::Type::QuestionQuestionEquals,   AST::Assignment::Operators::Coalesce          },
		{ Token::Type::SlashEquals,              AST::Assignment::Operators::Divide            },
	};

	auto rhs = expression(Precedence::Assigns);
	if (!rhs) return nullptr;

	return makeNode<AST::Assignment>(
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
		{ Token::Type::Dot,                AST::BinaryExpression::Operators::Access              },
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
		{ Token::Type::QuestionDot,        AST::BinaryExpression::Operators::NullsafeAccess      },
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

Ptr<AST::Call> Parser::call(const Token&, Ptr<AST::Expression> lhs)
{
	std::vector<Ptr<AST::Expression>> arguments;

	while (!peek(Token::Type::RightParenthesis)) {
		if (!arguments.empty() && !consume(Token::Type::Comma))
			return nullptr;

		auto arg = expression();
		if (!arg) return nullptr;

		arguments.push_back(std::move(arg));
	}
	ASSERT(consume(Token::Type::RightParenthesis));

	return makeNode<AST::Call>(std::move(lhs), std::move(arguments));
}

Ptr<AST::Subscript> Parser::subscript(const Token&, Ptr<AST::Expression> lhs)
{
	// Allow empty subscript expressions (eg. `expr[]`)
	if (peek(Token::Type::RightBracket)) {
		consume();
		return makeNode<AST::Subscript>(lhs);
	}

	auto expr = expression();
	return expr && consume(Token::Type::RightBracket)
		? makeNode<AST::Subscript>(lhs, std::move(expr))
		: nullptr;
}

Ptr<AST::TernaryExpression> Parser::ternary(const Token&, Ptr<AST::Expression> lhs)
{
	auto consequent = expression(Precedence::Ternary);
	if (!consequent) return nullptr;

	if (!consume(Token::Type::Colon))
		return nullptr;

	auto alternate = expression(Precedence::Ternary);
	if (!alternate) return nullptr;

	return makeNode<AST::TernaryExpression>(
		std::move(lhs),
		std::move(consequent),
		std::move(alternate)
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
