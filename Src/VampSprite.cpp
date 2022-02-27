#include "VampSprite.h"
#include <fstream>
#include "VampEngine.h"
#include "VampTexture.h"

VSprite* VAtlas::AddSprite()
{
	VSprite* pNewSprite = new VSprite();
	sprintf_s(pNewSprite->mName, "sprite_%d", mSprites.Length());
	mSprites.AddEmplace(pNewSprite);
	pNewSprite->mSpriteIndex = VSprite::AllSprites.Length();
	VSprite::AllSprites.AddEmplace(pNewSprite);
	return pNewSprite;
}

VSprite* VAtlas::GetSpriteByIndex(int index) const
{
	if(!mSprites.IsIndexValid(index))
		return nullptr;

	return mSprites[index];
}

int VAtlas::GetSpriteIndex(const VSprite* sprite) const
{
	return mSprites.IndexOf((VSprite*)sprite);
}

void VAtlas::Serialize(VOutStream& stream)
{
	stream << mSprites.Length();
	for (int i = 0; i < mSprites.Length(); i++)
	{
		assert(mSprites[i]);
		mSprites[i]->Serialize(stream);
	}
}

void VAtlas::Serialize(VInStream& stream)
{
	assert(mSprites.mLength == 0);
	
	int len = 0;
	stream >> len;

	for (int i = 0; i < len; i++)
	{
		VSprite* pSprite = AddSprite();
		pSprite->Serialize(stream);
	}
}

void VSprite::Serialize(VInStream& stream)
{
	int nameLen = 0;
	stream >> nameLen;
	stream.Read(mName, nameLen);
	mName[nameLen] = 0;

	stream >> mSize >> mOffset;

	VTexture2D::SerializeHandle(mTexture, stream);

	OnPropertyChanged();
}

void VSprite::Serialize(VOutStream& stream)
{
	int nameLen = strlen(mName);
	stream << nameLen;
	stream.Write(mName, nameLen);

	stream << mSize << mOffset;

	VTexture2D::SerializeHandle(mTexture, stream);
}

void VSprite::OnPropertyChanged()
{
	mQuadUV[0] = mOffset;
	mQuadUV[1] = VVec2F(mOffset.X + mSize.X, mOffset.Y);
	mQuadUV[2] = VVec2F(mOffset.X, mOffset.Y + mSize.Y);
	mQuadUV[3] = mOffset + mSize;
}



void VSprite::SerializeHandle(VSprite*& ptr, VOutStream& stream)
{
	if (ptr)
	{
		int nameLen = (int)strlen(ptr->mName);
		stream << nameLen;
		stream.Write(ptr->mName, nameLen);
	}
	else
	{
		stream << int(0);
	}

}

void VSprite::SerializeHandle(VSprite*& ptr, VInStream& stream)
{
	int nameLen = 0;
	stream >> nameLen;

	char nameStack[VMAX_NAME_LEN];
	stream.Read(nameStack, nameLen);
	nameStack[nameLen] = 0;

	ptr = VSprite::FindByName(nameStack);
}

VSprite* VSprite::FindByName(const char* name)
{
	for (int i = 0; i < VSprite::AllSprites.Length(); i++)
	{
		 VSprite* pSprite = VSprite::AllSprites[i];
		 if(pSprite && strcmp(pSprite->mName, name) == 0)
			return pSprite;
	}

	return nullptr;
}

VSprite* VSprite::FindByIndex(int index)
{
	if(AllSprites.IsIndexValid(index))
		return AllSprites[index];

	return nullptr;
}

int VSprite::GetSpriteIndex(const VSprite* ptr)
{
	return ptr ? ptr->mSpriteIndex : 0;
}

VArray<VSprite*> VSprite::AllSprites;

void Helper_MakeQuadTriStrip(VSprite::Vertex* out, VVec2F position, VVec2F size)
{
	out[0] = { position, VVec2F(0,0) };
	out[1] = { VVec2F(position.X + size.X, position.Y), VVec2F(1,0) };
	out[2] = { VVec2F(position.X, position.Y + size.Y), VVec2F(0,1) };
	out[3] = { position + size, VVec2F(1,1) };
}
