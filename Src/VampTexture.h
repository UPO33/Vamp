#pragma once

#include "VampGFX.h"
#include "VampMath.h"
#include "VampContainer.h"
#include <iosfwd>

static const int VMAX_NAME_LEN = 64;

struct VTexture2D
{
	const char* mName = nullptr;
	ComPtr<ID3D11Texture2D> mTextureHandle;
	ComPtr<ID3D11ShaderResourceView> mDefaultView;
	VVec2I mSize = VVec2I(0);
	int mTextureIndex = 0;

	int GetTextureIndex() const { return mTextureIndex; }

	static VTexture2D* LoadFromFile(const char* folder, const char* name);

	static void SerializeHandle(VTexture2D*& ptr, VInStream& stream);
	static void SerializeHandle(VTexture2D*& ptr, VOutStream& stream);
	static VTexture2D* FindByName(const char* name);
	static VTexture2D* FindByIndex(int index);
	static int GetTextureIndex(const VTexture2D* ptr);

	static VArray<VTexture2D*> AllTextures;

};