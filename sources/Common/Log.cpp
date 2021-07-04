/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / Log.cpp
*/

#include "Common/Assertions.hpp"
#include "Common/Log.hpp"
#include "fmt/chrono.h"
#include "fmt/color.h"
#include "fmt/format.h"
#include <sys/time.h>

// -----------------------------------------------------------------------------

Log::Level Log::s_level = Log::Level::Trace;

// -----------------------------------------------------------------------------

void Log::log(Level level, fmt::string_view format, fmt::format_args args)
{
	static const struct {
		const char* name;
		fmt::text_style style;
	} levels[] = {
		{ "TRACE",     fmt::emphasis::faint },
		{ "DEBUG",     fmt::fg(fmt::terminal_color::magenta) },
		{ "INFO",      fmt::fg(fmt::terminal_color::blue) },
		{ "NOTICE",    fmt::fg(fmt::terminal_color::green) },
		{ "WARNING",   fmt::fg(fmt::terminal_color::yellow) },
		{ "ERROR",     fmt::fg(fmt::terminal_color::red) },
		{ "CRITICAL",  fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red) },
		{ "ALERT",     fmt::emphasis::bold | fmt::emphasis::underline | fmt::fg(fmt::terminal_color::red) },
		{ "EMERGENCY", fmt::emphasis::bold | fmt::emphasis::underline | fmt::emphasis::blink | fmt::fg(fmt::terminal_color::red) },
	};

	if (level < s_level)
		return;

	std::time_t t = std::time(nullptr);
	fmt::print("{} {} {}\n",
		fmt::format(fmt::emphasis::faint, "{:%Y-%m-%d %H:%M:%S}", fmt::localtime(t)),
		fmt::format(levels[level].style, "{}", levels[level].name),
		fmt::vformat(format, args)
	);
}
