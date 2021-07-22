/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** TokenTypes.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#define __ENUMERATE_TOKEN_TYPES \
	__ENUMERATE(Asterisk) \
	__ENUMERATE(AsteriskAsterisk) \
	__ENUMERATE(AsteriskAsteriskEquals) \
	__ENUMERATE(AsteriskEquals) \
	__ENUMERATE(Eof) \
	__ENUMERATE(Equals) \
	__ENUMERATE(EqualsEquals) \
	__ENUMERATE(Identifier) \
	__ENUMERATE(Let) \
	__ENUMERATE(Minus) \
	__ENUMERATE(MinusEquals) \
	__ENUMERATE(MinusMinus) \
	__ENUMERATE(Number) \
	__ENUMERATE(Plus) \
	__ENUMERATE(PlusEquals) \
	__ENUMERATE(PlusPlus) \
	__ENUMERATE(Slash) \
	__ENUMERATE(SlashEquals) \
	__ENUMERATE(Unknown)
