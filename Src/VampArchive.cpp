#include "VampArchive.h"
#include <fstream>

void VInStream::Read(void* ptr, size_t size)
{
	mStream.read((char*)ptr, size);
}

void VOutStream::Write(const void* ptr, size_t size)
{
	mStream.write((const char*)ptr, size);
}
