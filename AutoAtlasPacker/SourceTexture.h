#pragma once

#include "stdafx.h"

struct SourceTexture
{
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Text index;
	sf::Text resolution;
	sf::IntRect cropRect;

	bool operator== (SourceTexture value)
	{
		return &value == this;
	}

	bool operator!= (SourceTexture value)
	{
		return !(*this == value);
	}
};