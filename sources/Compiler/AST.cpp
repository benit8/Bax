/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** AST.cpp
*/

#include "Bax/Compiler/AST.hpp"

// -----------------------------------------------------------------------------

namespace Bax::AST
{

void FunctionExpression::dump(int i) const
{
	Expression::dump(i);
	for (auto &param : parameters)
		param->dump(i + 1);
	body->dump(i + 1);
}

}
