#pragma once

#include "SourceTexture.h"
#include "TexturesList.h"
#include "ActiveTexture.h"

class ActiveTexture;

class Application
{

public:

	static UINT SCREEN_WIDTH;
	static UINT SCREEN_HEIGHT;

	int Run(HINSTANCE hInstance);

	static sf::Font* GetFont();
	static UINT GetScreenWidth();
	static UINT GetScreenHeight();
	static HWND GetMainWindowHandle();
	static void SetSaveButtonsActive(bool flag);

private:

	static Application* singleton;

	TexturesList* texturesList;
	ActiveTexture* activeTexture;

	HWND mainWindow;
	HMENU menu;
	sf::Clock timer;
	sf::Font font;

	int GetAtlasSize(std::list<sf::Texture> textures);
	int GetImageSize(int atlasSize);

	void Render();
	void Init(HINSTANCE hInstance);
	void InitMenu();

	static LRESULT CALLBACK MainWindow_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	double deltaTime;

	static const UINT_PTR ID_LOADIMAGES = 1001;
	static const UINT_PTR ID_LOADATLAS = 1002;
	static const UINT_PTR ID_SAVEIMAGES = 1003;
	static const UINT_PTR ID_SAVEATLAS = 1004;
	static const UINT_PTR ID_EXIT = 1006;
	static const UINT_PTR ID_CLEAR = 1500;
	static const UINT_PTR ID_ANIMATION = 1501;

};