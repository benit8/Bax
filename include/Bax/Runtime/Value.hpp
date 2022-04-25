/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Value.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Object.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

struct Value
{
	enum class Type {
		Null,
		False,
		True,
		Number,
		Object,
	} type { Type::Null };

	union {
		bool boolean;
		double number;
		Object* object;
	} as { false };

	Value() = default;

	explicit Value(Type t)
	: type(t)
	{}

	explicit Value(bool value)
	: type(value ? Value::Type::True : Value::Type::False)
	{}

	explicit Value(double n)
	: type(Value::Type::Number)
	, as(n)
	{}
};

}

template <>
struct fmt::formatter<Bax::Value> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::Value& v, FormatContext& ctx) {
		switch (v.type) {
			case Bax::Value::Type::Null:
				return format_to(ctx.out(), "null");
			case Bax::Value::Type::False:
				return format_to(ctx.out(), "false");
			case Bax::Value::Type::True:
				return format_to(ctx.out(), "true");
			case Bax::Value::Type::Number:
				return format_to(ctx.out(), "{}", v.as.number);
			case Bax::Value::Type::Object:
				return format_to(ctx.out(), "(Object)");
			default:
				return format_to(ctx.out(), "UNKNOWN_VALUE");
		}
	}
};
