/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / OptionParser.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include <functional>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

class OptionParser
{
public:
	struct Option
	{
		bool requires_argument = true;
		const char* help = nullptr;
		const char* long_name = nullptr;
		char short_name = 0;
		const char* value_name = nullptr;
		std::function<bool(const char*)> acceptor;
	};

	struct Argument
	{
		const char* help = nullptr;
		const char* name = nullptr;
		int min_values = 0;
		int max_values = 1;
		std::function<bool(const char*)> acceptor;

		constexpr bool is_required() const { return min_values > 0; }
		constexpr bool is_repeated() const { return max_values > 1; }
	};

public:
	OptionParser();

	bool parse(int argc, char** argv, bool exit_on_failure = true);
	void print_help(FILE*, const char* program_name);

	void add_option(Option&&);
	void add_option(bool&        value, char short_name, const char* long_name, const char* help);
	void add_option(int&         value, char short_name, const char* long_name, const char* help, const char* value_name);
	void add_option(std::string& value, char short_name, const char* long_name, const char* help, const char* value_name);

	void add_argument(Argument&&);
	void add_argument(int&                      value, const char* name, const char* help, bool required = true);
	void add_argument(std::string&              value, const char* name, const char* help, bool required = true);
	void add_argument(std::vector<std::string>& value, const char* name, const char* help, bool required = true);

private:
	std::vector<Option> m_options;
	std::vector<Argument> m_args;

	bool m_show_help = false;
};
