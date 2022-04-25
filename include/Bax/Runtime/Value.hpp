/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Value.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Object.hpp"
#include "Common/Log.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

struct Value
{
	enum class Type {
		Null,
		False,
		True,
		Glyph,
		Number,
		Object,
	} type { Type::Null };

	union {
		bool boolean;
		uint64_t integer;
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

	explicit Value(uint64_t i)
	: type(Value::Type::Glyph)
	, as(i)
	{}

	explicit Value(double n)
	: type(Value::Type::Number)
	, as(n)
	{}

	explicit Value(Object *o)
	: type(Value::Type::Object)
	{
		as.object = o;
	}

	bool operator ==(const Value& other) const
	{
		if (type != other.type) {
			return false;
		}

		switch (type) {
			case Type::Glyph:
				return as.integer == other.as.integer;
			case Type::Number:
				return as.number == other.as.number;
			// Fixme: make this better
			case Type::Object:
				return as.object == other.as.object;
			default:
				return false;
		}
	}
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
				return format_to(ctx.out(), "Null");
			case Bax::Value::Type::False:
				return format_to(ctx.out(), "Boolean(false)");
			case Bax::Value::Type::True:
				return format_to(ctx.out(), "Boolean(true)");
			case Bax::Value::Type::Glyph:
				return format_to(ctx.out(), "Glyph({})", v.as.integer);
			case Bax::Value::Type::Number:
				return format_to(ctx.out(), "Number({})", v.as.number);
			case Bax::Value::Type::Object:
				return format_to(ctx.out(), "Object({})", (void*)v.as.object);
			default:
				return format_to(ctx.out(), "UNKNOWN_VALUE");
		}
	}
};
