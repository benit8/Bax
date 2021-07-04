/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / OptionParser.cpp
*/

#include "Assertions.hpp"
#include "fmt/core.h"
#include "OptionParser.hpp"
#include <algorithm>
#include <getopt.h>

// -----------------------------------------------------------------------------

OptionParser::OptionParser()
{
	add_option(m_show_help, 'h', "help", "Display this message");
}

// -----------------------------------------------------------------------------

bool OptionParser::parse(int argc, char** argv, bool exit_on_failure)
{
	auto print_help_and_exit = [this, argv, exit_on_failure] {
		print_help(stderr, argv[0]);
		if (exit_on_failure)
			exit(1);
	};

	opterr = 0;
	optind = 0;

	std::vector<struct option> longopt;
	std::string shortopt;
	for (auto& opt : m_options) {
		if (opt.long_name)
			longopt.push_back({opt.long_name, opt.requires_argument ? required_argument : no_argument, nullptr, 0});
		if (opt.short_name) {
			shortopt.push_back(opt.short_name);
			if (opt.requires_argument)
				shortopt.push_back(':');
		}
	}
	longopt.push_back({ NULL, 0, NULL, 0 });

	while (true) {
		int long_index = -1;
		int c = getopt_long(argc, argv, shortopt.c_str(), longopt.data(), &long_index);
		if (c == -1)
			break;
		else if (c == '?') {
			print_help_and_exit();
			return false;
		}

		auto it = m_options.begin();
		if (c == 0)
			it += long_index;
		else
			it = std::find_if(m_options.begin(), m_options.end(), [c] (auto& o) { return c == o.short_name; });
		ASSERT(it != m_options.end());

		const char* arg = it->requires_argument ? optarg : nullptr;
		if (!it->acceptor(arg)) {
			fmt::print(stderr, "Invalid value for option {}\n", argv[optind - 1]);
			print_help_and_exit();
			return false;
		}
	}

	int left_values_count = argc - optind;
	int args_values_count[m_args.size()];
	int total_required_values = 0;
	for (size_t i = 0; i < m_args.size(); i++) {
		args_values_count[i] = m_args[i].min_values;
		total_required_values += m_args[i].min_values;
	}

	if (total_required_values > left_values_count) {
		print_help_and_exit();
		return false;
	}
	int extra_values_count = left_values_count - total_required_values;

	for (size_t i = 0; i < m_args.size(); i++) {
		int arg_values_count = std::min(m_args[i].max_values - m_args[i].min_values, extra_values_count);
		args_values_count[i] += arg_values_count;
		extra_values_count -= arg_values_count;
		if (extra_values_count == 0)
			break;
	}

	if (extra_values_count > 0) {
		print_help_and_exit();
		return false;
	}

	for (size_t i = 0; i < m_args.size(); i++) {
		for (int j = 0; j < args_values_count[i]; j++) {
			const char* value = argv[optind++];
			if (!m_args[i].acceptor(value)) {
				fmt::print(stderr, "Invalid value for argument {}\n", m_args[i].name);
				print_help_and_exit();
				return false;
			}
		}
	}

	if (m_show_help) {
		print_help(stdout, argv[0]);
		exit(0);
	}

	return true;
}

void OptionParser::print_help(FILE* fp, const char* program_name)
{
	auto pretty_option_name = [] (const Option& opt)
	{
		std::string name;
		if (opt.short_name) {
			name.push_back('-');
			name.push_back(opt.short_name);
			if (opt.long_name)
				name.append(", --").append(opt.long_name);
		}
		else {
			ASSERT(opt.long_name != nullptr);
			name.append("    --").append(opt.long_name);
		}
		if (opt.value_name) {
			name.push_back('=');
			name.append(opt.value_name);
		}
		return name;
	};

	fmt::print(fp, "Usage:\n\t{}", program_name);
	for (auto& arg : m_args) {
		if (arg.is_required() && arg.is_repeated())
			fmt::print(fp, " <{}...>", arg.name);
		else if (arg.is_required() && !arg.is_repeated())
			fmt::print(fp, " <{}>", arg.name);
		else if (!arg.is_required() && arg.is_repeated())
			fmt::print(fp, " [{}...]", arg.name);
		else if (!arg.is_required() && !arg.is_repeated())
			fmt::print(fp, " [{}]", arg.name);
	}
	fmt::print(fp, "\n");

	if (m_args.size()) {
		fmt::print(fp, "\nArguments:");
		for (auto& arg : m_args)
			fmt::print(fp, "\n\t{:30} {}", arg.name, arg.help);
		fmt::print(fp, "\n");
	}

	if (m_options.size()) {
		fmt::print(fp, "\nOptions:");
		for (auto& opt : m_options)
			fmt::print(fp, "\n\t{:30} {}", pretty_option_name(opt), opt.help);
		fmt::print(fp, "\n");
	}
}

// -----------------------------------------------------------------------------

void OptionParser::add_option(Option&& option)
{
	m_options.push_back(std::move(option));
}

void OptionParser::add_option(bool& value, char short_name, const char* long_name, const char* help)
{
	m_options.push_back({
		false,
		help,
		long_name,
		short_name,
		nullptr,
		[&value] (const char* s) {
			ASSERT(s == nullptr);
			value = true;
			return true;
		}
	});
}

void OptionParser::add_option(int& value, char short_name, const char* long_name, const char* help, const char* value_name)
{
	m_options.push_back({
		true,
		help,
		long_name,
		short_name,
		value_name,
		[&value](const char* s) {
			value = atoi(s);
			return true;
		}
	});
}

void OptionParser::add_option(std::string& value, char short_name, const char* long_name, const char* help, const char* value_name)
{
	m_options.push_back({
		true,
		help,
		long_name,
		short_name,
		value_name,
		[&value] (const char* s) {
			value = s;
			return true;
		}
	});
}

// -----------------------------------------------------------------------------

void OptionParser::add_argument(Argument&& arg)
{
	m_args.push_back(std::move(arg));
}

void OptionParser::add_argument(int& value, const char* name, const char* help, bool required)
{
	m_args.push_back({
		help,
		name,
		required ? 1 : 0,
		1,
		[&value] (const char* s) {
			value = atoi(s);
			return true;
		}
	});
}

void OptionParser::add_argument(std::string& value, const char* name, const char* help, bool required)
{
	m_args.push_back({
		help,
		name,
		required ? 1 : 0,
		1,
		[&value] (const char* s) {
			value = s;
			return true;
		}
	});
}

void OptionParser::add_argument(std::vector<std::string>& values, const char* name, const char* help, bool required)
{
	m_args.push_back({
		help,
		name,
		required ? 1 : 0,
		__INT_MAX__,
		[&values] (const char* s) {
			values.push_back(s);
			return true;
		}
	});
}
