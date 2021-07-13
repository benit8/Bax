/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / GenericLexer.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Assertions.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <functional>
#include <optional>
#include <ostream>
#include <string_view>

// -----------------------------------------------------------------------------

class GenericLexer
{
public:
	struct Position {
		size_t column = 1;
		size_t line = 1;
	};

protected:
	std::string_view m_input;
	size_t m_index = 0;
	Position m_position;

public:
	constexpr explicit GenericLexer(std::string_view input)
	: m_input(input)
	{}

	constexpr size_t tell() const { return m_index; }
	constexpr size_t tell_remaining() const { return m_input.length() - m_index; }
	std::string_view remaining() const { return m_input.substr(m_index); }
	const Position& position() const { return m_position; }

	constexpr bool is_eof() const
	{
		return m_index >= m_input.length();
	}

	constexpr char peek(size_t offset = 0) const
	{
		return (m_index + offset < m_input.length()) ? m_input[m_index + offset] : '\0';
	}

	constexpr char consume()
	{
		ASSERT(!is_eof());
		char c = peek();
		advance(1);
		return c;
	}

	constexpr void retreat()
	{
		ASSERT(tell() > 0);
		advance(1, false);
	}

	constexpr bool next_is(char expected) const
	{
		return peek() == expected;
	}

	constexpr bool next_is(const char* expected) const
	{
		for (size_t i = 0; expected[i] != '\0'; ++i)
			if (peek(i) != expected[i])
				return false;
		return true;
	}

	constexpr bool next_is(std::string_view expected) const
	{
		for (size_t i = 0; i < expected.length(); ++i)
			if (peek(i) != expected[i])
				return false;
		return true;
	}

	template<typename TPredicate>
	constexpr bool next_is(TPredicate pred) const { return pred(peek()); }

	std::string_view consume(size_t count);
	std::string_view consume_all();
	std::string_view consume_line();
	std::string_view consume_quoted_string(char escape_char = 0);
	std::string consume_and_unescape_string(char escape_char = '\\');
	constexpr char consume_escaped_character(char escape_char = '\\', std::string_view escape_map = "n\nr\rt\tb\bf\f")
	{
		if (!consume_specific(escape_char))
			return consume();

		auto c = consume();
		for (size_t i = 0; i < escape_map.length(); i += 2) {
			if (c == escape_map[i])
				return escape_map[i + 1];
		}
		return c;
	}

	template<typename T>
	constexpr bool consume_specific(const T& next)
	{
		if (!next_is(next))
			return false;
		if constexpr (requires { next.length(); })
			ignore(next.length());
		else
			ignore(sizeof(next));
		return true;
	}

	template<typename TPredicate>
	std::string_view consume_while(TPredicate pred)
	{
		size_t start = tell();
		while (!is_eof() && pred(peek()))
			advance(1);
		size_t length = tell() - start;

		if (length == 0)
			return {};
		return m_input.substr(start, length);
	}

	template <typename T>
	std::string_view consume_until(const T& stop)
	{
		size_t start = tell();
		while (!is_eof() && peek() != stop)
			advance(1);
		size_t length = tell() - start;

		if constexpr (requires { stop.length(); })
			ignore(stop.length());
		else
			ignore(sizeof(stop));

		if (length == 0)
			return {};
		return m_input.substr(start, length);
	}

	template<typename TPredicate>
	std::string_view consume_until(TPredicate pred)
	{
		size_t start = tell();
		while (!is_eof() && !pred(peek()))
			advance(1);
		size_t length = tell() - start;

		if (length == 0)
			return {};
		return m_input.substr(start, length);
	}

	constexpr void ignore(size_t count = 1)
	{
		count = std::min(count, tell_remaining());
		advance(count);
	}

	constexpr void ignore_line()
	{
		ignore_until('\n');
	}

	constexpr void ignore_until(char stop)
	{
		while (!is_eof() && !next_is(stop)) {
			advance(1);
		}
		ignore(1);
	}

	constexpr void ignore_until(const char* stop)
	{
		while (!is_eof() && !next_is(stop)) {
			advance(1);
		}
		ignore(__builtin_strlen(stop));
	}

	template<typename TPredicate>
	constexpr void ignore_while(TPredicate pred)
	{
		while (!is_eof() && pred(peek()))
			advance(1);
	}

	template<typename TPredicate>
	constexpr void ignore_until(TPredicate pred)
	{
		while (!is_eof() && !pred(peek()))
			advance(1);
	}

protected:
	constexpr void advance(size_t count, bool forward = true)
	{
		if (count == 0)
			return;

		if (forward)
			ASSERT(count <= tell_remaining());
		else
			ASSERT(count <= m_index);

		while (count--) {
			if (forward) {
				if (m_input[m_index++] == '\n') {
					m_position.line++;
					m_position.column = 1;
				}
				else {
					m_position.column++;
				}
			}
			else {
				if (m_input[--m_index] == '\n') {
					m_position.line--;
					m_position.column = 1;
					// Iterate backwards to the beginning of the line
					for (size_t i = 0; i < m_index && m_input[m_index - i - 1] != '\n'; ++i)
						m_position.column++;
				}
				else {
					m_position.column--;
				}
			}
		}
	}
};

// -----------------------------------------------------------------------------

struct GenericToken
{
	std::string_view trivia;
	GenericLexer::Position start, end;
};

// -----------------------------------------------------------------------------

constexpr auto is_any_of(const std::string_view& values)
{
	return [values](auto c) { return values.find(c) != std::string_view::npos; };
}

constexpr auto is_path_separator = is_any_of("/\\");
constexpr auto is_quote = is_any_of("'\"");

// -----------------------------------------------------------------------------

template <>
struct fmt::formatter<GenericLexer::Position> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const GenericLexer::Position& p, FormatContext& ctx) {
		return format_to(ctx.out(), "l{}:{}", p.line, p.column);
	}
};

template <>
struct fmt::formatter<GenericToken> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const GenericToken& t, FormatContext& ctx) {
		return format_to(ctx.out(), "{}>{}:\"{}\"", t.start, t.end, t.trivia);
	}
};
