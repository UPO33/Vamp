#include "ImGuiHelper.h"

#include "VampTexture.h"
#include "VampEngine.h"
#include "VampSprite.h"
#include "imgui.h"
#include "VampAtlasMgr.h"

bool Imgui_Combo_Texture2D(const char* lable, VTexture2D*& pointer)
{
	int curTextureIndex = VTexture2D::GetTextureIndex(pointer);

	auto Proc = [](void* data, int idx, const char** outText) -> bool {

		*outText = "null";
		VTexture2D* pTexture = VTexture2D::FindByIndex(idx);
		if(!pTexture)
			return true;
		*outText = pTexture->mName;
		return true;
	};

	if (ImGui::Combo(lable, &curTextureIndex, Proc, nullptr, VTexture2D::AllTextures.Length(), 64))
	{
		pointer = VTexture2D::FindByIndex(curTextureIndex);
		return true;
	}
	return false;
}

bool Imgui_Combo_Sprite(const char* lable, VSprite*& pointer)
{
	int curIndex = VSprite::GetSpriteIndex(pointer);
	auto Proc = [](void* data, int idx, const char** outText) -> bool {
		
		*outText = "null";
		VSprite* pSprite = VSprite::FindByIndex(idx);
		if(!pSprite)
			return true;
		*outText = pSprite->mName;
		return true;
	};

	if (ImGui::Combo(lable, &curIndex, Proc, nullptr, VSprite::AllSprites.Length(), 64))
	{
		pointer = VSprite::FindByIndex(curIndex);
		return true;
	}
	return false;
}

