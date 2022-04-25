/*
** Bax, 2022
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
	enum class Type {
		Array,
		Chunk,
		Closure,
		Function,
		Module,
		NativeFn,
		NativePtr,
		String,
		Table,
		UpValue,
	};

	virtual ~Object() = default;

	virtual Type type() = 0;
	virtual const char* class_name() = 0;

protected:
	Object()
	{}
};

}
