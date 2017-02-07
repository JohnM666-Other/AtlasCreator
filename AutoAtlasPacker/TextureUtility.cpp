#include "stdafx.h"
#include "TextureUtility.h"

sf::IntRect GetOptimizedCropRect(sf::Texture* texture)
{
	int minX, minY, maxX, maxY;

	sf::Image image = texture->copyToImage();

	for (minX = 0; minX < (int)texture->getSize().x; minX++)
	{
		for (int j = 0; j < (int)texture->getSize().y; j++)
		{
			if (image.getPixel(minX, j).a > 0)
			{
				goto FindMinY;
			}
		}
	}

FindMinY:
	for (minY = 0; minY < (int)texture->getSize().y; minY++)
	{
		for (int i = 0; i < (int)texture->getSize().x; i++)
		{
			if (image.getPixel(i, minY).a > 0)
			{
				goto FindMaxX;
			}
		}
	}

FindMaxX:
	for (maxX = (int)texture->getSize().x - 1; maxX >= 0; maxX--)
	{
		for (int j = 0; j < (int)texture->getSize().y; j++)
		{
			if (image.getPixel(maxX, j).a > 0)
			{
				goto FineMaxY;
			}
		}
	}

FineMaxY:
	for (maxY = (int)texture->getSize().y - 1; maxY >= 0; maxY--)
	{
		for (int i = 0; i < (int)texture->getSize().x; i++)
		{
			if (image.getPixel(i, maxY).a > 0)
			{
				goto Finish;
			}
		}
	}

Finish:

	int sizeX = maxX - minX + 1;
	int sizeY = maxY - minY + 1;

	int size = std::max(sizeX, sizeY);

	minX -= (size - sizeX) / 2;
	minY -= (size - sizeY) / 2;

	return sf::IntRect(minX, minY, size, size);
}

void ApplyCrop(sf::IntRect cropRect, std::list<SourceTexture>& textures)
{
	for (auto iter = textures.begin(); iter != textures.end(); iter++)
	{
		if (cropRect.left + cropRect.width <= (int)iter->texture.getSize().x &&
			cropRect.top + cropRect.height <= (int)iter->texture.getSize().y)
		{
			iter->cropRect = cropRect;
		}
	}
}

void CropAsAnimation(std::list<SourceTexture>& textures)
{
	int minX = INT_MAX;
	int minY = INT_MAX;
	int maxX = 0;
	int maxY = 0;

	for (auto iter = textures.begin(); iter != textures.end(); iter++)
	{
		sf::IntRect crop = GetOptimizedCropRect(&iter->texture);

		if (crop.left < minX)
		{
			minX = crop.left;
		}
		if (crop.top < minY)
		{
			minY = crop.top;
		}
		if (crop.left + crop.width > maxX)
		{
			maxX = crop.left + crop.width;
		}
		if (crop.top + crop.height > maxY)
		{
			maxY = crop.top + crop.height;
		}
	}

	sf::IntRect cropRect = sf::IntRect(minX, minY, maxX - minX, maxY - minY);

	ApplyCrop(cropRect, textures);
}

sf::IntRect MoveCropToCenter(sf::IntRect cropRect, sf::Texture* texture)
{
	int centerX = texture->getSize().x / 2;
	int centerY = texture->getSize().y / 2;

	cropRect.left = centerX - cropRect.width / 2;
	cropRect.top = centerY - cropRect.height / 2;

	return cropRect;
}

sf::IntRect NoCrop(sf::Texture* texture)
{
	sf::IntRect cropRect = { 0, 0, (int)texture->getSize().x, (int)texture->getSize().y };

	return cropRect;
}

LPCWSTR GetFileFilter()
{
	return L"PNG image\0*.png\0BMP image\0*.bmp\0TGA image\0*.tga\0JPG image\0*.jpg\0PSD image\0*.psd\0All files\0*\0\0";
}