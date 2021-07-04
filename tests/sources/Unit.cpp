/*
** Bax Tests, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Unit test
*/

#include "criterion/criterion.h"

// -----------------------------------------------------------------------------

Test(Unit, Test1)
{
	cr_expect_eq(1 + 1, 2);
}
