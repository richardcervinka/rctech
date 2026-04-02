#pragma once

namespace Rc
{

	template<typename T>
	struct Rectangle
	{
		T x {};
		T y {};
		T w {};
		T h {};
	};

	template<typename T>
	struct Point
	{
		T x {};
		T y {};
	};

} // Rc