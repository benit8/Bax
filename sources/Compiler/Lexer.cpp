/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Lexer.cpp
*/

#include "Bax/Compiler/Lexer.hpp"
#include "Common/Log.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <functional>

// -----------------------------------------------------------------------------

namespace Bax
{

const std::unordered_map<std::string_view, Token::Type> Lexer::keywords = {
	{ "class",      Token::Type::Class      },
	{ "const",      Token::Type::Const      },
	{ "else",       Token::Type::Else       },
	{ "extends",    Token::Type::Extends    },
	{ "false",      Token::Type::False      },
	{ "for",        Token::Type::For        },
	{ "if",         Token::Type::If         },
	{ "implements", Token::Type::Implements },
	{ "let",        Token::Type::Let        },
	{ "null",       Token::Type::Null       },
	{ "private",    Token::Type::Private    },
	{ "protected",  Token::Type::Protected  },
	{ "public",     Token::Type::Public     },
	{ "return",     Token::Type::Return     },
	{ "true",       Token::Type::True       },
	{ "while",      Token::Type::While      },
};

const Lexer::OperatorTreeNode Lexer::operator_tree_root = {
	Token::Type::Unknown, {
		{'!', { Token::Type::Exclamation, {
			{'=', { Token::Type::ExclamationEquals, {} }}
		} }},
		{'%', { Token::Type::Percent, {
			{'=', { Token::Type::PercentEquals, {} }}
		} }},
		{'&', { Token::Type::Ampersand, {
			{'&', { Token::Type::AmpersandAmpersand, {
				{'=', { Token::Type::AmpersandAmpersandEquals, {} }}
			} }},
			{'=', { Token::Type::AmpersandEquals, {} }}
		} }},
		{'(', { Token::Type::LeftParenthesis, {} }},
		{')', { Token::Type::RightParenthesis, {} }},
		{'*', { Token::Type::Asterisk, {
			{'*', { Token::Type::AsteriskAsterisk, {
				{'=', { Token::Type::AsteriskAsteriskEquals, {} }}
			} }},
			{'=', { Token::Type::AsteriskEquals, {} }}
		} }},
		{'+', { Token::Type::Plus, {
			{'+', { Token::Type::PlusPlus, {} }},
			{'=', { Token::Type::PlusEquals, {} }}
		} }},
		{',', { Token::Type::Comma, {} }},
		{'-', { Token::Type::Minus, {
			{'-', { Token::Type::MinusMinus, {} }},
			{'=', { Token::Type::MinusEquals, {} }}
		} }},
		{'.', { Token::Type::Dot, {} }},
		{'/', { Token::Type::Slash, {
			{'=', { Token::Type::SlashEquals, {} }}
		} }},
		{':', { Token::Type::Colon, {} }},
		{';', { Token::Type::Semicolon, {} }},
		{'<', { Token::Type::Less, {
			{'<', { Token::Type::LessLess, {
				{'=', { Token::Type::LessLessEquals, {} }}
			} }},
			{'=', { Token::Type::LessEquals, {} }}
		} }},
		{'=', { Token::Type::Equals, {
			{'=', { Token::Type::EqualsEquals, {} }}
		} }},
		{'>', { Token::Type::Greater, {
			{'>', { Token::Type::GreaterGreater, {
				{'=', { Token::Type::GreaterGreaterEquals, {} }}
			} }},
			{'=', { Token::Type::GreaterEquals, {} }}
		} }},
		{'?', { Token::Type::Question, {
			{'.', { Token::Type::QuestionDot, {} }},
			{':', { Token::Type::QuestionColon, {} }},
			{'?', { Token::Type::QuestionQuestion, {
				{'=', { Token::Type::QuestionQuestionEquals, {} }}
			} }},
		} }},
		{'[', { Token::Type::LeftBracket, {} }},
		{']', { Token::Type::RightBracket, {} }},
		{'^', { Token::Type::Caret, {
			{'=', { Token::Type::CaretEquals, {} }}
		} }},
		{'{', { Token::Type::LeftBrace, {} }},
		{'|', { Token::Type::Pipe, {
			{'|', { Token::Type::PipePipe, {
				{'=', { Token::Type::PipePipeEquals, {} }}
			} }},
			{'=', { Token::Type::PipeEquals, {} }}
		} }},
		{'}', { Token::Type::RightBrace, {} }},
		{'~', { Token::Type::Tilde, {} }},
	}
};

// -----------------------------------------------------------------------------

Lexer::Lexer(const std::string_view& source)
: GenericLexer(source)
{}

Lexer::~Lexer()
{}

Token Lexer::next()
{
	// Skip whitespaces and comments
	while (true) {
		ignore_while(isspace);
		if (next_is(inline_comment_start))
			ignore_line();
		else if (next_is(block_comment_start))
			ignore_until(block_comment_end);
		else
			break;
	}

	if (is_eof()) {
		return make_token(Token::Type::Eof);
	}

	// Identifiers
	if (next_is(is_identifier_start)) {
		auto token = make_token(Token::Type::Identifier, [this] {
			return consume_while(is_identifier_body);
		});
		auto it = keywords.find(token.trivia);
		if (it != keywords.end())
			token.type = it->second;
		return token;
	}

	// Numbers
	if (next_is(isdigit) || (next_is('.') && isdigit(peek(1)))) {
		return lex_number();
	}

	// Glyphs
	if (next_is('\'')) {
		return lex_glyph();
	}

	// Strings
	if (next_is('"')) {
		return lex_string();
	}

	// Operators
	if (next_is(ispunct)) {
		return make_token([this] {
			return descend_operator_tree(operator_tree_root);
		});
	}

	return make_token(Token::Type::Unknown);
}

Token Lexer::lex_number()
{
	static auto base_digits = "0123456789abcdef";

	int base = 10;
	double result = 0;
	auto start_index = tell();
	auto start_position = position();

	if (next_is(isdigit)) {
		if (consume_specific('0')) {
			switch (tolower(peek())) {
				case 'x':
					base = 16;
					ignore(1);
					break;
				case 'b':
					base = 2;
					ignore(1);
					break;
				case 'o':
					base = 8;
					ignore(1);
					break;
				case '.':
					break;
			}
		}
		while (!is_eof()) {
			auto p = strchr(base_digits, tolower(peek()));
			if (p == nullptr)
				break;
			int digit = p - base_digits;
			if (digit >= base)
				break;
			ignore(1);
			result = result * base + digit;
		}
	}

	if (base == 10 && consume_specific('.')) {
		int fraction = 0;
		unsigned divider = 1;
		for (; next_is(isdigit); divider *= 10)
			fraction = fraction * 10 + (consume() - '0');
		result += fraction / (double)divider;
	}

	if (base == 10 && tolower(peek()) == 'e') {
		ignore();
		bool is_exponent_negative = false;
		if (consume_specific('+')) {}
		else if (consume_specific('-'))
			is_exponent_negative = true;
		int exponent = 0;
		while (next_is(isdigit))
			exponent = exponent * 10 + (consume() - '0');
		result *= pow(10, is_exponent_negative ? -exponent : exponent);
	}

	Token t;
	t.type = Token::Type::Number;
	t.start = start_position;
	t.end = position();
	t.trivia = m_input.substr(start_index, tell() - start_index);
	t.value.as.number = result;
	return t;
}

Token Lexer::lex_glyph()
{
	return make_token(Token::Type::Glyph, [this] {
		return consume_until('\'');
	});
}

Token Lexer::lex_string()
{
	return make_token(Token::Type::String, [this] {
		return consume_until('"');
	});
}

std::pair<Token::Type, std::string_view> Lexer::descend_operator_tree(const OperatorTreeNode& node, size_t level)
{
	auto it = node.children.find(peek(level));
	if (it == node.children.end())
		return std::make_pair(node.type, consume(std::max(1UL, level)));
	return descend_operator_tree(it->second, level + 1);
}

}
