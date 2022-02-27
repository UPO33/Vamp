#pragma once

#include "VampTexture.h"
#include "VampMath.h"
#include "VampContainer.h"
#include "VampArchive.h"



struct VSprite
{
	static const int MAX_VERTEX = 16;

	struct Vertex
	{
		VVec2F mPosition;
		VVec2F mUV;
	};

	//
	VTexture2D* mTexture = nullptr;
	//size in UV space
	VVec2F mSize = VVec2F(1);
	//offset in UV space
	VVec2F mOffset = VVec2F(0);
	//UV of quad, is generated from mSize and mOffset
	VVec2F mQuadUV[4];
	//must be between 0, 1
	VVec2F mOrigin = VVec2F(0.5f);
	//extra scale used when drawing this sprite
	VVec2F mDrawScale = VVec2F(1);
	//
	char mName[VMAX_NAME_LEN] = {0};
	//
	int mSpriteIndex = 0;

	VVec2F GetSizeInPixel() const { return mSize * VVec2F(mTexture->mSize); }
	VVec2F GetOffsetInPixel() const { return mOffset * VVec2F(mTexture->mSize); }

	VVec2F GetUV0() const { return mOffset; }
	VVec2F GetUV1() const { return mOffset + mSize; }

	void Serialize(VInStream& stream);
	void Serialize(VOutStream& stream);


	void OnPropertyChanged();
	
	static void SerializeHandle(VSprite*& ptr, VInStream& stream);
	static void SerializeHandle(VSprite*& ptr, VOutStream& stream);
	static VSprite* FindByName(const char* name);
	static VSprite* FindByIndex(int index);
	static int GetSpriteIndex(const VSprite* ptr);
	static VArray<VSprite*> AllSprites;

};

struct VAtlas
{
	static const int MAX_SPRITE = 1024;

	VArray<VSprite*> mSprites;

	VSprite* AddSprite();
	VSprite* GetSpriteByIndex(int index) const;
	int GetSpriteIndex(const VSprite* sprite) const;
	int NumSprite() const { return mSprites.mLength; }

	void Serialize(VInStream& stream);
	void Serialize(VOutStream& stream);
};

void Helper_MakeQuadTriStrip(VSprite::Vertex* out, VVec2F position, VVec2F size);