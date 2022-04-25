/*
** Bax Tests, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Unit test
*/

#include "Bax/Lexer.hpp"
#include "gtest/gtest.h"

// -----------------------------------------------------------------------------

TEST(Lexer, EmptySource)
{
	std::string_view source = "";
	Bax::Lexer lexer(source);

	ASSERT_TRUE(lexer.is_eof());

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Eof);
}

TEST(Lexer, UnknownCharacter)
{
	std::string_view source = "$";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Unknown);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 2);
}

TEST(Lexer, InvalidNumber)
{
	std::string_view source = "0a";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source.substr(0, 1));
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 2);
}

TEST(Lexer, NumberInteger)
{
	std::string_view source = "12";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 3);
}

TEST(Lexer, NumberDecimal)
{
	std::string_view source = "64.265";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 7);
}

TEST(Lexer, NumberExponent)
{
	std::string_view source = "3e5";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 4);
}

TEST(Lexer, NumberNegativeExponent)
{
	std::string_view source = "3e-5";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 5);
}

TEST(Lexer, NumberBinary)
{
	std::string_view source = "0b00110011";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 11);
}

TEST(Lexer, NumberOctal)
{
	std::string_view source = "0o644";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 6);
}

TEST(Lexer, NumberHexadecimal)
{
	std::string_view source = "0xDEADBEEF";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Number);
	ASSERT_EQ(token.trivia, source);
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 11);
}

TEST(Lexer, IdentifierAndKeywords)
{
	std::string_view source = "var str ";
	Bax::Lexer lexer(source);

	auto token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Var);
	ASSERT_EQ(token.trivia, source.substr(0, 3));
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 1);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 4);

	token = lexer.next();
	ASSERT_EQ(token.type, Bax::Token::Type::Identifier);
	ASSERT_EQ(token.trivia, source.substr(4, 3));
	ASSERT_EQ(token.start.line, 1);
	ASSERT_EQ(token.start.column, 5);
	ASSERT_EQ(token.end.line, 1);
	ASSERT_EQ(token.end.column, 8);
}

TEST(Lexer, Operators)
{
	std::string_view source = "+ - * / = += -= *= /= == **= ++ --";
	Bax::Lexer lexer(source);

	ASSERT_EQ(lexer.next().type, Bax::Token::Type::Plus);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::Minus);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::Asterisk);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::Slash);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::Equals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::PlusEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::MinusEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::AsteriskEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::SlashEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::EqualsEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::AsteriskAsteriskEquals);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::PlusPlus);
	ASSERT_EQ(lexer.next().type, Bax::Token::Type::MinusMinus);
}
