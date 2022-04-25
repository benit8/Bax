/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** VM / VM.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Bax/Runtime/Runner.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

class VM final : public Runner
{
public:
	VM(
		const std::vector<std::string>& args = {},
		const std::unordered_map<std::string, std::string>& environment = {}
	);

	Value interpret(const std::string& source) override;
};

}
