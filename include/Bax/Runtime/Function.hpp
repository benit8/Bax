/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Function.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Object.hpp"
#include "Bax/Runtime/UpValue.hpp"
#include "Bax/Runtime/Value.hpp"
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax
{

class Function final : public Object
{
public:
	Function() {}
	~Function() {}

	size_t arity() const { return m_arity; }
	const std::vector<Value>& constants() const { return m_constants; }
	const std::vector<uint8_t>& bytecode() const { return m_bytecode; }

	const char* class_name() override { return "Function"; }

private:
	size_t m_arity { 0 };
	std::vector<Value> m_constants { };
	std::vector<uint8_t> m_bytecode { };
};

class Closure final : public Object
{
public:
	explicit Closure(Function* function)
	: m_function(function)
	{}

	const Function* function() const { return m_function; }
	const std::vector<UpValue>& upValues() const { return m_upValues; }

	const char* class_name() override { return "Closure"; }

private:
	Function* m_function;
	std::vector<UpValue> m_upValues { };
};

}
