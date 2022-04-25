/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Fiber.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Object.hpp"
#include "Bax/Runtime/Value.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

class UpValue final : public Object
{
public:
	UpValue(Value* value)
	: m_value(value)
	{}

	const Value* value() const { return m_value; }

	Type type() override { return Type::UpValue; }
	const char* class_name() override { return "UpValue"; }

private:
	Value* m_value;
	Value m_closed;
};

}
