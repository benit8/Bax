/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Main entry point
*/

#include "Common/Log.hpp"
#include "Common/OptionParser.hpp"

// -----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	Log::trace("Program started!");

	OptionParser opt;
	if (!opt.parse(argc, argv))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
