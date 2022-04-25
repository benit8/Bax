/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Runner.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Value.hpp"
#include <fstream>
#include <istream>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax
{

class Runner
{
protected:
	std::vector<std::string> m_args;
	std::unordered_map<std::string, std::string> m_environment;

protected:
	Runner(const std::vector<std::string>& args = {},
		const std::unordered_map<std::string, std::string>& environment = {})
	: m_args(args)
	, m_environment(environment)
	{}

public:
	virtual ~Runner() = default;

	virtual Value interpret(const std::string& source) = 0;

	Value interpret_stream(std::istream& stream)
	{
		std::string source { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
		return interpret(source);
	}

	Value interpret_file(const std::string& filename)
	{
		std::ifstream file(filename);
		return interpret_stream(file);
	}

	const std::vector<std::string>& args() const { return m_args; }
	const std::unordered_map<std::string, std::string>& environment() const { return m_environment; }
};

}
