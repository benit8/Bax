/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / GenericLexer.cpp
*/

#include "Common/GenericLexer.hpp"

// -----------------------------------------------------------------------------

std::string_view GenericLexer::consume(size_t count)
{
	if (count == 0)
		return {};

	size_t start = tell();
	size_t length = std::min(count, tell_remaining());
	advance(length);
	return m_input.substr(start, length);
}

std::string_view GenericLexer::consume_all()
{
	if (is_eof())
		return {};

	auto rest = m_input.substr(tell(), tell_remaining());
	advance(tell_remaining());
	return rest;
}

std::string_view GenericLexer::consume_line()
{
	size_t start = tell();
	while (!is_eof() && peek() != '\r' && peek() != '\n')
		advance(1);
	size_t length = tell() - start;

	consume_specific('\r');
	consume_specific('\n');

	if (length == 0)
		return {};
	return m_input.substr(start, length);
}

std::string_view GenericLexer::consume_quoted_string(char escape_char)
{
	if (!next_is(is_quote))
		return {};

	auto start_position = position();
	char quote_char = consume();
	size_t start = tell();

	while (!is_eof() && !next_is(quote_char)) {
		if (next_is(escape_char))
			advance(1);
		advance(1);
	}
	size_t length = tell() - start;

	if (peek() != quote_char) {
		// Restore the index in case the string is unterminated
		m_index = start - 1;
		m_position = start_position;
		return {};
	}

	// Ignore closing quote
	ignore(1);

	return m_input.substr(start, length);
}

std::string GenericLexer::consume_and_unescape_string(char escape_char)
{
	auto view = consume_quoted_string(escape_char);
	if (view.empty())
		return {};

	std::string str;
	for (size_t i = 0; i < view.length(); ++i)
		str.push_back(consume_escaped_character(escape_char));
	str.shrink_to_fit();
	return str;
}
