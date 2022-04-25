/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Function.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Op.hpp"
#include "Bax/Scope.hpp"
#include "Bax/Runtime/Object.hpp"
#include "Bax/Runtime/UpValue.hpp"
#include "Bax/Runtime/Value.hpp"
#include "Common/Log.hpp"
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax
{

class Chunk : public Object
{
protected:
	Chunk* m_parent { nullptr };
	std::vector<Value> m_constants { };
	std::vector<Op> m_bytecode { };

public:
	explicit Chunk(Chunk* parent = nullptr)
	: m_parent(parent)
	{}

	void emit(Op::Code op, int arg = 0)
	{
		m_bytecode.emplace_back(op, arg);
		Log::debug("{} => {}", (void*)this, *m_bytecode.rbegin());
	}

	size_t add_constant(Value&& constant)
	{
		for (size_t i = 0; i < m_constants.size(); ++i) {
			if (m_constants[i] == constant) {
				return i;
			}
		}
		m_constants.push_back(std::move(constant));
		return m_constants.size() - 1;
	}

	const std::vector<Value>& constants() const { return m_constants; }
	const std::vector<Op>& bytecode() const { return m_bytecode; }

	Type type() override { return Type::Chunk; }
	const char* class_name() override { return "Chunk"; }
};

// -----------------------------------------------------------------------------

class Function : public Chunk
{
protected:
	size_t m_arity { 0 };

public:
	explicit Function(Chunk* parent = nullptr, size_t arity = 0)
	: Chunk(parent)
	, m_arity(arity)
	{}

	size_t arity() const { return m_arity; }

	Type type() override { return Type::Function; }
	const char* class_name() override { return "Function"; }
};

// -----------------------------------------------------------------------------

class Closure final : public Function
{
private:
	std::vector<UpValue> m_up_values { };

public:
	explicit Closure(Chunk* parent = nullptr, size_t arity = 0)
	: Function(parent, arity)
	{}

	const std::vector<UpValue>& up_values() const { return m_up_values; }

	Type type() override { return Type::Closure; }
	const char* class_name() override { return "Closure"; }
};

}
