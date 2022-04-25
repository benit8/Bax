/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Runner.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/Module.hpp"
#include "Bax/Runtime/Value.hpp"
#include "Common/Log.hpp"
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
	std::unordered_map<std::string, Module> m_modules;

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
		if (!file.is_open()) {
			Log::critical("Failed to open file '{}'", filename);
			return Value();
		}
		return interpret_stream(file);
	}

	const std::vector<std::string>& args() const { return m_args; }
	const std::unordered_map<std::string, std::string>& environment() const { return m_environment; }
	const Module& module(const std::string& name) const { return m_modules.at(name); }
};

}
