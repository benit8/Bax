/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Fiber.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Op.hpp"
#include "Bax/Runtime/Function.hpp"
#include "Bax/Runtime/Object.hpp"
#include "Bax/Runtime/Value.hpp"
#include <array>
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax
{

class Fiber final : public Object
{
public:
	static constexpr size_t StackSize = 512;

	enum class State {
		Dead,
		Ready,
		Running,
		Yielded,
	};

	struct CallFrame {
		Closure* closure;
		std::array<Value, StackSize>::iterator stack_start;
		std::vector<Op>::const_iterator ip;
	};

public:
	explicit Fiber(Closure* closure, Fiber* caller = nullptr)
	: m_caller(caller)
	{
		CallFrame first_frame { closure, m_stack.begin(), closure->function()->bytecode().begin() };
		m_frames.push_back(std::move(first_frame));
	}

	~Fiber() override {}

	Type type() override { return Type::Fiber; }
	const char* class_name() override { return "Fiber"; }

private:
	State m_state { Ready };
	Fiber* m_caller { nullptr };
	std::array<Value, StackSize> m_stack { };
	std::vector<CallFrame> m_frames { };
};

}
