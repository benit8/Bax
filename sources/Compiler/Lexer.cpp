/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Lexer.cpp
*/

#include "Lexer.hpp"
#include <cctype>
#include <cstdlib>

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
	if (next_is(isdigit) || next_is('.')) {
		return make_token(Token::Type::Number, [this] {
			const char* start = m_input.data() + m_index;
			char* end = NULL;
			double res = strtod(start, &end);
			// FIXME: check errno for ERANGE
			return consume(end - start);
		});
	}

	// Operators
	if (next_is(ispunct)) {
		return make_token([this] {
			return descend_tree(operator_tree_root);
		});
	}

	return make_token(Token::Type::Unknown);
}

}
