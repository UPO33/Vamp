#pragma once
#include <iosfwd>

struct VInStream
{
	std::ifstream& mStream;

	VInStream(std::ifstream& in) : mStream(in) {}

	void Read(void* ptr, size_t size);
};

struct VOutStream
{
	std::ofstream& mStream;

	VOutStream(std::ofstream& in) : mStream(in) {}

	void Write(const void* ptr, size_t size);
};

template<typename T> VOutStream& operator << (VOutStream& stream, const T& value)
{
	stream.Write(&value, sizeof(T));
	return stream;
}
template<typename T> VInStream& operator >> (VInStream& stream, T& value)
{
	stream.Read(&value, sizeof(T));
	return stream;
}