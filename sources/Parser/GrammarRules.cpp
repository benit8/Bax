/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Parser / GrammarRules.cpp
*/

#include "Bax/Parser.hpp"
#include "Common/Log.hpp"
#include <sstream>

// -----------------------------------------------------------------------------

namespace Bax
{

#define PREFIX(F) [] (Parser* p, const Token& t) { return p->F(t); }
#define INFIX(F) [] (Parser* p, const Token& t, AST::Expression* l) { return p->F(t, l); }

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
	{ Token::Type::Function,                 { Precedence::Lowest,      Associativity::Right, PREFIX(function),   nullptr           } },
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
	{ Token::Type::Match,                    { Precedence::Lowest,      Associativity::Right, PREFIX(match),      nullptr           } },
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

// -----------------------------------------------------------------------------

AST::Identifier* Parser::identifier(const Token& token)
{
	auto idx = current_scope()->find_local(token);
	if (!idx) {
		Log::error("Undefined indentifier: {}", token);
		return nullptr;
	}
	return new AST::Identifier(token);
}

AST::Null* Parser::null(const Token&)
{
	return new AST::Null();
}

AST::Boolean* Parser::boolean(const Token& token)
{
	switch (token.type) {
		case Token::Type::False: return new AST::Boolean(false);
		case Token::Type::True:  return new AST::Boolean(true);
		default:
			break;
	}
	ASSERT_NOT_REACHED();
}

AST::Glyph* Parser::glyph(const Token& token)
{
	auto t = token.trivia.cbegin();
	uint64_t value = (*t == '\\') ? parse_escape_sequence(++t) : *t;

	if (++t != token.trivia.cend()) {
		Log::error("Invalid multi-glyph expression: {}", token);
		return nullptr;
	}

	return new AST::Glyph(value);
}

AST::Number* Parser::number(const Token& token)
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

	return new AST::Number(result);
}

AST::String* Parser::string(const Token& token)
{
	std::ostringstream oss;

	for (auto it = token.trivia.cbegin(); it != token.trivia.cend(); ++it) {
		oss << (*it == '\\' ? (char)parse_escape_sequence(++it) : *it);
	}

	return new AST::String(oss.str());
}

// -----------------------------------------------------------------------------

AST::ArrayExpression* Parser::array(const Token&)
{
	std::vector<AST::Expression*> elements;

	while (!peek(Token::Type::RightBracket)) {
		auto expr = expression();
		if (!expr) return nullptr;

		elements.push_back(std::move(expr));

		if (!consume(Token::Type::Comma))
			break;
	}
	MUST_CONSUME(Token::Type::RightBracket);

	return new AST::ArrayExpression(std::move(elements));
}

