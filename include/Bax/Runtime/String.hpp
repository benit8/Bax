/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / String.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Object.hpp"
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{

class String final : public Object
{
public:
	explicit String(const Token& t)
	: m_string(t.trivia.data(), t.trivia.length())
	{}

	explicit String(std::string s)
	: m_string(std::move(s))
	{}

	~String() override {}

	const std::string& string() const { return m_string; }

	Type type() override { return Type::String; }
	const char* class_name() override { return "String"; }

private:
	std::string m_string;
};

}

template <>
struct fmt::formatter<Bax::String> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}
	template <typename FormatContext>
	auto format(const Bax::String& s, FormatContext& ctx) {
		return format_to(ctx.out(), "String({})", s.string());
	}
};
