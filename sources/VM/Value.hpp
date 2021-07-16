/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Value.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

namespace Bax
{

struct Value
{
	enum class Type {
		Null,
		Bool,
		Number,
	} type { Type::Null };

	union {
		double number;
		void* object;
	} as { 0.0 };
};

}
