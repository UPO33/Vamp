#pragma once

#include "VampGFX.h"
#include "VampSprite.h"


struct VSpriteRenderer
{
	struct VSConstantBuffer
	{
		VVec2F ViewPortSize;
		VVec2F InvViewportSize;
	};

	ComPtr<ID3D11VertexShader> VSBatch;
	ComPtr<ID3D11PixelShader> PSBatch;
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11InputLayout> InputLayout;
	ComPtr<ID3D11Buffer> ConstantBuffer;
	ComPtr<ID3D11BlendState> AlphaBlendState;
	ComPtr<ID3D11BlendState> DefaultBlendState;
	ComPtr<ID3D11RasterizerState> RasterizerState;
	ComPtr<ID3D11RasterizerState> WireRasterizerState;
	ComPtr<ID3D11DepthStencilState> DepthNoneState;
	ComPtr<ID3D11SamplerState> DefaultSampler;

	VGFXDynamicBuffer mVertexBuffer;
	VGFXDynamicBuffer mIndexBuffer;

	bool mDrawWireFrame = false;

	VSpriteRenderer();

	void BeginRender();
	void EndRender();

	void Render();
	
};