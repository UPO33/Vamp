#include "VampAtlasMgr.h"
#include "imgui.h"
#include "VampEngine.h"
#include <fstream>
#include "ImGuiHelper.h"

void VAtlasMgr::DrawUI()
{
	ImGui::Begin("AtlasMgr", &bOpen, ImGuiWindowFlags_MenuBar);
	VAtlas& curAtlas = mDefaultAtlas;
	if (!bOpen)
	{
		ImGui::End();
		return;
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Action"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SaveAtlas();
			}
			if (ImGui::MenuItem("Reload"))
			{
				LoadAtlas();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("TabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("View"))
		{
			DrawTab_View();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Generator"))
		{
			DrawTab_Generator();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

// 	if (ImGui::Button("AutoGenerate"))
// 	{
// 		ImGui::OpenPopup("AutoGenerate?");
// 	}
	//ImGui::SameLine();
	//if (ImGui::Button("SpawnSprite"))
	//{
	//	gEngine->SpawnTestSprite(mDefaultAtlas.mSprites[SelectedSprite_Index]);
	//}

	ImGui::End();


	
}



void VAtlasMgr::DrawUI_AutoGeneratePopup()
{
	// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("AutoGenerate?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static VTexture2D* TargetTexture = nullptr;
		Imgui_Combo_Texture2D("Texture", TargetTexture);

		static bool bUseDivide = true;
		static VVec2I DivideXY = VVec2I(1);
		static float ViewScale = 1;

		ImGui::Checkbox("UseDivide", &bUseDivide);
		ImGui::DragFloat("ViewScale", &ViewScale, 0.1f, 0.1f, 5.0f);

		if(bUseDivide)
		{
			ImGui::DragInt2("Divide", &DivideXY.X);
		}
		else
		{

			static int RepeatXY[2] = { 1,1 };
			ImGui::DragInt2("RepeatXY", RepeatXY);
			static float OffsetXY[2] = { 0,0 };
			ImGui::DragFloat2("OffsetXY", OffsetXY);
			static float SizeXY[2] = {32,32};
			ImGui::DragFloat2("SizeXY", SizeXY);
		}

		VVec2F ImageScreenPos;
		ImGui::Separator();
		if(TargetTexture)
		{
			ImVec2 size = VVec2F(TargetTexture->mSize) * ViewScale;
			ImageScreenPos = ImGui::GetCursorScreenPos();
			ImGui::Image(TargetTexture->mDefaultView.Get(), size);
		}

		if (bUseDivide && TargetTexture)
		{
			VVec2F cellSize = VVec2F(TargetTexture->mSize) / VVec2F(DivideXY) * ViewScale;

			for (int y = 0; y < DivideXY.Y; y++)
			{
				for (int x = 0; x < DivideXY.X; x++)
				{
					VVec2F min = ImageScreenPos + VVec2F(x, y) * cellSize;
					VVec2F max = min + cellSize;
					
					ImGui::GetWindowDrawList()->AddRect(min, max, ImColor(255, 0,0,255));
				}
			}
		}

		if (ImGui::Button("OK", ImVec2(120, 0))) 
		{
			ImGui::CloseCurrentPopup();

			//generate sprites
			if (bUseDivide && TargetTexture)
			{
				VVec2F cellSizeUV = VVec2F(1) / VVec2F(DivideXY);

				for (int y = 0; y < DivideXY.Y; y++)
				{
					for (int x = 0; x < DivideXY.X; x++)
					{
						VSprite* pSprite = mDefaultAtlas.AddSprite();
						sprintf_s(pSprite->mName, "%s_%dx%d", TargetTexture->mName, x, y);
						pSprite->mOffset = cellSizeUV * VVec2F(x, y);
						pSprite->mSize = cellSizeUV;
						pSprite->mTexture = TargetTexture;
						pSprite->OnPropertyChanged();
					}
				}
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) 
		{
			ImGui::CloseCurrentPopup(); 
		}
		ImGui::EndPopup();
	}
}

void VAtlasMgr::SaveAtlas()
{
	std::ofstream atlasFile;
	atlasFile.open("../Assets/Atlas.bin", std::ofstream::out | std::ofstream::binary);

	VOutStream outStream(atlasFile);
	mDefaultAtlas.Serialize(outStream);
}

void VAtlasMgr::LoadAtlas()
{
	std::ifstream atlasFile;
	atlasFile.open("../Assets/Atlas.bin", std::ifstream::in | std::ifstream::binary);
	if (atlasFile.is_open() && atlasFile.good())
	{
		VInStream inStream(atlasFile);
		mDefaultAtlas.Serialize(inStream);
	}

}

void VAtlasMgr::DrawSelectedSprite(VSprite* pSelectedSprite)
{
	if (pSelectedSprite)
	{
		bool bPropertyChanged = false;
		bPropertyChanged |= ImGui::InputText("Name", pSelectedSprite->mName, VMAX_NAME_LEN);

		bPropertyChanged |= Imgui_Combo_Texture2D("Texture", pSelectedSprite->mTexture);

		bPropertyChanged |= ImGui::DragFloat2("Offset", &pSelectedSprite->mOffset.X, 0.05f, 0, 1);
		bPropertyChanged |= ImGui::DragFloat2("Size", &pSelectedSprite->mSize.X, 0.05f, 0, 1);

		if(bPropertyChanged)
			pSelectedSprite->OnPropertyChanged();

		ImGui::Separator();
		if (pSelectedSprite->mTexture)
		{
			ImVec2 size = ImVec2(pSelectedSprite->GetSizeInPixel() * SpriteViewScale);
			ImVec2 uv0 = pSelectedSprite->mOffset;
			ImVec2 uv1 = pSelectedSprite->mOffset + pSelectedSprite->mSize;
			ImGui::Image(pSelectedSprite->mTexture->mDefaultView.Get(), size, uv0, uv1);
		}
	}
}

void VAtlasMgr::DrawTab_View()
{
	ImGui::Columns(1);
	ImGui::DragFloat("ViewScale", &SpriteViewScale, 0.2f, 0.1f, 10.0f);
	if (ImGui::Button("+"))
	{
		VSprite* pNewSprite = mDefaultAtlas.AddSprite();
		pNewSprite->OnPropertyChanged();
	}
	
	ImGui::Separator();

	ImGui::Columns(2);

	ImGui::BeginChild("child");

	if(ImGui::BeginPopup("item context menu"))
	{
		if (ImGui::Selectable("Delete"))
		{
			if (mDefaultAtlas.mSprites.IsIndexValid(SelectedSprite_Index))
			{
				VSprite* selectedSprite = mDefaultAtlas.mSprites[SelectedSprite_Index];
				delete selectedSprite;
				mDefaultAtlas.mSprites.RemoveAt(SelectedSprite_Index, 1);
			}
			
		}
		if (ImGui::Selectable("Duplicate"))
		{
			if (mDefaultAtlas.mSprites.IsIndexValid(SelectedSprite_Index))
			{
				VSprite* selectedSprite = mDefaultAtlas.mSprites[SelectedSprite_Index];
				VSprite* pNewSprite = mDefaultAtlas.AddSprite();
				
				pNewSprite->mTexture = selectedSprite->mTexture;
				pNewSprite->mSize = selectedSprite->mSize;
				pNewSprite->mOffset = selectedSprite->mOffset;

				pNewSprite->OnPropertyChanged();
			}
			
		}
		ImGui::EndPopup();
	}

	//draw selectable
	for (int iSprite = 0; iSprite < mDefaultAtlas.NumSprite(); iSprite++)
	{
		VSprite* pSprite = mDefaultAtlas.mSprites[iSprite];
		if (ImGui::Selectable(pSprite->mName, SelectedSprite_Index == iSprite))
		{
			SelectedSprite_Index = iSprite;
		}

		ImGui::OpenPopupContextItem("item context menu");



	}
	ImGui::EndChild();



	ImGui::NextColumn();


	VSprite* pSelectedSprite = mDefaultAtlas.GetSpriteByIndex(SelectedSprite_Index);
	DrawSelectedSprite(pSelectedSprite);

	ImGui::Columns(1);
}

void VAtlasMgr::DrawTab_Generator()
{
	{
		static VTexture2D* TargetTexture = nullptr;
		Imgui_Combo_Texture2D("Texture", TargetTexture);

		static bool bUseDivide = true;
		static VVec2I DivideXY = VVec2I(1);
		static float ViewScale = 1;

		ImGui::Checkbox("UseDivide", &bUseDivide);
		ImGui::DragFloat("ViewScale", &ViewScale, 0.1f, 0.1f, 5.0f);

		if (bUseDivide)
		{
			ImGui::DragInt2("Divide", &DivideXY.X);
		}
		else
		{

			static int RepeatXY[2] = { 1,1 };
			ImGui::DragInt2("RepeatXY", RepeatXY);
			static float OffsetXY[2] = { 0,0 };
			ImGui::DragFloat2("OffsetXY", OffsetXY);
			static float SizeXY[2] = { 32,32 };
			ImGui::DragFloat2("SizeXY", SizeXY);
		}

		VVec2F ImageScreenPos;
		ImGui::Separator();
		if (TargetTexture)
		{
			ImVec2 size = VVec2F(TargetTexture->mSize) * ViewScale;
			ImageScreenPos = ImGui::GetCursorScreenPos();
			ImGui::Image(TargetTexture->mDefaultView.Get(), size);
		}

		if (bUseDivide && TargetTexture)
		{
			VVec2F cellSize = VVec2F(TargetTexture->mSize) / VVec2F(DivideXY) * ViewScale;

			for (int y = 0; y < DivideXY.Y; y++)
			{
				for (int x = 0; x < DivideXY.X; x++)
				{
					VVec2F min = ImageScreenPos + VVec2F(x, y) * cellSize;
					VVec2F max = min + cellSize;

					ImGui::GetWindowDrawList()->AddRect(min, max, ImColor(255, 0, 0, 255));
				}
			}
		}

		if (ImGui::Button("Generate", ImVec2(120, 0)))
		{
			//generate sprites
			if (bUseDivide && TargetTexture)
			{
				VVec2F cellSizeUV = VVec2F(1) / VVec2F(DivideXY);

				for (int y = 0; y < DivideXY.Y; y++)
				{
					for (int x = 0; x < DivideXY.X; x++)
					{
						VSprite* pSprite = mDefaultAtlas.AddSprite();
						sprintf_s(pSprite->mName, "%s_%dx%d", TargetTexture->mName, x, y);
						pSprite->mOffset = cellSizeUV * VVec2F(x, y);
						pSprite->mSize = cellSizeUV;
						pSprite->mTexture = TargetTexture;
						pSprite->OnPropertyChanged();
					}
				}
			}
		}
		ImGui::SameLine();
	}
}

void VAtlasMgr::Init()
{
	LoadAtlas();
}

VAtlasMgr* gAtlasMgr;