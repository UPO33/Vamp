#include "VampMath.h"


#include <fstream>

//const VVec2F& VVec2F::operator<<(std::ofstream& stream) const
//{
//	stream << X << Y;
//	return *this;
//}
//
//VVec2F& VVec2F::operator>>(std::ifstream& stream) 
//{
//	stream >> X >> Y;
//	return *this;
//}

std::ifstream& operator>>(std::ifstream& in, VVec2F& v)
{
	in >> v.X >> v.Y;
	return in;
}

std::ofstream& operator<<(std::ofstream& out, const VVec2F& v)
{
	out << v.X << v.Y;
	return out;
}

VInStream& operator>>(VInStream& in, VVec2F& v)
{
	in >> v.X >> v.Y;
	return in;
}

VOutStream& operator<<(VOutStream& out, const VVec2F& v)
{
	out << v.X << v.Y;
	return out;
}



std::ifstream& operator>>(std::ifstream& in, VVec2I& v)
{
	in >> v.X >> v.Y;
	return in;
}

std::ofstream& operator<<(std::ofstream& out, const VVec2I& v)
{
	out << v.X << v.Y;
	return out;
}


VInStream& operator>>(VInStream& in, VVec2I& v)
{
	in >> v.X >> v.Y;
	return in;
}

VOutStream& operator<<(VOutStream& out, const VVec2I& v)
{
	out << v.X << v.Y;
	return out;
}

VVec2F VVec2F::GetRotated(float cosValue, float sinValue) const
{
	float newX = X * cosValue - Y * sinValue;
	float newY = X * sinValue + Y * cosValue;
	return VVec2F(newX, newY);
}

VVec2F VVec2F::GetRotated(float angleRadian) const
{
	float cosValue = cos(angleRadian);
	float sinValue = sin(angleRadian);
	return GetRotated(cosValue, sinValue);
}

void VVec2F::Rotate(float angleRadian)
{
	float cosValue = cos(angleRadian);
	float sinValue = sin(angleRadian);
	return Rotate(cosValue, sinValue);
}

void VVec2F::Rotate(float cosValue, float sinValue)
{
	float newX = X * cosValue - Y * sinValue;
	float newY = X * sinValue + Y * cosValue;
	X = newX;	Y = newY;
}

void VVec2F::RotateAround(float angleRadian, const VVec2F& origin)
{
	*this -= origin;
	Rotate(angleRadian);
	*this += origin;
}
