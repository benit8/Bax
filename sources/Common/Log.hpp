/*
** Bax, 2022
** Benoît Lormeau <blormeau@outlook.com>
** Common / Log.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "fmt/format.h"
#include <iostream>

// -----------------------------------------------------------------------------

class Log
{
public:
	enum Level
	{
		Trace,
		Debug,
		Info,
		Notice,
		Warning,
		Error,
		Critical,
		Alert,
		Emergency,
	};

public:
	template <typename S, typename... Args> static void     trace(const S& f, Args&&... args) { return log(Level::Trace,     f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void     debug(const S& f, Args&&... args) { return log(Level::Debug,     f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void      info(const S& f, Args&&... args) { return log(Level::Info,      f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void    notice(const S& f, Args&&... args) { return log(Level::Notice,    f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void   warning(const S& f, Args&&... args) { return log(Level::Warning,   f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void     error(const S& f, Args&&... args) { return log(Level::Error,     f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void  critical(const S& f, Args&&... args) { return log(Level::Critical,  f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void     alert(const S& f, Args&&... args) { return log(Level::Alert,     f, fmt::make_args_checked<Args...>(f, args...)); }
	template <typename S, typename... Args> static void emergency(const S& f, Args&&... args) { return log(Level::Emergency, f, fmt::make_args_checked<Args...>(f, args...)); }

	static Level level() { return s_level; }
	static void set_level(Level level) { s_level = level; }

private:
	static void log(Level level, fmt::string_view format, fmt::format_args args);

private:
	static Level s_level;
};
