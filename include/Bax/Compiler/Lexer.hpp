/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Lexer.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/Token.hpp"
#include "Common/GenericLexer.hpp"
#include <algorithm>
#include <map>
#include <unordered_map>

// -----------------------------------------------------------------------------

namespace Bax
{

class Lexer : public GenericLexer
{
private:
	static constexpr const char* inline_comment_start = "//";
	static constexpr const char* block_comment_start  = "/*";
	static constexpr const char* block_comment_end    = "*/";

	static constexpr auto is_identifier_start = [] (char c) { return isalpha(c) || c == '_'; };
	static constexpr auto is_identifier_body  = [] (char c) { return isalnum(c) || c == '_'; };

	static const std::unordered_map<std::string_view, Token::Type> keywords;

	struct OperatorTreeNode {
		Token::Type type;
		std::map<char, OperatorTreeNode> children;
	};
	static const OperatorTreeNode operator_tree_root;

public:
	Lexer(const std::string_view& source);
	~Lexer();

	Token next();

private:
	template <typename L>
	Token make_token(Token::Type type, L consumer)
	{
		Token t;
		t.type = type;
		t.start = position();
		t.trivia = consumer();
		t.end = position();
		return t;
	}

	template <typename L>
	Token make_token(L consumer) {
		Token t;
		t.start = position();

		std::pair<Token::Type, std::string_view> res = consumer();
		t.type = res.first;
		t.trivia = res.second;
		t.end = position();
		return t;
	}

	Token make_token(Token::Type type) {
		return make_token(type, [this] () { return consume(1); });
	}

	std::pair<Token::Type, std::string_view> descend_operator_tree(const OperatorTreeNode& node, size_t level = 0);

	Token lex_glyph();
	Token lex_number();
	Token lex_string();
};

}
