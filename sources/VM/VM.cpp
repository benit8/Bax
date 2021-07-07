/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** VM.cpp
*/

#include "VM/VM.hpp"
#include "Common/Log.hpp"
#include <cstring>

// -----------------------------------------------------------------------------

namespace Bax
{

VM::VM()
{}

VM::VM(char** environment)
: VM()
{
	for (auto it = environment; *it != nullptr; ++it) {
		auto e = *it;
		auto p = strchr(e, '=');
		m_environment.emplace(std::string(e, p), std::string(p + 1));
	}
}

VM::~VM()
{}

}
