/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** VM / VM.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include <unordered_map>
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{

class VM
{
public:
	VM();
	VM(char** environment);
	~VM();

	const std::unordered_map<std::string, std::string>& environment() const { return m_environment; }

private:
	std::unordered_map<std::string, std::string> m_environment;
};

}
