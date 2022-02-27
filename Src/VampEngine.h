#pragma once

#include "VampTexture.h"
#include "SpriteBatch.h"

struct VScene;

struct VEngine
{
	static const int MAX_TEXTURE = 64;

	VTexture2D* mCharactersTx = nullptr;
	VTexture2D* mIllustrations = nullptr;
	VTexture2D* mTilemap_Ships = nullptr;
	VTexture2D* mTilemap_Characters = nullptr;

	VSpriteRenderer SpriteRenderer;
	VScene* mScene;

	VEngine();

	void LoadAssets();
	void Render();
	void Tick(float delta);
	

	VTexture2D* LoadTextureFromFile(const char* name);

	void SpawnTestSprite(VSprite* sprite);
};

extern VEngine* gEngine;