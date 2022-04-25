/*
** Bax, 2021
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Object.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

namespace Bax
{

class Object
{
public:
	virtual ~Object() = default;

	virtual const char* class_name() = 0;

protected:
	Object()
	{}
};

}
