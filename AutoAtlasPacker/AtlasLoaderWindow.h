#pragma once

#include "TexturesList.h"

class AtlasLoaderWindow
{

public:

	AtlasLoaderWindow(TexturesList* texturesList);

	static void Open(TexturesList* texturesList);

private:

	static AtlasLoaderWindow* singleton;

	TexturesList* texturesList;
	
	sf::RenderWindow renderer;
	sf::Texture atlasTexture;
	bool loaded = false;

	HWND atlasLoaderWindow;

	int texturesPerWidth = 8;
	int texturesPerHeight = 8;
	int totalCount = 64;

	int textureWidth;
	int textureHeight;

	void UpdatePreview();
	void Render();
	void GenerateTextures();

	static BOOL CALLBACK AtlasLoader_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};