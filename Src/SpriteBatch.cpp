#include "SpriteBatch.h"
#include "imgui.h"
#include "VampEngine.h"

VSpriteRenderer::VSpriteRenderer()
{
	//create default smapler
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.MaxLOD = FLT_MAX;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		assert(gD3DDevice->CreateSamplerState(&desc, DefaultSampler.GetAddressOf()) == S_OK);
	}

	// Create the blending setup
	{
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			assert(gD3DDevice->CreateBlendState(&desc, AlphaBlendState.GetAddressOf()) == S_OK);
		}
		
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = false;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			assert(gD3DDevice->CreateBlendState(&desc, DefaultBlendState.GetAddressOf()) == S_OK);
		}
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = false;
		desc.DepthClipEnable = false;
		assert(gD3DDevice->CreateRasterizerState(&desc, RasterizerState.GetAddressOf()) == S_OK);
		desc.FillMode = D3D11_FILL_WIREFRAME;
		assert(gD3DDevice->CreateRasterizerState(&desc, WireRasterizerState.GetAddressOf()) == S_OK);
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = false;
		desc.StencilEnable = false;
		assert(gD3DDevice->CreateDepthStencilState(&desc, DepthNoneState.GetAddressOf()) == S_OK);
	}

	//create vertex buffer
	{
		VSprite::Vertex vertices[4] = {};

		Helper_MakeQuadTriStrip(vertices, VVec2F(100), VVec2F(400));





		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(vertices);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = vertices;
		data.SysMemPitch = sizeof(vertices);

		assert(gD3DDevice->CreateBuffer(&desc, &data, VertexBuffer.GetAddressOf()) == S_OK);
	}
	ComPtr<ID3DBlob> vsBloob, vsError, psBloob, psError;

	//compile and create shader
	{
		auto filename = L"../Src/Shaders/SpriteBatch.hlsl";
		HRESULT vsCompileResult = D3DCompileFromFile(filename, nullptr, nullptr, "VSMain", "vs_4_0", 0, 0, vsBloob.GetAddressOf(), vsError.GetAddressOf());
		assert(vsCompileResult == S_OK);
		HRESULT psCompileResult = D3DCompileFromFile(filename, nullptr, nullptr, "PSMain", "ps_4_0", 0, 0, psBloob.GetAddressOf(), psError.GetAddressOf());
		assert(psCompileResult == S_OK);

		assert(gD3DDevice->CreateVertexShader(vsBloob->GetBufferPointer(), vsBloob->GetBufferSize(), nullptr, VSBatch.GetAddressOf()) == S_OK);
		assert(gD3DDevice->CreatePixelShader(psBloob->GetBufferPointer(), psBloob->GetBufferSize(), nullptr, PSBatch.GetAddressOf()) == S_OK);
	}

	assert(sizeof(VSprite::Vertex) == 16);

	//create input layout
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(VSprite::Vertex, mPosition)	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(VSprite::Vertex, mUV)		, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		assert(gD3DDevice->CreateInputLayout(layout, 2, vsBloob->GetBufferPointer(), vsBloob->GetBufferSize(), InputLayout.GetAddressOf()) == S_OK);
	}

	// Create the constant buffer
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(VSConstantBuffer);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		assert(gD3DDevice->CreateBuffer(&desc, nullptr, ConstantBuffer.GetAddressOf()) == S_OK);
	}


	///---------------------------------- NEW ----------------------------------
	{
		mVertexBuffer.mDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		mVertexBuffer.mDesc.Usage = D3D11_USAGE_DYNAMIC;
		mVertexBuffer.mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		mVertexBuffer.mDesc.ByteWidth = sizeof(VSprite::Vertex) * 4096 * 4;
		mVertexBuffer.Init();

		mIndexBuffer.mDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		mIndexBuffer.mDesc.Usage = D3D11_USAGE_DYNAMIC;
		mIndexBuffer.mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		mIndexBuffer.mDesc.ByteWidth = sizeof(uint32_t) * 4096 * 6;
		mIndexBuffer.Init();
	}
}

void VSpriteRenderer::Render()
{

}

void VSpriteRenderer::BeginRender()
{
	gImmediateContext->OMSetDepthStencilState(DepthNoneState.Get(), 0);
	float factors[4] = { 0,0,0,0 };
	if(mDrawWireFrame)
	{
		gImmediateContext->RSSetState(WireRasterizerState.Get());
		gImmediateContext->OMSetBlendState(DefaultBlendState.Get(), factors, 0xFFffFFff);
	}
	else
	{
		gImmediateContext->RSSetState(RasterizerState.Get());
		gImmediateContext->OMSetBlendState(AlphaBlendState.Get(), factors, 0xFFffFFff);
	}
	

	D3D11_VIEWPORT vp = {};
	vp.Width = ImGui::GetIO().DisplaySize.x;
	vp.Height = ImGui::GetIO().DisplaySize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	gImmediateContext->RSSetViewports(1, &vp);

	gImmediateContext->VSSetShader(VSBatch.Get(), nullptr, 0);
	gImmediateContext->PSSetShader(PSBatch.Get(), nullptr, 0);

	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		gImmediateContext->Map(ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		VSConstantBuffer* pCB = (VSConstantBuffer*)mappedData.pData;
		pCB->ViewPortSize =  VVec2F(ImGui::GetIO().DisplaySize);
		pCB->InvViewportSize = VVec2F(1) / pCB->ViewPortSize;
		gImmediateContext->Unmap(ConstantBuffer.Get(), 0);

		gImmediateContext->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	}
	
	//gImmediateContext->PSSetSamplers(0, 1, DefaultSampler.GetAddressOf());
	//gImmediateContext->PSSetShaderResources(0, 1, gEngine->mCharactersTx->mDefaultView.GetAddressOf());

	gImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gImmediateContext->IASetInputLayout(InputLayout.Get());

	//ID3D11Buffer* buffers[] = { VertexBuffer.Get() };
	//UINT strides[] = { sizeof(VSprite::Vertex) };
	//UINT offsets[] = { 0 };
	//gImmediateContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);


	//gImmediateContext->Draw(4, 0);
}

void VSpriteRenderer::EndRender()
{

}

