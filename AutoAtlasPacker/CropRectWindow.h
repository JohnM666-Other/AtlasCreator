#pragma once

#include "ActiveTexture.h"

class ActiveTexture;

class CropRectWindow
{

public:

	CropRectWindow(ActiveTexture* activeTexture, int borderThickness);

	void SetRect(sf::IntRect cropRect, sf::Sprite* textureSprite, float zoom);
	sf::IntRect GetRect(sf::Sprite* textureSprite);
	void OnMouseButton(sf::RenderWindow* renderer, sf::View* view);
	void OnMouseButtonUp(sf::RenderWindow* renderer);
	void OnMouseButtonDown(sf::RenderWindow* renderer, sf::View* view);
	void OnMouseMove(sf::RenderWindow* renderer, sf::View* view);
	void Render(sf::RenderWindow* renderer, float zoom);
	void SetLockCenter(bool flag);

private:

	int borderThickness;
	static const int triggerWidth = 20;

	ActiveTexture* activeTexture;

	sf::IntRect cropRect;
	sf::FloatRect triggers[4];
	bool clicked[4];

	bool lockCenter;

};