#include "VampEngine.h"
#include "VampScene.h"
#include "imgui.h"
#include "GameplayConfig.h"
#include "VampAtlasMgr.h"

VEngine* gEngine;

VEngine::VEngine()
{
	//0 index is null texture
	{
		VTexture2D::AllTextures.AddEmplace(nullptr);
		VSprite::AllSprites.AddEmplace(nullptr);
	}

	{
		mScene = new VScene();
		
	}
}

void VEngine::LoadAssets()
{
	mCharactersTx = LoadTextureFromFile("characters.png");
	mIllustrations = LoadTextureFromFile("illustrations.png");
	mTilemap_Characters = LoadTextureFromFile("Tilemap/characters.png");
	mTilemap_Ships = LoadTextureFromFile("Tilemap/ships.png");

	LoadTextureFromFile("items.png");
	LoadTextureFromFile("monsters.png");

	gAtlasMgr->Init();

	VGameplayConfig::Get().Init();
}

void VEngine::Render()
{
	

	SpriteRenderer.BeginRender();
	mScene->Render();
	SpriteRenderer.EndRender();
}

void VEngine::Tick(float delta)
{
	mScene->Tick(delta);
}

VTexture2D* VEngine::LoadTextureFromFile(const char* name)
{
	VTexture2D* pTX = VTexture2D::LoadFromFile("../Assets/", name);
	pTX->mTextureIndex = VTexture2D::AllTextures.Length();
	VTexture2D::AllTextures.AddEmplace(pTX);

	return pTX;
}




void VEngine::SpawnTestSprite(VSprite* sprite)
{
	VEntSprite& ent = mScene->AddSprite();
	ent.mSprite = sprite;
	ent.mLocation = ImGui::GetMousePos();
	
}

