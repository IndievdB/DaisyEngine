#pragma once

#include <map>
#include "../Core/Math/Vector2.hpp"

struct Character
{
	int TextureID;  // ID handle of the glyph texture
	Vector2 Size;       // Size of glyph
	Vector2 Bearing;    // Offset from baseline to left/top of glyph
	int Advance;    // Offset to advance to next glyph
};

struct Font
{
	Font(std::string filePath);
	std::map<char, Character> characters;
};