AST::AssignmentExpression* Parser::assignment(const Token& token, AST::Expression* lhs)
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

	return new AST::AssignmentExpression(
		assign_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

AST::BinaryExpression* Parser::binary(const Token& token, AST::Expression* lhs)
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

	return new AST::BinaryExpression(
		binary_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

AST::CallExpression* Parser::call(const Token&, AST::Expression* lhs)
{
	std::vector<AST::Expression*> arguments;
	if (!parse_argument_list(arguments, Token::Type::RightParenthesis))
		return nullptr;
	MUST_CONSUME(Token::Type::RightParenthesis);

	return new AST::CallExpression(std::move(lhs), std::move(arguments));
}

AST::FunctionExpression* Parser::function(const Token&)
{
	MUST_CONSUME(Token::Type::LeftParenthesis);

	std::vector<AST::Identifier*> parameters;
	if (!parse_parameter_list(parameters, Token::Type::RightParenthesis))
		return nullptr;

	MUST_CONSUME(Token::Type::RightParenthesis);

	AST::Statement* body = nullptr;
	if (consume(Token::Type::EqualsGreater)) {
		auto expr = expression();
		if (!expr) return nullptr;
		body = new AST::ReturnStatement(expr);
	} else {
		body = block_statement(parameters);
	}
	if (!body) return nullptr;

	return new AST::FunctionExpression(std::move(parameters), std::move(body));
}

AST::Expression* Parser::group(const Token&)
{
	auto expr = expression();
	MUST_CONSUME(Token::Type::RightParenthesis);
	return expr;
}

AST::MatchExpression* Parser::match(const Token&)
{
	auto subject = expression();
	if (!subject) return nullptr;

	AST::MatchExpression::CasesType cases;

	MUST_CONSUME(Token::Type::LeftBrace);
	bool has_default = false;
	while (!done() && !peek(Token::Type::RightBrace)) {
		std::vector<AST::Expression*> cases_expressions;
		do {
			AST::Expression* expr = nullptr;
			if (consume(Token::Type::Default)) {
				if (has_default) {
					Log::error("Match expression already has a 'default' expression, found other {}", m_current_token);
					return nullptr;
				}
				has_default = true;
			}
			else {
				expr = expression();
				if (!expr) return nullptr;
			}
			cases_expressions.push_back(std::move(expr));
			if (!consume(Token::Type::Comma))
				break;
		} while (!peek(Token::Type::EqualsGreater));

		MUST_CONSUME(Token::Type::EqualsGreater);

		auto expr = expression();
		if (!expr) return nullptr;
		cases.push_back(std::make_pair(std::move(cases_expressions), std::move(expr)));

		if (!consume(Token::Type::Comma))
			break;
	}
	MUST_CONSUME(Token::Type::RightBrace);

	return new AST::MatchExpression(
		std::move(subject),
		std::move(cases)
	);
}

AST::MemberExpression* Parser::member(const Token& token, AST::Expression* lhs)
{
	static const std::unordered_map<Token::Type, AST::MemberExpression::Operators> member_operators = {
		{ Token::Type::Backslash,   AST::MemberExpression::Operators::Namespace },
		{ Token::Type::ColonColon,  AST::MemberExpression::Operators::Static    },
		{ Token::Type::Dot,         AST::MemberExpression::Operators::Member    },
		{ Token::Type::QuestionDot, AST::MemberExpression::Operators::Nullsafe  },
	};

	if (!detail::is<AST::Identifier, AST::MemberExpression>(lhs)) {
		Log::error("Left-hand side of member expressions must be an identifier or another member expression, found {} instead.", lhs->class_name());
		return nullptr;
	}

	auto rhs = identifier();
	if (!rhs) {
		Log::error("Right-hand side of member expressions must be an identifier, found {} instead.", rhs->class_name());
		return nullptr;
	}

	return new AST::MemberExpression(
		member_operators.at(token.type),
		std::move(lhs),
		std::move(rhs)
	);
}

AST::ObjectExpression* Parser::object(const Token&)
{
	std::map<AST::Identifier*, AST::Expression*> members;

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

	return new AST::ObjectExpression(std::move(members));
}

AST::SubscriptExpression* Parser::subscript(const Token&, AST::Expression* lhs)
{
	// Allow empty subscript expressions (eg. `expr[]`)
	if (consume(Token::Type::RightBracket)) {
		return new AST::SubscriptExpression(lhs);
	}

	auto expr = expression();
	if (!expr) return nullptr;

	MUST_CONSUME(Token::Type::RightBracket);

	return new AST::SubscriptExpression(lhs, std::move(expr));
}

AST::TernaryExpression* Parser::ternary(const Token&, AST::Expression* lhs)
{
	auto consequent = expression(Precedence::Ternary);
	if (!consequent) return nullptr;

	MUST_CONSUME(Token::Type::Colon);

	auto alternate = expression(Precedence::Ternary);
	if (!alternate) return nullptr;

	return new AST::TernaryExpression(
		std::move(lhs),
		std::move(consequent),
		std::move(alternate)
	);
}

AST::UnaryExpression* Parser::unary(const Token& token)
{
	static const std::unordered_map<Token::Type, AST::UnaryExpression::Operators> unary_operators = {
		{ Token::Type::Exclamation, AST::UnaryExpression::Operators::BooleanNot },
		{ Token::Type::Plus,        AST::UnaryExpression::Operators::Positive   },
		{ Token::Type::Minus,       AST::UnaryExpression::Operators::Negative   },
		{ Token::Type::Tilde,       AST::UnaryExpression::Operators::BitwiseNot },
	};

	auto rhs = expression(Precedence::Unaries);
	if (!rhs) return nullptr;

	return new AST::UnaryExpression(
		unary_operators.at(token.type),
		std::move(rhs)
	);
}

AST::UpdateExpression* Parser::update(const Token& token, AST::Expression* lhs)
{
	static const std::unordered_map<Token::Type, AST::UpdateExpression::Operators> update_operators = {
		{ Token::Type::PlusPlus,   AST::UpdateExpression::Operators::Increment },
		{ Token::Type::MinusMinus, AST::UpdateExpression::Operators::Decrement },
	};

	bool is_prefix_update = false;

	// `lhs` will be a `nullptr` in the case of an infix {in,de}crement.
	if (!lhs) {
		is_prefix_update = true;
		lhs = expression(Precedence::Updates);
		if (!lhs) return nullptr;
	}
	if (!detail::is<AST::Identifier, AST::MemberExpression>(lhs)) {
		Log::error(
			"{}-hand side of update operator must be an identifier or a member expression, found {} instead.",
			is_prefix_update ? "Right" : "Left",
			lhs->class_name()
		);
		return nullptr;
	}

	return new AST::UpdateExpression(
		update_operators.at(token.type),
		std::move(lhs),
		is_prefix_update
	);
}

}
