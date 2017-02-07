#pragma once

#include "TexturesList.h"

class AnimationPreview
{

public:

	AnimationPreview(TexturesList* texturesList);
	~AnimationPreview();

	static void Open(TexturesList* texturesList);
	static void Update();

private:

	static AnimationPreview* singleton;

	HWND animationPreview;

	sf::RenderWindow renderer;
	sf::Clock timer;

	float totalTime;

	SourceTexture* frames;
	int framesCount;

	int textureWidth;
	int textureHeight;

	int currentFrame;
	int FPS = 30;
	int startFrame;
	int endFrame;

	void UpdatePreview();
	void Render();

	static BOOL CALLBACK AnimationPreview_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};