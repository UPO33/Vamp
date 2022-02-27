#include "VampGFX.h"



void VGFXDynamicBuffer::Init()
{
	mBuffer = nullptr;
	HRESULT result = gD3DDevice->CreateBuffer(&mDesc, nullptr, mBuffer.GetAddressOf());
	assert(result == S_OK);
}

void* VGFXDynamicBuffer::Map(UINT requiredBytes, D3D11_MAP MapType, UINT MapFlags)
{
	if (mDesc.ByteWidth < requiredBytes)
	{
		mBuffer = nullptr;
		mDesc.ByteWidth = requiredBytes;
		HRESULT result = gD3DDevice->CreateBuffer(&mDesc, nullptr, mBuffer.GetAddressOf());
		if (result != S_OK)
			return nullptr;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT result = gImmediateContext->Map(mBuffer.Get(), 0, MapType, MapFlags, &mapped);
	if (result != S_OK)
		return nullptr;

	return mapped.pData;
}

void VGFXDynamicBuffer::UnMap()
{
	gImmediateContext->Unmap(mBuffer.Get(), 0);
}
