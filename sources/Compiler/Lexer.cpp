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
	{ "let", Token::Type::Let },
};

const Lexer::OperatorTreeNode Lexer::operator_tree_root = {
	Token::Type::Unknown, {
		{'+', { Token::Type::Plus, {
			{'+', { Token::Type::PlusPlus, {} }},
			{'=', { Token::Type::PlusEquals, {} }}
		} }},
		{'-', { Token::Type::Minus, {
			{'-', { Token::Type::MinusMinus, {} }},
			{'=', { Token::Type::MinusEquals, {} }}
		} }},
		{'*', { Token::Type::Asterisk, {
			{'*', { Token::Type::AsteriskAsterisk, {
				{'=', { Token::Type::AsteriskAsteriskEquals, {} }}
			} }},
			{'=', { Token::Type::AsteriskEquals, {} }}
		} }},
		{'/', { Token::Type::Slash, {
			{'=', { Token::Type::SlashEquals, {} }}
		} }},
		{'=', { Token::Type::Equals, {
			{'=', { Token::Type::EqualsEquals, {} }}
		} }},
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

	if (is_eof())
		return make_token(Token::Type::Eof);

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

	// Operators
	if (next_is(ispunct)) {
		return make_token([this] {
			return descend_tree(operator_tree_root);
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
				case 'x': base = 16; break;
				case 'b': base = 2; break;
				case '1' ... '9': base = 8; break;
				default: base = 0; break;
			}
			if (base == 0)
				goto end;
			ignore();
		}
		while (!is_eof()) {
			auto p = strchr(base_digits, tolower(peek()));
			if (p == nullptr)
				break;
			int digit = p - base_digits;
			if (digit >= base)
				break;
			ignore();
			result = result * 10 + digit;
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

end:
	Token t;
	t.type = Token::Type::Number;
	t.start = start_position;
	t.end = position();
	t.trivia = m_input.substr(start_index, tell() - start_index);
	t.value.as.number = result;
	return t;
}

std::pair<Token::Type, std::string_view> Lexer::descend_tree(const OperatorTreeNode& node, size_t level)
{
	auto it = node.children.find(peek(level));
	if (it == node.children.end())
		return std::make_pair(node.type, consume(std::max(1UL, level)));
	return descend_tree(it->second, level + 1);
}

}
