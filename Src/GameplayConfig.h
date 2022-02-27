#pragma once

#include "VampSprite.h"
#include "VampArchive.h"

struct VGameplayConfig
{
	static VGameplayConfig& Get();

	VTexture2D* mTestTexture = nullptr;
	VSprite* mTestSprite = nullptr;
	VVec2F mCamMoveSpeed;

	void DrawUI();
	void DrawProperties();
	void Init();
	void LoadConfigs();
	void SaveConfigs();

	void Serialize(VInStream& stream);
	void Serialize(VOutStream& stream);
};