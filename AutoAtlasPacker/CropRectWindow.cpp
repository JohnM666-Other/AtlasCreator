#include "stdafx.h"
#include "CropRectWindow.h"
#include "Application.h"

CropRectWindow::CropRectWindow(ActiveTexture* activeTexture, int borderThickness)
{
	this->activeTexture = activeTexture;
	this->borderThickness = borderThickness;

	for (int i = 0; i < 4; i++)
	{
		clicked[i] = false;
	}

	lockCenter = false;
}

void CropRectWindow::SetRect(sf::IntRect cropRect, sf::Sprite* textureSprite, float zoom)
{
	this->cropRect = sf::IntRect(
		(int)(cropRect.left + textureSprite->getGlobalBounds().left),
		(int)(cropRect.top + textureSprite->getGlobalBounds().top),
		cropRect.width, cropRect.height);

	cropRect = this->cropRect;

	float tw = (triggerWidth / zoom);

	triggers[0] = sf::FloatRect(cropRect.left - tw / 2, cropRect.top - tw / 2, cropRect.width + tw, tw);
	triggers[1] = sf::FloatRect(cropRect.left + cropRect.width - tw / 2, cropRect.top - tw / 2, tw, cropRect.height + tw);
	triggers[2] = sf::FloatRect(cropRect.left - tw / 2, cropRect.top + cropRect.height - tw / 2, cropRect.width + tw, tw);
	triggers[3] = sf::FloatRect(cropRect.left - tw / 2, cropRect.top - tw / 2, tw, cropRect.height + tw);
}

sf::IntRect CropRectWindow::GetRect(sf::Sprite* textureSprite)
{
	return sf::IntRect(
		(int)(cropRect.left - textureSprite->getGlobalBounds().left),
		(int)(cropRect.top - textureSprite->getGlobalBounds().top),
		cropRect.width, cropRect.height);
}

void CropRectWindow::OnMouseButtonDown(sf::RenderWindow* renderer, sf::View* view)
{
	sf::View oldView = renderer->getView();
	renderer->setView(*view);

	sf::Vector2i pixel = sf::Mouse::getPosition(*renderer);
	sf::Vector2f coords = renderer->mapPixelToCoords(pixel);

	renderer->setView(oldView);

	for (int i = 0; i < 4; i++)
	{
		if (triggers[i].contains(coords.x, coords.y))
		{
			clicked[i] = true;
			renderer->setMouseCursorGrabbed(true);
		}
	}
}

void CropRectWindow::OnMouseMove(sf::RenderWindow* renderer, sf::View* view)
{
	sf::View oldView = renderer->getView();
	renderer->setView(*view);

	sf::Vector2i pixel = sf::Mouse::getPosition(*renderer);
	sf::Vector2f coords = renderer->mapPixelToCoords(pixel);

	renderer->setView(oldView);

	bool hover[4] = {};

	for (int i = 0; i < 4; i++)
	{
		if (triggers[i].contains(coords.x, coords.y))
		{
			hover[i] = true;
		}
	}

	if ((hover[0] || hover[2]) && !(hover[1] || hover[3]))
	{
		SetClassLongPtr(renderer->getSystemHandle(), GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_SIZENS));
	}
	else if (!(hover[0] || hover[2]) && (hover[1] || hover[3]))
	{
		SetClassLongPtr(renderer->getSystemHandle(), GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_SIZEWE));
	}
	else if ((hover[0] && hover[3]) || (hover[1] && hover[2]))
	{
		SetClassLongPtr(renderer->getSystemHandle(), GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_SIZENWSE));
	}
	else if ((hover[0] && hover[1]) || (hover[2] && hover[3]))
	{
		SetClassLongPtr(renderer->getSystemHandle(), GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_SIZENESW));
	}
	else
	{
		SetClassLongPtr(renderer->getSystemHandle(), GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, IDC_ARROW));
	}
}

void CropRectWindow::OnMouseButtonUp(sf::RenderWindow* renderer)
{
	for (int i = 0; i < 4; i++)
	{
		clicked[i] = false;
	}

	renderer->setMouseCursorGrabbed(false);
}

void CropRectWindow::OnMouseButton(sf::RenderWindow* renderer, sf::View* view)
{
	sf::View oldView = renderer->getView();
	renderer->setView(*view);

	sf::Vector2i pixel = sf::Mouse::getPosition(*renderer);
	sf::Vector2f coords = renderer->mapPixelToCoords(pixel);

	renderer->setView(oldView);

	float aspectRatio = cropRect.width / (float)cropRect.height;

	if (clicked[0])
	{
		if (coords.y < activeTexture->GetActiveTextureSprite()->getGlobalBounds().top)
		{
			coords.y = activeTexture->GetActiveTextureSprite()->getGlobalBounds().top;
		}

		int deltaHeight = cropRect.top - (int)coords.y;
		int bottom = cropRect.top + cropRect.height + (lockCenter ? deltaHeight : 0);
		cropRect.top = (int)(coords.y);
		cropRect.height = bottom - cropRect.top;

		activeTexture->SetCropRectDataFromWindow();
	}

	if (clicked[1])
	{
		if (coords.x > activeTexture->GetActiveTextureSprite()->getGlobalBounds().left + activeTexture->GetActiveTextureSprite()->getGlobalBounds().width)
		{
			coords.x = activeTexture->GetActiveTextureSprite()->getGlobalBounds().left + activeTexture->GetActiveTextureSprite()->getGlobalBounds().width;
		}

		int deltaWidth = (int)coords.x - (cropRect.left + cropRect.width);
		int right = (int)coords.x;
		cropRect.left -= (lockCenter ? deltaWidth : 0);
		cropRect.width = right - cropRect.left;

		activeTexture->SetCropRectDataFromWindow();
	}

	if (clicked[2])
	{
		if (coords.y > activeTexture->GetActiveTextureSprite()->getGlobalBounds().top + activeTexture->GetActiveTextureSprite()->getGlobalBounds().height)
		{
			coords.y = activeTexture->GetActiveTextureSprite()->getGlobalBounds().top + activeTexture->GetActiveTextureSprite()->getGlobalBounds().height;
		}

		int deltaHeight = (int)coords.y - (cropRect.top + cropRect.height);
		int bottom = (int)coords.y;
		cropRect.top -= (lockCenter ? deltaHeight : 0);
		cropRect.height = bottom - cropRect.top;

		activeTexture->SetCropRectDataFromWindow();
	}

	if (clicked[3])
	{
		if (coords.x < activeTexture->GetActiveTextureSprite()->getGlobalBounds().left)
		{
			coords.x = activeTexture->GetActiveTextureSprite()->getGlobalBounds().left;
		}

		int deltaWidth = cropRect.left - (int)coords.x;
		int right = cropRect.left + cropRect.width + (lockCenter ? deltaWidth : 0);
		cropRect.left = (int)coords.x;
		cropRect.width = right - cropRect.left;

		activeTexture->SetCropRectDataFromWindow();
	}
}

void CropRectWindow::Render(sf::RenderWindow* renderer, float zoom)
{
	sf::RectangleShape rect;
	rect.setPosition((float)cropRect.left, (float)cropRect.top);
	rect.setSize(sf::Vector2f((float)cropRect.width, (float)cropRect.height));
	rect.setFillColor(sf::Color(255, 255, 255, 0));
	rect.setOutlineColor(sf::Color(255, 0, 0, 255));
	rect.setOutlineThickness(borderThickness / zoom);

	renderer->draw(rect);
}

void CropRectWindow::SetLockCenter(bool flag)
{
	lockCenter = flag;
}
