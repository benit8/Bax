/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Token.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Common/GenericLexer.hpp"
#include "TokenTypes.hpp"
#include <ostream>

// -----------------------------------------------------------------------------

namespace Bax
{

struct Token : public GenericToken
{
	enum class Type {
#define __ENUMERATE(T) T,
		__ENUMERATE_TOKEN_TYPES
#undef __ENUMERATE
	};

	Type type;

	static const char* type_to_string(Token::Type);
	const char* type_to_string() const { return type_to_string(type); }

	friend std::ostream& operator<<(std::ostream& os, const Token& t) {
		return os << t.start << ',' << t.end << ':' << t.type_to_string() << '{' << t.trivia << '}';
	}
};

}
