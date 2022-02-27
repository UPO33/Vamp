#pragma once

#include <algorithm>

#ifdef min
#undef  min
#endif

#ifdef max
#undef  max
#endif

// template<typename T, int MaxLen> struct VStackArray
// {
// 	T mElements[MaxLen];
// 	int mLength = 0;
// 
// 	int Add(const T& clone)
// 	{
// 		mElements[mLength] = clone;
// 		return mLength++;
// 	}
// 	int AddDefault()
// 	{
// 		return mLength++;
// 	}
// 	bool IsIndexValid(int index) const
// 	{
// 		 return index >= 0 && index < mLength;
// 	}
// 	void Reset()
// 	{
// 		mLength = 0;
// 	}
// 
// 	T& operator[](int index) { return mElements[index]; }
// 	const T& operator[](int index) const { return mElements[index]; }
// 
// };
// 
// 
// template<typename T> struct VArray
// {
// 	T* mElements;
// 	int mLength = 0;
// 	int mCapacity = 0;
// };



namespace JArrayUtil
{
	template<typename T> void DestroyElements(T* elements, int len)
	{
		for (int i = 0; i < len; i++)
			(elements + i)->~T();
	}
	/*
	*/
	template<typename TSrc, typename TDst> void ConstructElements(TDst* dst, const TSrc* src, size_t count)
	{
		//#Note currently we use copy constructor. we need some optimization to use memcpy for arithmetic, pod types .. (those who are memcpy'able)

		for (size_t i = 0; i < count; i++)
			new (dst + i) TDst(src[i]);
	}

	template<typename T> void RemoveShift(T* elements, int& length, int index, int count)
	{
		JArrayUtil::DestroyElements(elements + index, count);
		memmove(elements + index, elements + (index + count), (length - index - count) * sizeof(T));
		length -= count;
	}
	template<typename T> void RemoveAtSwap(T* elements, int& length, int index)
	{
		if (length - 1 == index)
		{
			JArrayUtil::DestroyElements(elements + length - 1, 1);
		}
		else
		{
			elements[length - 1] = std::move(elements[index]);
		}
		length--;
	}

	/*
	*/
	template<typename T> int IndexOfAddr(const T* elements, int length, const void* pAddress)
	{
		auto diff = ((const char*)pAddress) - ((const char*)elements);
		auto index = diff / sizeof(T);
		if ((diff % sizeof(T) == 0) && (index >= 0 && index < length))
			return static_cast<int>(index);

		return -1;
	}
	template<typename T> int IndexOfAddrUnsafe(const T* elements, int length, const void* pAddress)
	{
		return static_cast<int>(elements - static_cast<const T*>(pAddress));
	}
	/*
	*/
	template<typename T> bool IsAddresValid(const T* elements, int length, const void* pAddress)
	{
		return IndexOfAddr(elements, length, pAddress) != -1;
	}
	template<typename T> void Reverse(T* elements, int length)
	{
		for (int i = 0; i < length / 2; i++)
			std::swap(elements[i], elements[length - i - 1]);
	}
};
















