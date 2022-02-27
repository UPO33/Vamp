#pragma once

// #include "imgui.h"

#include <math.h>
#include <iosfwd>
#include <stdint.h>
#include <stddef.h>
#include "VampArchive.h"

struct VVec2F;
struct VVec2I;

struct VVec2F
{
	float X, Y;

	
	VVec2F(): X(0), Y(0){}
	explicit VVec2F(float xy) : X(xy), Y(xy) {}
	explicit VVec2F(float x, float y) : X(x), Y(y) {}
	inline explicit VVec2F(const VVec2I&);
	/*ImVec2 ToImgui() const { return ImVec2(X, Y); }*/


	VVec2F operator * (float v) const { return VVec2F(X * v, Y * v); }
	VVec2F operator / (float v) const { return VVec2F(X / v, Y / v); }
	VVec2F operator + (float v) const { return VVec2F(X + v, Y + v); }
	VVec2F operator - (float v) const { return VVec2F(X - v, Y - v); }

	VVec2F operator + (const VVec2F& other) const { return VVec2F(X + other.X, Y + other.Y); }
	VVec2F operator - (const VVec2F& other) const { return VVec2F(X - other.X, Y - other.Y); }
	VVec2F operator * (const VVec2F& other) const { return VVec2F(X * other.X, Y * other.Y); }
	VVec2F operator / (const VVec2F& other) const { return VVec2F(X / other.X, Y / other.Y); }

	//VVec2F& operator << (std::ofstream&) const;
	//VVec2F& operator >> (std::ifstream&);

	VVec2F& operator += (const VVec2F& other) { X += other.X; Y += other.Y; return *this; }
	VVec2F& operator -= (const VVec2F& other) { X -= other.X; Y -= other.Y; return *this; }
	VVec2F& operator *= (const VVec2F& other) { X *= other.X; Y *= other.Y; return *this; }
	VVec2F& operator /= (const VVec2F& other) { X /= other.X; Y /= other.Y; return *this; }

	VVec2F& operator += (float other) { X += other; Y += other; return *this; }
	VVec2F& operator -= (float other) { X -= other; Y -= other; return *this; }
	VVec2F& operator *= (float other) { X *= other; Y *= other; return *this; }
	VVec2F& operator /= (float other) { X /= other; Y /= other; return *this; }

	VVec2F GetRotated(float angleRadian) const;
	VVec2F GetRotated(float cosValue, float sinValue) const;

	void Rotate(float angleRadian);
	void Rotate(float cosValue, float sinValue);
	void RotateAround(float angleRadian, const VVec2F& origin);
};

std::ofstream& operator << (std::ofstream& out, const VVec2F& v);
std::ifstream& operator >> (std::ifstream& in, VVec2F& v);

VOutStream& operator << (VOutStream& out, const VVec2F& v);
VInStream& operator >> (VInStream& in, VVec2F& v);


struct VVec2I
{
	int X, Y;

	VVec2I() : X(0), Y(0) {}
	explicit VVec2I(int xy) : X(xy), Y(xy) {}
	explicit VVec2I(int x, int y) : X(x), Y(y) {}

// 	ImVec2 ToImgui() const { return ImVec2((float)X, (float)Y); }

	
	VVec2I operator * (int v) const { return VVec2I(X * v, Y * v); }
	VVec2I operator / (int v) const { return VVec2I(X / v, Y / v); }
	VVec2I operator + (int v) const { return VVec2I(X + v, Y + v); }
	VVec2I operator - (int v) const { return VVec2I(X - v, Y - v); }

	VVec2I operator + (const VVec2I& other) const { return VVec2I(X + other.X, Y + other.Y); }
	VVec2I operator - (const VVec2I& other) const { return VVec2I(X - other.X, Y - other.Y); }
	VVec2I operator * (const VVec2I& other) const { return VVec2I(X * other.X, Y * other.Y); }
	VVec2I operator / (const VVec2I& other) const { return VVec2I(X / other.X, Y / other.Y); }

};

std::ofstream& operator << (std::ofstream& out, const VVec2I& v);
std::ifstream& operator >> (std::ifstream& in, VVec2I& v);

VOutStream& operator << (VOutStream& out, const VVec2I& v);
VInStream& operator >> (VInStream& in, VVec2I& v);


VVec2F::VVec2F(const VVec2I& vi) : X((float)vi.X), Y((float)vi.Y)
{
}


struct VColor32
{
	union 
	{
		uint8_t RGBA[4];
		uint32_t mColor;
	};

	
	
};