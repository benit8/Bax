/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.cpp
*/

#include "Bax/Compiler/Parser.hpp"
#include "Common/Assertions.hpp"
#include "Common/Log.hpp"
#include <iostream>

// -----------------------------------------------------------------------------

namespace Bax
{

const std::unordered_map<Token::Type, Parser::GrammarRule> Parser::grammar_rules = {
	{ Token::Type::Asterisk,        { Precedence::Factor,   nullptr,             &Parser::binary } },
	{ Token::Type::Equals,          { Precedence::Assign,   nullptr,             &Parser::binary } },
	{ Token::Type::Identifier,      { Precedence::Lowest,   &Parser::identifier, nullptr         } },
	{ Token::Type::LeftParenthesis, { Precedence::Property, &Parser::group,      &Parser::call   } },
	{ Token::Type::Minus,           { Precedence::Term,     &Parser::unary,      &Parser::binary } },
	{ Token::Type::Number,          { Precedence::Lowest,   &Parser::literal,    nullptr         } },
	{ Token::Type::Plus,            { Precedence::Term,     &Parser::unary,      &Parser::binary } },
	{ Token::Type::Slash,           { Precedence::Factor,   nullptr,             &Parser::binary } },
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
	if (consume().type != type) {
		Log::error("Unexpected token {}, expected {}", m_current_token, Token::type_to_string(type));
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
		node = (this->*it_->second.infix)(token, std::move(node));
	}

	return node;
}

Ptr<AST::Expression> Parser::literal(const Token& token)
{
	return makeNode<AST::Number>(token.value.as.number);
}

Ptr<AST::Expression> Parser::identifier(const Token& token)
{
	return makeNode<AST::Identifier>(std::string(token.trivia.data(), token.trivia.length()));
}

Ptr<AST::Expression> Parser::group(const Token&)
{
	auto expr = expression();
	return consume(Token::Type::RightParenthesis) ? std::move(expr) : nullptr;
}

Ptr<AST::Expression> Parser::unary(const Token& token)
{
	static const std::unordered_map<Token::Type, AST::UnaryExpression::Operators> unary_operators = {
		{ Token::Type::Plus,  AST::UnaryExpression::Operators::Positive },
		{ Token::Type::Minus, AST::UnaryExpression::Operators::Negative },
	};
	return makeNode<AST::UnaryExpression>(
		unary_operators.at(token.type),
		expression(Precedence::Unary)
	);
}

Ptr<AST::Expression> Parser::binary(const Token& token, Ptr<AST::Expression> lhs)
{
	static const std::unordered_map<Token::Type, AST::BinaryExpression::Operators> binary_operators = {
		{ Token::Type::Asterisk, AST::BinaryExpression::Operators::Multiply  },
		{ Token::Type::Equals,   AST::BinaryExpression::Operators::Assign    },
		{ Token::Type::Minus,    AST::BinaryExpression::Operators::Substract },
		{ Token::Type::Plus,     AST::BinaryExpression::Operators::Add       },
		{ Token::Type::Slash,    AST::BinaryExpression::Operators::Divide    },
	};
	return makeNode<AST::BinaryExpression>(
		binary_operators.at(token.type),
		std::move(lhs),
		expression(grammar_rules.at(token.type).precedence)
	);
}

Ptr<AST::Expression> Parser::call(const Token&, Ptr<AST::Expression> lhs)
{
	std::vector<Ptr<AST::Expression>> arguments;

	while (!peek(Token::Type::RightParenthesis)) {
		if (!arguments.empty() && !consume(Token::Type::Comma))
			return nullptr;
		arguments.push_back(expression());
	}
	ASSERT(consume(Token::Type::RightParenthesis));

	return makeNode<AST::CallExpression>(std::move(lhs), std::move(arguments));
}

}
