#include <iostream>
#include "vector.h"
#include "color.h"
#include "geometry.h"
#include "str.h"

std::ostream& operator<<(std::ostream& os, Rc::Vector4 const& v)
{
	return os << "xyzw(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
}

std::ostream& operator<<(std::ostream& os, Rc::Color const& c)
{
	return os << "rgba(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ')';
}

std::ostream& operator<<(std::ostream& os, Rc::RGBA8 const& c)
{
	return os << "rgba8(" << int{c.r} << ", " << int{c.g} << ", " << int{c.b} << ", " << int{c.a} << ')';
}

std::ostream& operator<<(std::ostream& os, Rc::Point const& p)
{
	return os << '[' << p.x << ", " << p.y << ']';
}