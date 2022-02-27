#include "GameplayConfig.h"
#include "imgui.h"
#include "ImGuiHelper.h"
#include <fstream>
#include "VampArchive.h"

void VGameplayConfig::DrawUI()
{
	ImGui::Begin("GameConfig", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Action"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SaveConfigs();
			}
			if (ImGui::MenuItem("Reload"))
			{
				LoadConfigs();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	DrawProperties();
	

	ImGui::End();
}

void VGameplayConfig::DrawProperties()
{
	Imgui_Combo_Sprite("mTestSprite", mTestSprite);
	Imgui_Combo_Texture2D("mTestTexture", mTestTexture);
	ImGui::DragFloat2("mCamMoveSpeed", &mCamMoveSpeed.X);
}

void VGameplayConfig::Init()
{
	LoadConfigs();
}

void VGameplayConfig::LoadConfigs()
{
	std::ifstream configFile;
	configFile.open("../Assets/GameConfig.bin", std::ifstream::in | std::ifstream::binary);
	if (configFile.is_open() && configFile.good())
	{
		VInStream inStream(configFile);
		Serialize(inStream);
	}

}

void VGameplayConfig::SaveConfigs()
{

	std::ofstream configFile;
	configFile.open("../Assets/GameConfig.bin", std::ofstream::out | std::ofstream::binary);

	VOutStream outStream(configFile);
	Serialize(outStream);
}

void VGameplayConfig::Serialize(VInStream& stream)
{
	VTexture2D::SerializeHandle(mTestTexture, stream);
	VSprite::SerializeHandle(mTestSprite, stream);
	stream >> mCamMoveSpeed;
}

void VGameplayConfig::Serialize(VOutStream& stream)
{
	VTexture2D::SerializeHandle(mTestTexture, stream);
	VSprite::SerializeHandle(mTestSprite, stream);
	stream << mCamMoveSpeed;
}

VGameplayConfig& VGameplayConfig::Get()
{
	static VGameplayConfig Ins;
	return Ins;
}
