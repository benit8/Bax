/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.cpp
*/

#include "Bax/Compiler/Parser.hpp"
#include "Common/Log.hpp"
#include <iostream>

// -----------------------------------------------------------------------------

namespace Bax
{

const std::unordered_map<Token::Type, Parser::GrammarRule> Parser::grammar_rules = {
	{ Token::Type::Asterisk,   { Precedence::Factor, nullptr,             &Parser::binary } },
	{ Token::Type::Equals,     { Precedence::Assign, nullptr,             &Parser::binary } },
	{ Token::Type::Identifier, { Precedence::Lowest, &Parser::identifier, nullptr } },
	{ Token::Type::Minus,      { Precedence::Term,   &Parser::unary,      &Parser::binary } },
	{ Token::Type::Number,     { Precedence::Lowest, &Parser::literal,    nullptr } },
	{ Token::Type::Plus,       { Precedence::Term,   &Parser::unary,      &Parser::binary } },
	{ Token::Type::Slash,      { Precedence::Factor, nullptr,             &Parser::binary } },
};

// -----------------------------------------------------------------------------

Parser::Parser(const std::string_view& source)
: m_lexer(source)
, m_current_token(m_lexer.next())
{}

Parser::~Parser()
{}

const Token& Parser::peek() const
{
	return m_current_token;
}

Token Parser::consume()
{
	Token t = m_current_token;
	m_current_token = m_lexer.next();
	return t;
}

AST::Node* Parser::run()
{
	// for (Token t = consume(); t.type != Token::Type::Eof; t = consume()) Log::debug("{}", t);
	return expression();
}

AST::Expression* Parser::expression(Parser::Precedence prec)
{
	auto token = consume();
	if (token.type == Token::Type::Eof) {
		Log::notice("Reached end of file");
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

	auto node = (this->*rule.prefix)(token);

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
		node = (this->*it_->second.infix)(token, node);
	}

	return node;
}

AST::Expression* Parser::literal(const Token& token)
{
	return new AST::Number(token.value.as.number);
}

AST::Expression* Parser::identifier(const Token& token)
{
	return new AST::Identifier(std::string(token.trivia.data(), token.trivia.length()));
}

AST::Expression* Parser::unary(const Token& token)
{
	static const std::unordered_map<Token::Type, AST::UnaryExpression::Operators> unary_operators = {
		{ Token::Type::Plus,  AST::UnaryExpression::Operators::Positive },
		{ Token::Type::Minus, AST::UnaryExpression::Operators::Negative },
	};
	return new AST::UnaryExpression(
		unary_operators.at(token.type),
		expression(Precedence::Unary)
	);
}

AST::Expression* Parser::binary(const Token& token, AST::Expression* lhs)
{
	static const std::unordered_map<Token::Type, AST::BinaryExpression::Operators> binary_operators = {
		{ Token::Type::Asterisk, AST::BinaryExpression::Operators::Multiply },
		{ Token::Type::Equals,   AST::BinaryExpression::Operators::Assign },
		{ Token::Type::Minus,    AST::BinaryExpression::Operators::Substract },
		{ Token::Type::Plus,     AST::BinaryExpression::Operators::Add },
		{ Token::Type::Slash,    AST::BinaryExpression::Operators::Divide },
	};
	return new AST::BinaryExpression(
		binary_operators.at(token.type),
		lhs,
		expression(grammar_rules.at(token.type).precedence)
	);
}

}
