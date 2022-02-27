#pragma once

struct VTexture2D;
struct VSprite;

bool Imgui_Combo_Texture2D(const char* lable, VTexture2D*& pointer);
bool Imgui_Combo_Sprite(const char* lable, VSprite*& pointer);

