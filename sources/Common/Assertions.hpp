/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Common / Assertions.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "TTYEscapeSequences.hpp"
#include <cstdio>

// -----------------------------------------------------------------------------

#define ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, TTY_BRIGHT_RED("Assertion failed") ": " TTY_YELLOW(#x) "\n\tin " TTY_CYAN(__FILE__) ":" TTY_BLUE("%d") "\n", __LINE__); \
		abort(); \
	} \
} while (0)

#define ASSERT_MSG(x, fmt, ...) do { \
	if (!(x)) { \
		fprintf(stderr, TTY_BRIGHT_RED("Assertion failed") ": " TTY_YELLOW(fmt) "\n\tin " TTY_CYAN(__FILE__) ":" TTY_BLUE("%d") "\n", ##__VA_ARGS__, __LINE__); \
		abort(); \
	} \
} while (0)

#define ASSERT_NOT_REACHED() ASSERT_MSG(false, "Unreachable point reached")
#define TODO() ASSERT_MSG(false, "TODO: %s",  __PRETTY_FUNCTION__)
