#pragma once

#include "SourceTexture.h"

sf::IntRect GetOptimizedCropRect(sf::Texture* texture);
void ApplyCrop(sf::IntRect cropRect, std::list<SourceTexture>& textures);
void CropAsAnimation(std::list<SourceTexture>& textures);
sf::IntRect MoveCropToCenter(sf::IntRect cropRect, sf::Texture* texture);
sf::IntRect NoCrop(sf::Texture* texture);
LPCWSTR GetFileFilter();