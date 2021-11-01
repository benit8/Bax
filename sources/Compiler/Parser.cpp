/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Parser.cpp
*/

#include "Bax/Compiler/Parser.hpp"
#include "Common/Assertions.hpp"
#include "Common/Log.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

const std::array<Token::Type, 3> Parser::declaration_tokens = {
	Token::Type::Const,
	Token::Type::Let,
	Token::Type::Static,
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

bool Parser::done() const
{
	return peek(Token::Type::Eof);
}

Ptr<AST::Node> Parser::run()
{
	std::vector<Ptr<AST::Statement>> statements;

	while (!done()) {
		auto stmt = top_level_statement();
		if (!stmt) return nullptr;
		statements.push_back(std::move(stmt));
	}

	return makeNode<AST::BlockStatement>(std::move(statements));
}

// -----------------------------------------------------------------------------

Ptr<AST::Statement> Parser::top_level_statement()
{
	auto stmt = any_statement();
	// if (stmt && !detail::is<AST::ClassDeclaration/*, AST::ImportStatement, AST::NamespaceStatement*/>(stmt)) {
	// 	Log::error("Unexpected {}, expected top level statement", stmt->class_name());
	// 	return nullptr;
	// }
	return stmt;
}

Ptr<AST::Statement> Parser::any_statement()
{
	if (CONTAINS(declaration_tokens, m_current_token.type))
		return declaration();
	else if (CONTAINS(statement_tokens, m_current_token.type))
		return statement();
	return nullptr;
}

Ptr<AST::Declaration> Parser::declaration()
{
	switch (m_current_token.type) {
		case Token::Type::Const:
		case Token::Type::Let:
		case Token::Type::Static:
			return variable_declaration();
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
		case Token::Type::Return:     return return_statement();
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
		 || (it_->second.precedence == prec && it_->second.associativity == Associativity::Left)
		 || it_->second.infix == nullptr) {
			break;
		}

		token = consume();
		node = (it_->second.infix)(this, token, std::move(node));
		if (!node) return nullptr;
	}

	return node;
}

// -----------------------------------------------------------------------------

Ptr<AST::Identifier> Parser::identifier()
{
	auto token = consume();
	if (token.type != Token::Type::Identifier) {
		Log::error("Unexpected token {}, expected identifier", m_current_token);
		return nullptr;
	}
	return makeNode<AST::Identifier>(std::string(token.trivia.data(), token.trivia.length()));
}

// -----------------------------------------------------------------------------

Ptr<AST::BlockStatement> Parser::block_statement()
{
	MUST_CONSUME(Token::Type::LeftBrace);

	std::vector<Ptr<AST::Statement>> statements;
	while (!done() && !peek(Token::Type::RightBrace)) {
		auto stmt = any_statement();
		if (!stmt) {
			Log::error("Unexpected token {}, expected statement or declaration", m_current_token);
			return nullptr;
		} else if (!detail::is<AST::VariableDeclaration, AST::Statement>(stmt)) {
			Log::error("Unexpected {}, expected statement or variable declaration", stmt->class_name());
			return nullptr;
		}
		statements.push_back(std::move(stmt));
	}

	MUST_CONSUME(Token::Type::RightBrace);

	return makeNode<AST::BlockStatement>(std::move(statements));
}

Ptr<AST::ExpressionStatement> Parser::expression_statement()
{
	auto expr = expression();
	if (!expr) return nullptr;

	MUST_CONSUME(Token::Type::Semicolon);

	if (!detail::is<AST::AssignmentExpression, AST::CallExpression, AST::UpdateExpression>(expr)) {
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

Ptr<AST::ReturnStatement> Parser::return_statement()
{
	MUST_CONSUME(Token::Type::Return);

	auto expr = expression();
	if (!expr) return nullptr;

	MUST_CONSUME(Token::Type::Semicolon);
	return makeNode<AST::ReturnStatement>(std::move(expr));
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

Ptr<AST::VariableDeclaration> Parser::variable_declaration()
{
	bool is_static = consume(Token::Type::Static);

	bool is_constant = consume(Token::Type::Const);
	if (!is_constant && !consume(Token::Type::Let)) {
		Log::error("Unexpected token {}, expected 'let' or 'const'", m_current_token);
		return nullptr;
	}

	auto name = identifier();
	if (!name) return nullptr;

	MUST_CONSUME(Token::Type::Equals);

	auto value = expression();
	if (!value) return nullptr;

	MUST_CONSUME(Token::Type::Semicolon);

	return makeNode<AST::VariableDeclaration>(
		std::move(name),
		std::move(value),
		is_constant,
		is_static
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

bool Parser::parse_argument_list(std::vector<Ptr<AST::Expression>>& arguments, Token::Type stop)
{
	while (!peek(stop)) {
		if (!arguments.empty() && !must_consume(Token::Type::Comma))
			return false;

		auto arg = expression();
		if (!arg) return false;

		arguments.push_back(std::move(arg));
	}

	return true;
}

bool Parser::parse_parameter_list(std::vector<Ptr<AST::Expression>>& parameters, Token::Type stop)
{
	while (!peek(stop)) {
		if (!parameters.empty() && !must_consume(Token::Type::Comma))
			return false;

		auto param = expression();
		if (!param) return false;

		parameters.push_back(std::move(param));
	}

	return true;
}

}
