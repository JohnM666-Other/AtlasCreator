#pragma once

#include "SourceTexture.h"

class TexturesList
{

public:

	TexturesList(UINT left, UINT top, UINT width, UINT height);
	~TexturesList();

	void Render();
	void Update(bool& needUpdateActiveTexture);
	SourceTexture* GetActiveTexture();
	void LoadTextures(HINSTANCE hInstance);
	void DeleteTexture(std::list<SourceTexture>::iterator iter);
	void DeleteActiveTexture();
	void Clear();
	void Scroll(int delta);
	void ResizeWindow(UINT left, UINT top, UINT width, UINT height);
	void ApplyCropForEachTexture(sf::IntRect cropRect);
	void CropTexturesAsAnimation();
	std::list<SourceTexture>* GetLoadedTextures();
	void SaveImages();
	void SaveActiveTexture();
	void AddTexture(sf::Texture* texture);

private:

	UINT left;
	UINT top;
	UINT width;
	UINT height;

	HWND renderField;
	HWND scrollBar;
	sf::RenderWindow renderer;
	sf::View view;

	std::list<SourceTexture> loadedTextures;
	std::list<SourceTexture>::iterator activeTexture;

	sf::RectangleShape activeTextureRect;
	float yOffset;

	static LRESULT CALLBACK TexturesList_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateScrolling();
	void UpdateViewport();
	void UpdateActiveTextureRect();
	void UpdateScrollBarInfo();
	void UpdateTexturesSprites();

	bool AddFromFile(std::string filePath);

};