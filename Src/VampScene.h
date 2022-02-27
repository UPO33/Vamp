#pragma once

#include "VampSprite.h"
#include <unordered_map>
#include <memory>

#include "chipmunk/chipmunk.h"

struct VSpriteBuilder;
struct VSpriteBuilder;

struct VEntityHandle
{
	uint32_t mIndex = 0;
	uint32_t mHash = 0;
};

template<typename T> struct VEntityLine
{
	static_assert(sizeof(T) >= sizeof(uint32_t));

	VArray<T> mItems;
	VArray<uint32_t> mHashArray;

	typedef VEntityHandle Handle;
	
	int mLastFreeIndex = -1;
	uint32_t mHashCounter = 0;
	bool bDebugDestroying = false;

	int Length() const { return mItems.Length(); }

	T& Add()
	{
		if (mLastFreeIndex != -1)
		{
			int lastFree = mLastFreeIndex;
			int* pNextFree = (int*)(mItems.Elements() + lastFree);
			mLastFreeIndex = *pNextFree;

			mHashArray[lastFree] = GenHash();
			T* pNewItem = new (mItems.Elements() + lastFree) T();
			return *pNewItem;
		}
		else
		{
			mItems.AddDefault();
			mHashArray.AddEmplace(GenHash());
			return mItems.Last();
		}
	}
	uint32_t GenHash()
	{
		mHashCounter++;
		if(mHashCounter == 0)
			mHashCounter++;
		
		return mHashCounter;
	}
	void Remove(T& item)
	{
		int index = mItems.IndexOfAddress(&item);
		if (index != -1)
		{
			Remove_Internal(index);
		}
	}
	void Remove(Handle handle)
	{
		if(IsHandleValid(handle))
			Remove_Internal(handle.mIndex);
	}
	void Remove_Internal(int index)
	{
		assert(!bDebugDestroying);
		bDebugDestroying = true;

		const int savedLastFreeIndex = mLastFreeIndex;

		mHashArray[index] = 0;
		mLastFreeIndex = index;
		mItems[index].~T();

		if (savedLastFreeIndex != -1)
		{
			int* pNextFree = (int*)(mItems.Elements() + index);
			*pNextFree = savedLastFreeIndex;
		}

		bDebugDestroying = false;
	}
	bool IsHandleValid(Handle handle) const
	{
		return ResolveHandle(handle) != nullptr;
	}
	T* ResolveHandle(Handle handle) const
	{
		if (handle.mHash != 0 && mHashArray.IsIndexValid(handle.mIndex) && mHashArray[handle.mIndex] == handle.mHash)
			return mItems.Elements() + handle.mIndex;

		return nullptr;
	}
	//0 means slot is empty
	uint32_t GetHashAt(int index) const { return mHashArray[index]; }
	//just get the element, no checking
	T& GetItemAt(int index) { return mItems[index]; }
};

struct VEntSprite
{
	VSprite* mSprite = nullptr;
	VVec2F mLocation = VVec2F(0);
	float mRotation = 0;
	int mGridIndex = 0;
	int mDepth = 0;
	bool mHiddenInGame = false;
};



struct VCamera
{
	VVec2F mPosition;
	float  mDepth;
	float  mRotation;
};

struct VScene
{
	VCamera mCamera;

	VEntityLine<VEntSprite> mSprites;

	std::unique_ptr<VSpriteBuilder> mLastBuilder;

	VScene();
	
	VEntSprite& AddSprite();

	void MakeSpriteBatch();
	void Render();
	void Tick(float delta);


	#pragma region physx
	cpSpace* mPhysSpace = nullptr;
	void InitPhysx();
	void DestroyPhysx();
	void TickPhys();

	#pragma endregion physx
};