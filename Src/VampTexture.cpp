#include "VampTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

VTexture2D* VTexture2D::LoadFromFile(const char* folder, const char* name)
{
	char filename[1024] = {0};
	strcat_s(filename, folder);
	strcat_s(filename, name);

	int w, h, channels;
	stbi_uc* data = stbi_load(filename, &w, &h, &channels, 4);
	if (!data)
		return nullptr;


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = w;
	desc.Height = h;
	desc.SampleDesc.Count = 1;
	
	D3D11_SUBRESOURCE_DATA dxdata;
	dxdata.pSysMem = data;
	dxdata.SysMemPitch = w * 4;

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT creationResult = gD3DDevice->CreateTexture2D(&desc, &dxdata, &pTexture);
	if (creationResult != S_OK)
	{
		STBI_FREE(data);
		return nullptr;
	}

	ID3D11ShaderResourceView* pView = nullptr;
	HRESULT viewResult = gD3DDevice->CreateShaderResourceView(pTexture, nullptr, &pView);
	if (viewResult != S_OK)
	{
		STBI_FREE(data);
		pTexture->Release();
		return nullptr;
	}

	
	VTexture2D* pNewTexture = new VTexture2D{name, pTexture, pView, VVec2I(w,h)};
	return pNewTexture;
}

void VTexture2D::SerializeHandle(VTexture2D*& ptr, VInStream& stream)
{
	int nameLen = 0;
	stream >> nameLen;
	char nameStack[VMAX_NAME_LEN];
	stream.Read(nameStack, nameLen);
	nameStack[nameLen] = 0;

	ptr = VTexture2D::FindByName(nameStack);
}

void VTexture2D::SerializeHandle(VTexture2D*& ptr, VOutStream& stream)
{
	if(ptr)
	{
		const int nameLen = (int)strlen(ptr->mName);
		stream << nameLen;
		stream.Write(ptr->mName, nameLen);
	}
	else
	{
		stream << int(0);
	}
}

VTexture2D* VTexture2D::FindByName(const char* name)
{
	for (int i = 0; i < VTexture2D::AllTextures.Length(); i++)
	{
		VTexture2D* ptr = VTexture2D::AllTextures[i];
		if(ptr && strcmp(ptr->mName, name) == 0)
			return ptr;
	}
	return nullptr;
}

VTexture2D* VTexture2D::FindByIndex(int index)
{
	if(AllTextures.IsIndexValid(index))
		return AllTextures[index];

	return nullptr;
}

int VTexture2D::GetTextureIndex(const VTexture2D* ptr)
{
	return ptr ? ptr->mTextureIndex : 0;
}

VArray<VTexture2D*> VTexture2D::AllTextures;

