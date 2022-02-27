#pragma once

#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <assert.h>

#define VAMP_DX11 1

extern ID3D11Device* gD3DDevice;
extern ID3D11DeviceContext* gImmediateContext;

//for ComPtr
using namespace Microsoft::WRL;

struct VGFXDynamicBuffer
{
	ComPtr<ID3D11Buffer> mBuffer;
	D3D11_BUFFER_DESC mDesc = {};


	VGFXDynamicBuffer()
	{

	}
	void Init();
	void* Map(UINT requiredBytes, D3D11_MAP MapType, UINT MapFlags );
	void UnMap();
};