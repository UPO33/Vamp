#pragma once

#include "VampSprite.h"

struct VAtlasMgr
{
	VAtlas mDefaultAtlas;

	bool bOpen = true;
	int SelectedSprite_Index = -1;
	float SpriteViewScale = 1;
	void DrawUI();

	void DrawUI_AutoGeneratePopup();
	void SaveAtlas();
	void LoadAtlas();
	void DrawSelectedSprite(VSprite* pSelectedSprite);

	void DrawTab_View();
	void DrawTab_Generator();

	void Init();
};

extern VAtlasMgr* gAtlasMgr;