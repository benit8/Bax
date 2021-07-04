/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / GenericLexer.cpp
*/

#include "Common/GenericLexer.hpp"

// -----------------------------------------------------------------------------

/**
 * Consume a number of characters.
 */
std::string_view GenericLexer::consume(size_t count)
{
	if (count == 0)
		return {};

	size_t start = m_index;
	size_t length = std::min(count, m_input.length() - m_index);
	m_index += length;

	return m_input.substr(start, length);
}

/**
 * Consume the rest of the input.
 */
std::string_view GenericLexer::consume_all()
{
	if (is_eof())
		return {};

	auto rest = m_input.substr(m_index, m_input.length() - m_index);
	m_index = m_input.length();
	return rest;
}

/**
 * Consume until a new line is found.
 */
std::string_view GenericLexer::consume_line()
{
	size_t start = m_index;
	while (!is_eof() && peek() != '\r' && peek() != '\n')
		m_index++;
	size_t length = m_index - start;

	consume_specific('\r');
	consume_specific('\n');

	if (length == 0)
		return {};
	return m_input.substr(start, length);
}

/**
 * Consume and return characters until `stop` is peek'd.
 * The `stop` character is ignored, as it is user-defined.
 */
std::string_view GenericLexer::consume_until(char stop)
{
	size_t start = m_index;
	while (!is_eof() && peek() != stop)
		m_index++;
	size_t length = m_index - start;

	ignore();

	if (length == 0)
		return {};
	return m_input.substr(start, length);
}

/**
 * Consume and return characters until the string `stop` is found.
 * The `stop` string is ignored, as it is user-defined.
 */
std::string_view GenericLexer::consume_until(const char* stop)
{
	size_t start = m_index;
	while (!is_eof() && !next_is(stop))
		m_index++;
	size_t length = m_index - start;

	ignore(__builtin_strlen(stop));

	if (length == 0)
		return {};
	return m_input.substr(start, length);
}

/**
 * Consume a string surrounded by single or double quotes. The returned
 * std::string_view does not include the quotes. An escape character can be provided
 * to capture the enclosing quotes. Please note that the escape character will
 * still be in the resulting std::string_view
 */
std::string_view GenericLexer::consume_quoted_string(char escape_char)
{
	if (!next_is(is_quote))
		return {};

	char quote_char = consume();
	size_t start = m_index;
	while (!is_eof()) {
		if (next_is(escape_char))
			m_index++;
		else if (next_is(quote_char))
			break;
		m_index++;
	}
	size_t length = m_index - start;

	if (peek() != quote_char) {
		// Restore the index in case the string is unterminated
		m_index = start - 1;
		return {};
	}

	// Ignore closing quote
	ignore();

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
	return str;
}
