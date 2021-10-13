/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Token.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Compiler/TokenTypes.hpp"
#include "Bax/VM/Value.hpp"
#include "Common/GenericLexer.hpp"
#include "fmt/format.h"
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

	Type type { Type::Unknown };

	static const char* type_to_string(Token::Type);
	const char* type_to_string() const { return type_to_string(type); }
};

}

template <>
struct fmt::formatter<Bax::Token> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::Token& t, FormatContext& ctx) {
		return format_to(ctx.out(), "{}(\"{}\")[{}->{}]", t.type_to_string(), t.trivia, t.start, t.end);
	}
};