/*
*/
template<typename TElement> struct VArray
{
	using ElementT = TElement;

	TElement* mElements;
	TElement* mMemory;
	int mLength;
	int mCapacity;
	

	//creates an empty array
	VArray() : mLength(0), mCapacity(0), mElements(nullptr), mMemory(nullptr)
	{
	}
	//copy constructor
	VArray(const VArray& other) : VArray(other.mElements, other.mLength)
	{
	}
	/*
	*/
	VArray(std::initializer_list<TElement> list) : VArray(list.begin(), list.size())
	{
	}
	/*
	*/
	VArray(const TElement* elements, size_t count) : VArray()
	{
		if (elements && count > 0)
		{
			AddUnInit(count);
			JArrayUtil::ConstructElements(Elements(), elements, count);
		}
	}
	/*
	*/
	VArray(VArray&& moveFrom)
	{
		mLength = moveFrom.mLength;
		mCapacity = moveFrom.mCapacity;
		mElements = moveFrom.mElements;
		mMemory = moveFrom.mMemory;

		moveFrom.mLength = 0;
		moveFrom.mCapacity = 0;
		moveFrom.mElements = nullptr;
		moveFrom.mMemory = nullptr;
	}
	/*
	copy assignment
	*/
	VArray& operator = (const VArray& other)
	{
		if (this != &other)
		{
			Reset();
			AddUnInit(other.Length());
			JArrayUtil::ConstructElements(Elements(), other.Elements(), other.mLength);
		}
		return *this;
	}
	/*
	move assignment
	*/
	VArray& operator = (VArray&& other)
	{
		if (this != &other)
		{
			this->~VArray();
			new (this) VArray(std::forward<VArray>(other));
		}
		return *this;
	}
	/*
	*/
	int Append(const TElement* elements, size_t count)
	{
		if (elements == nullptr || count == 0)
			return -1;

		int index = AddUnInit(count);
		JArrayUtil::ConstructElements(Elements() + index, elements, count);
		return index;
	}
	/*
	*/
	int Append(std::initializer_list<TElement> list)
	{
		return Append(list.begin(), list.size());
	}
	/*
	*/
	int Append(const VArray& other)
	{
		return Append(other.Elements(), other.Length());
	}
	/*
	return the current length of array. number of available elements.
	*/
	int Length() const { return mLength; }
	/*
	returns the current capacity of array. maximum number of elements
	possible to have without any allocation.
	*/
	int Capacity() const { return mCapacity; }
	/*
	returns pointer to the first element. head of array
	*/
	TElement* Elements() { return mElements; }
	/*
	returns const pointer to the first element. head of array
	*/
	const TElement* Elements() const { return mElements; }

	/*
	returns pointer to element at the specified index if index is valid
	*/
	TElement* GetElementAt(int index)
	{
		return IsIndexValid(index) ? Elements() + index : nullptr;
	}
	/*
	returns a const pointer to element at the specified index if index is valid
	*/
	const TElement* GetElementAt(int index) const
	{
		return IsIndexValid(index) ? Elements() + index : nullptr;
	}
	/*
	*/
	TElement& operator[] (int index)
	{
		assert(IsIndexValid(index));
		return mElements[index];
	}
	/*
	*/
	const TElement& operator [] (int index) const
	{
		assert(IsIndexValid(index));
		return mElements[index];
	}
	/*
	*/
	TElement& Last(int indexFromEnd = 0)
	{
		assert(IsIndexValid(mLength - 1 - indexFromEnd));
		return mElements[mLength - 1 - indexFromEnd];
	}
	/*
	*/
	const TElement& Last(int indexFromEnd = 0) const
	{
		assert(IsIndexValid(mLength - 1 - indexFromEnd));
		return mElements[mLength - 1 - indexFromEnd];
	}
	/*
	destroy all the elements but keep the memory for further use
	*/
	void Reset()
	{
		if (mLength != 0)
		{
			JArrayUtil::DestroyElements(Elements(), mLength);
			mLength = 0;
		}
	}
	/*
	destroy all the elements and release the memory
	*/
	void Empty()
	{
		Reset();

		if (mMemory)
		{
			::free(mMemory);
			mElements = mMemory = nullptr;
			mCapacity = 0;
		}
	}

	/*
	*/
	bool IsIndexValid(int index) const { return index < mLength&& index >= 0; }
	/*
	returns true if the specified address is in a valid pointer
	*/
	bool IsAddressValid(const void* address) const
	{
		return IndexOfAddress(address) != -1;
	}
	/*
	*/
	void SetLength(int newLength)
	{
		if (newLength < mLength)
		{
			Pop(mLength - newLength);
		}
		else if (newLength > mLength)
		{
			AddDefault(newLength - mLength);
		}
	}
	/*
	removes @count elements from the end of array
	*/
	void Pop(int count = 1)
	{
		assert(count <= mLength);
		JArrayUtil::DestroyElements(mElements + (mLength - count), count);
		mLength -= count;
	}
	/*
	removes @count elements from the beginning of array
	*/
	void Shift(int count = 1)
	{
		assert(count <= mLength);
		JArrayUtil::DestroyElements(mElements, count);
		mElements += count;
		mLength -= count;
	}
	/*
	*/
	int Avail() const { return mCapacity - mLength; }
	/*
	*/
	int GetLeftSlack() const { return mElements - mMemory; }
	/*
	*/
	int GetRightSlack() const { return Avail(); }
	/*
	*/
	int GetSlack() const { return GetLeftSlack() + GetRightSlack(); }
	/*
	free the unused space from head and tail
	*/
	void Shrink()
	{
		ShrinkIf(0);
	}
	/*
	shrink the array if we have more than @min empty slot
	*/
	bool ShrinkIf(int min)
	{
		if (GetSlack() > min)
		{
			assert(mMemory);

			if (mLength == 0) //if we don't have any element just empty it
			{
				Empty();
			}
			else
			{
				//we use alloc and memcpy cause we want to rebase and remove the left slack
				TElement* newBlock = (TElement*)SAlloc(mLength * sizeof(TElement), alignof(TElement));
				memcpy(newBlock, mElements, mLength * sizeof(TElement));
				SFree(mMemory);
				mMemory = newBlock;
				mElements = newBlock;
				mCapacity = mLength;
			}
			return true;
		}

		return false;
	}
	/*
	*/
	void RemoveAtSwap(int index)
	{
		assert(IsIndexValid(index));
		JArrayUtil::RemoveAtSwap(Elements(), mLength, index);
	}
	/*
	*/
	void RemoveAt(int index, int count = 1)
	{
		assert(IsIndexValid(index) && IsIndexValid(index + count - 1));

		if (index == 0)
		{
			Shift(count);
		}
		else if (index + count - 1 == mLength)
		{
			Pop(count);
		}
		else
		{
			JArrayUtil::RemoveShift(Elements(), mLength, index, count);
		}
	}
	/*
	*/
	int AddUnInit(int count = 1)
	{
		if (GetRightSlack() < count)
			IncCapacity(count);

		mLength += count;
		return mLength - count;
	}
	/*
	*/
	int AddZero(int count = 1)
	{
		int index = AddUnInit(count);
		memset(Elements() + index, 0, sizeof(TElement) * count);
		return index;
	}
	/*
	*/
	int AddDefault(int count = 1)
	{
		int index = AddUnInit(count);
		for (int i = index; i < index + count; i++)
			new (Elements() + i) TElement();
		return index;
	}
	/*
	*/
	template<typename... Args> int AddEmplace(Args&&... args)
	{
		int index = AddUnInit(1);
		new (mElements + index) TElement(std::forward<Args>(args)...);
		return index;
	}
	/*
	*/
	int InsertUnInit(int index, int count = 1)
	{
		assert(index >= 0 && index <= mLength);

		if (count == 0)
			return -1;

		if (index == mLength)
		{
			AddUnInit(count);
		}
		else if (index == 0 && GetLeftSlack() >= count)
		{
			mElements -= count;
			mLength += count;
		}
		else
		{
			if (GetRightSlack() < count)
				IncCapacity(count);

			memmove(Elements() + (index + count), Elements() + index, (mLength - index) * sizeof(TElement));
			mLength += count;
		}
		return index;
	}
	/*
	*/
	int InsertDefault(int index, int count = 1)
	{
		InsertUnInit(index, count);
		for (int i = index; i < index + count; i++)
			new (mElements + i) TElement();
		return index;
	}
	/*
	*/
	int InsertZero(int index, int count = 1)
	{
		InsertUnInit(index, count);
		memset(Elements() + index, 0, sizeof(TElement) * count);
		return index;
	}
	/*
	*/
	int InsertArray(int index, TElement* elements, int count)
	{
		InsertUnInit(index, count);
		JArrayUtil::ConstructElements(Elements() + index, elements, count);
		return index;
	}
	/*
	*/
	int InsertArray(int index, const VArray& arrayToInsert)
	{
		return InsertArray(index, arrayToInsert.Elements(), arrayToInsert.Length());
	}
	/*
	*/
	template<typename... Args> int InsertEmplace(int index, int count, Args&&... args)
	{
		InsertUnInit(index, count);
		for (int i = index; i < index + count; i++)
			new (mElements + i) TElement(std::forward<Args>(args)...);
		return index;
	}
	static void* SAlloc(size_t size, size_t align)
	{
		return ::malloc(size); //#TODO
	}
	static void SFree(void* ptr)
	{
		::free(ptr);
	}
	static void* SRealloc(void* memory, size_t newSize, size_t align)
	{
		return ::realloc(memory, newSize);
	}
	/*
	*/
	void IncCapacity(int count)
	{
		int newCapacity = mCapacity + std::max(count, 128); //#TODO calculate new appropriate capacity

		mCapacity = newCapacity;

		if (mMemory == nullptr)
		{
			mMemory = (TElement*)SAlloc(newCapacity * sizeof(TElement), alignof(TElement));
			mElements = mMemory;
		}
		else
		{
			//#TODO maybe new design ? :thinking: how much is the chance of realloc giving us the same address :thinking:
			int leftSlack = GetLeftSlack();
			mMemory = (TElement*)SRealloc(mMemory, newCapacity * sizeof(TElement), alignof(TElement));
			mElements = mMemory + leftSlack;
		}

		assert(mMemory);
	}
	/*
	*/
	void SetCapacity(int newCapacity)
	{
		if (newCapacity < mLength)
		{
			assert(newCapacity < mCapacity);
			SetLength(newCapacity);
			Shrink();
		}
		else if (newCapacity > mCapacity)
		{
			IncCapacity(newCapacity - mCapacity);
		}
	}
	/*
	returns true if this array has such an element
	*/
	bool Contains(const TElement& other) const
	{
		return IndexOf(other) != -1;
	}
	/*
	*/
	int IndexOf(const TElement& other) const
	{
		for (int i = 0; i < mLength; i++)
			if (mElements[i] == other)
				return i;

		return -1;
	}
	/*
	*/
	template<typename TPredicate> int IndexOfByPredicate(TPredicate predicate)
	{
		for (int i = 0; i < mLength; i++)
			if (predicate(mElements[i]))
				return i;

		return -1;
	}
	/*
	*/
	int LastIndexOf(const TElement& other) const
	{
		int last = -1;
		for (int i = 0; i < mLength; i++)
			if (mElements[i] == other)
				last = i;

		return last;
	}
	/*
	*/
	template<typename TPredicate> int LastIndexOfByPredicate(TPredicate predicate) const
	{
		int last = -1;
		for (int i = 0; i < mLength; i++)
			if (predicate(mElements[i]))
				last = i;

		return last;
	}
	/*
	returns a valid index if the specified address is pointer to one of the elements. -1 otherwise.
	*/
	int IndexOfAddress(const void* item) const
	{
		return JArrayUtil::IndexOfAddr(mElements, mLength, item);
	}
	int IndexOfAddressUnsafe(const void* item) const
	{
		return JArrayUtil::IndexOfAddrUnsafe(mElements, mLength, item);
	}
	/*
	*/
	TElement* Find(const TElement& other)
	{
		int index = IndexOf(other);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	const TElement* Find(const TElement& other) const
	{
		int index = IndexOf(other);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	template<typename TPredicate> TElement* FindByPredicate(TPredicate predicate)
	{
		int index = IndexOfByPredicate(predicate);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	template<typename TPredicate> const TElement* FindByPredicate(TPredicate predicate) const
	{
		int index = IndexOfByPredicate(predicate);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	TElement* FindLast(const TElement& other)
	{
		int index = LastIndexOf(other);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	const TElement* FindLast(const TElement& other) const
	{
		int index = LastIndexOf(other);
		return index == -1 ? nullptr : GetElementAt(index);
	}
	/*
	*/
	bool Remove(const TElement& other)
	{
		int index = IndexOf(other);
		if (index != -1)
		{
			RemoveAt(index);
			return true;
		}
		return false;
	}
	/*
	*/
	bool RemoveSwap(const TElement& other)
	{
		int index = IndexOf(other);
		if (index != -1)
		{
			RemoveAtSwap(index);
			return true;
		}
		return false;
	}
	/*
	*/
	void Reverse()
	{
		JArrayUtil::Reverse(Elements(), Length());
	}
};












/*
*/
template <typename TElement, int MaxLength> struct VStackArray
{
	using ElementT = TElement;

	int	mLength;
	union
	{
		TElement mElements[MaxLength];
	} alignas(TElement);

	/*
	*/
	int Length() const { return mLength; }
	/*
	*/
	int Capacity() const { return MaxLength; }
	/*
	*/
	TElement* Elements() { return (TElement*)mElements; }
	/*
	*/
	const TElement* Elements() const { return (const TElement*)mElements; }
	/*
	returns pointer to element at the specified index if index is valid
	*/
	TElement* GetElementAt(int index)
	{
		return IsIndexValid(index) ? Elements() + index : nullptr;
	}
	/*
	returns a const pointer to element at the specified index if index is valid
	*/
	const TElement* GetElementAt(int index) const
	{
		return IsIndexValid(index) ? Elements() + index : nullptr;
	}
	/*
	*/
	int Avail() const { return MaxLength - mLength; }

	/*
	default constructor.
	*/
	VStackArray() : mLength(0) {}
	/*
	destructor
	*/
	~VStackArray()
	{
		Empty();
	}
	/*
	copy constructor that copies from another stack array.
	*/
	template<int OtherMaxLength> VStackArray(const VStackArray<TElement, OtherMaxLength>& copy)
	{
		static_assert(OtherMaxLength <= MaxLength);
	}
	/*
	move constructor that moves elements from another stack array.
	*/
	template<int OtherMaxLength> VStackArray(VStackArray<TElement, OtherMaxLength>&& move)
	{
		static_assert(OtherMaxLength <= MaxLength);
	}
	/*
	construct by copying from C array
	*/
	VStackArray(const TElement* elements, int length)
	{
		assert(length <= MaxLength);

		mLength = length;

		for (int i = 0; i < length; i++)
			new (Elements() + i) TElement(elements[i]);
	}
	/*
	*/
	VStackArray(const std::initializer_list<TElement>& elements)
	{

	}
	/*
	move constructor.
	*/
	VStackArray& operator = (const VStackArray& copyFrom)
	{

	}
	/*
	move assignment.
	*/
	VStackArray& operator = (VStackArray&& moveFrom)
	{

	}
	/*
	*/
	TElement& operator [] (int index)
	{
		assert(IsIndexValid(index));
		return Elements()[index];
	}
	/*
	*/
	const TElement& operator [](int index) const
	{
		assert(IsIndexValid(index));
		return Elements()[index];
	}
	/*
	*/
	bool IsIndexValid(int index) const { return index < mLength && index >= 0; }
	/*
	*/
	void Reset()
	{
		JArrayUtil::DestroyElements(Elements(), mLength);
		mLength = 0;
	}
	/*
	*/
	void Empty()
	{
		Reset();
	}
	/*
	removes @count element from the end of array
	*/
	void Pop(int count = 1)
	{
		assert(count <= mLength);
		JArrayUtil::DestroyElements(Elements() + (mLength - count), count);
		mLength -= count;
	}
	/*
	removes @count element from the beginning of array
	*/
	void Shift(int count = 1)
	{
		assert(count <= mLength);
		JArrayUtil::RemoveShift(Elements(), mLength, 0, count);
	}
	/*
	*/
	void RemoveAtSwap(int index)
	{
		assert(IsIndexValid(index));
		JArrayUtil::RemoveAtSwap(Elements(), mLength, index);
	}
	/*
	*/
	void Remove(int index, int count = 1)
	{
		assert(IsIndexValid(index) && IsIndexValid(index + count - 1));

		if (index == 0) //remove from the beginning ?
		{
			Shift(count);
		}
		else if (index + count == mLength) //remove from end of array ?
		{
			Pop(count);
		}
		else
		{
			JArrayUtil::RemoveShift(Elements(), mLength, index, count);
		}
	}
	/*
	*/
	void SetLength(int newLength)
	{
		assert(newLength >= 0);

		if (newLength < mLength)
		{
			Pop(mLength - newLength);
		}
		else if (newLength > mLength)
		{
			AddDefault(newLength - mLength);
		}
	}
	/*
	*/
	int AddDefault(int count = 1)
	{
		assert(Avail() >= cout);
		for (int i = 0; i < count; i++)
			new (Elements() + (mLength + i)) TElement();
		mLength += count;
		return mLength - count;
	}
	/*
	*/
	int AddUnInit(int cout = 1)
	{
		assert(Avail() >= cout);
		mLength += count;
		return mLength - count;
	}
	/*
	*/
	int AddZero(int count = 1)
	{
		assert(Avail() >= count);
		memset(Elements() + mLength, 0, count * sizeof(TElement));
		mLength += count;
		return mLength - count;
	}
	/*
	add an element to the end of array and return the index
	*/
	template<typename... Args> int Add(Args&&... args)
	{
		assert(Avail() > 0);
		new (Elements() + mLength) TElement(std::forward<Args>(args)...);
		return mLength++;
	}
	/*
	add an element to the end of array and return the reference
	*/
	template<typename... Args> TElement& AddRef(Args&&... args)
	{
		assert(Avail() > 0);
		new (Elements() + mLength) TElement(std::forward<Args>(args)...);
		mLength += 1;
		return Elements()[mLength - 1];
	}
	/**/
	void Reverse()
	{
		JArrayUtil::Reverse(Elements(), Length());
	}

	TElement& Last(int indexFromEnd = 0)
	{
		assert(IsIndexValid(mLength - 1 - indexFromEnd));
		return Elements()[mLength - 1 - indexFromEnd];
	}
	/*
	*/
	const TElement& Last(int indexFromEnd = 0) const
	{
		assert(IsIndexValid(mLength - 1 - indexFromEnd));
		return Elements()[mLength - 1 - indexFromEnd];
	}
};

