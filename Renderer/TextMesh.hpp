#pragma once

#include "../Core/Math/Vector3.hpp"
#include "Font.hpp"

struct TextMesh
{
	TextMesh(std::string text, float scale, Vector3 color, std::shared_ptr<Font> font) : text(text), scale(scale), color(color), font(font) {};

	std::string text;
	float scale;
	Vector3 color;
	std::shared_ptr<Font> font;

	static void Render(TextMesh textMesh, float x, float y);
};
