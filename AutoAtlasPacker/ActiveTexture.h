#pragma once

#include "Application.h"
#include "CropRectWindow.h"

class CropRectWindow;

class ActiveTexture
{

public:

	ActiveTexture(UINT left, UINT top, UINT width, UINT height, HWND parent, TexturesList* texturesList);
	~ActiveTexture();

	void Render();
	void SetCropRectDataFromWindow();
	void UpdateActiveTextureSprite();
	void ResizeWindow(UINT left, UINT top, UINT width, UINT height);

	HWND GetParamsPanelHandle();
	HWND GetRenderFieldHandle();
	sf::Sprite* GetActiveTextureSprite();

private:

	UINT width;
	UINT height;
	UINT left;
	UINT top;

	float paramsPanelRelativeWidth;

	TexturesList* texturesList;

	HWND renderField;
	HWND paramsPanel;

	static const UINT_PTR ID_LEFTFIELD = 1007;
	static const UINT_PTR ID_TOPFIELD = 1008;
	static const UINT_PTR ID_WIDTHFIELD = 1009;
	static const UINT_PTR ID_HEIGHTFIELD = 1010;
	static const UINT_PTR ID_APPLYTOEVERYTHING = 1011;
	static const UINT_PTR ID_OPTIMIZECROPRECT = 1012;
	static const UINT_PTR ID_CROPASANIMATION = 1013;
	static const UINT_PTR ID_MOVETOCENTER = 1014;
	static const UINT_PTR ID_NOCROP = 1015;
	static const UINT_PTR ID_LOCKCENTER = 1016;
	static const UINT_PTR ID_DELETETEXTURE = 1018;
	static const UINT_PTR ID_SAVEIMAGE = 1019;

	HWND leftFieldTitle;
	HWND leftField;
	HWND topFieldTitle;
	HWND topField;
	HWND widthFieldTitle;
	HWND widthField;
	HWND heightFieldTitle;
	HWND heightField;
	HWND applyToEverything;
	HWND optimizeCropRectWindow;
	HWND cropAsAnimation;
	HWND moveToCenter;
	HWND noCrop;
	HWND lockCenter;
	HWND deleteTexture;
	HWND saveTexture;

	CropRectWindow* cropRectWindow;

	sf::RenderWindow renderer;
	sf::View view;
	float zoom;

	sf::Sprite activeTextureSprite;

	void UpdateViewport();
	void UpdateCropRectText();
	void SetParamsPanelActive(bool flag);
	void CheckCropRect();

	static LRESULT CALLBACK ParamsPanel_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};