/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Token.cpp
*/

#include "Bax/Compiler/Token.hpp"
#include "Common/Assertions.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

const char* Token::type_to_string(Token::Type t)
{
	switch (t) {
#define __ENUMERATE(T) case Type::T: return #T;
		__ENUMERATE_TOKEN_TYPES
#undef __ENUMERATE
	}
	ASSERT_NOT_REACHED();
}

}
