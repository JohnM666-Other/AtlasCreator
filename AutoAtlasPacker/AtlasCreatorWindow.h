#pragma once

#include "TexturesList.h"

class AtlasCreatorWindow
{

public:

	AtlasCreatorWindow(TexturesList* texturesList);

	static void Open(TexturesList* texturesList);
	static void SaveTexture(sf::Texture* texture);

private:

	static AtlasCreatorWindow* singleton;
	static sf::Texture* generatedAtlas;

	TexturesList* texturesList;

	sf::RenderWindow renderer;

	HWND atlasCreatorWindow;

	int width;
	int height;
	float textureWidth;
	float textureHeight;

	int atlasWidth = 1024;
	int atlasHeight = 1024;
	int texturesPerWidth = 8;
	int texturesPerHeight = 8;
	int startFrame;
	int endFrame;

	void UpdatePreview();
	void Render();
	sf::Texture* GenerateAtlas();

	static BOOL CALLBACK AtlasCreator_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};