/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Op.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Common/Assertions.hpp"

// -----------------------------------------------------------------------------

#define __ENUMERATE_OP_CODES  \
	__ENUMERATE(NoOp)         \
	__ENUMERATE(PushNull)     \
	__ENUMERATE(PushFalse)    \
	__ENUMERATE(PushTrue)     \
	__ENUMERATE(PushConstant) \
	__ENUMERATE(Load)         \
	__ENUMERATE(Store)        \
	__ENUMERATE(LoadUpValue)  \
	__ENUMERATE(StoreUpValue) \
	__ENUMERATE(GetGlobal)    \
	__ENUMERATE(GetProperty)  \
	__ENUMERATE(MakeArray)    \
	__ENUMERATE(MakeTable)    \
	__ENUMERATE(Call)         \
	__ENUMERATE(Return)       \

// -----------------------------------------------------------------------------

namespace Bax
{

struct Op
{
	enum class Code {
	#define __ENUMERATE(op) op,
		__ENUMERATE_OP_CODES
	#undef __ENUMERATE
	};

	Code code { Code::NoOp };
	int arg { 0 };

	const char* code_to_string() const
	{
		switch (code) {
#define __ENUMERATE(op) case Code::op: return #op;
			__ENUMERATE_OP_CODES
#undef __ENUMERATE
		}
		ASSERT_NOT_REACHED();
	}
};

}

template <>
struct fmt::formatter<Bax::Op> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::Op& op, FormatContext& ctx) {
		return format_to(ctx.out(), "{} ({})", op.code_to_string(), op.arg);
	}
};
