#include "stdafx.h"
#include "TexturesList.h"
#include "Application.h"
#include "TextureUtility.h"

TexturesList::TexturesList(UINT left, UINT top, UINT width, UINT height)
{
	this->width = width;
	this->top = top;
	this->height = height;
	this->left = left;

	{
		WNDCLASS wnd = {};
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hbrBackground = NULL;
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wnd.hInstance = NULL;
		wnd.lpfnWndProc = TexturesList_WndProc;
		wnd.lpszClassName = L"TexturesList";
		wnd.lpszMenuName = NULL;
		wnd.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClass(&wnd))
		{
			MessageBox(NULL, L"Failed while registering window", L"Error", MB_OK);

			exit(1);
		}
	}

	activeTextureRect.setOutlineColor(sf::Color(50, 50, 255, 255));
	activeTextureRect.setOutlineThickness(2.0f);
	activeTextureRect.setFillColor(sf::Color(255, 255, 255, 0));

	scrollBar = CreateWindow(
		L"TexturesList", L"",
		WS_BORDER | WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		left, top, width, height,
		Application::GetMainWindowHandle(), NULL, NULL, this);

	int xOffset = GetSystemMetrics(SM_CXVSCROLL);

	renderField = CreateWindow(
		L"STATIC", L"",
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		left, top, width - xOffset, height,
		scrollBar, NULL, NULL, NULL);

	renderer.create(renderField);

	UpdateViewport();
	UpdateScrollBarInfo();
}

TexturesList::~TexturesList()
{
	UnregisterClass(L"TexturesList", (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE));
}

void TexturesList::ResizeWindow(UINT left, UINT top, UINT width, UINT height)
{
	this->left = left;
	this->top = top;
	this->width = width;
	this->height = height;

	SetWindowPos(scrollBar, HWND_TOP, left, top, width, height, 0);
	SetWindowPos(renderField, HWND_TOP, left, top, width - GetSystemMetrics(SM_CXHSCROLL), height, 0);

	renderer.setSize(sf::Vector2u(width - GetSystemMetrics(SM_CXHSCROLL), height));

	UpdateViewport();
	UpdateTexturesSprites();
	UpdateActiveTextureRect();
	UpdateScrollBarInfo();
	UpdateScrolling();
}

LRESULT CALLBACK TexturesList::TexturesList_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static std::map<HWND, TexturesList*> texturesLists;

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
		texturesLists.insert({ hWnd, (TexturesList*)(createStruct->lpCreateParams) });
	}
	break;

	case WM_VSCROLL:
	{
		SCROLLINFO scrollInfo;
		scrollInfo.fMask = SIF_ALL;
		GetScrollInfo(texturesLists[hWnd]->scrollBar, SB_VERT, &scrollInfo);

		switch (LOWORD(wParam))
		{
		case SB_PAGEUP:
			scrollInfo.nPos -= (int)texturesLists[hWnd]->view.getSize().y;
			break;
		case SB_PAGEDOWN:
			scrollInfo.nPos += (int)texturesLists[hWnd]->view.getSize().y;
			break;
		case SB_LINEUP:
			scrollInfo.nPos -= 10;
			break;
		case SB_LINEDOWN:
			scrollInfo.nPos += 10;
			break;
		case SB_THUMBTRACK:
			scrollInfo.nPos = HIWORD(wParam);
			break;
		case SB_THUMBPOSITION:
			scrollInfo.nPos = HIWORD(wParam);
			break;
		}

		SetScrollInfo(texturesLists[hWnd]->scrollBar, SB_VERT, &scrollInfo, true);
		texturesLists[hWnd]->UpdateScrolling();

		texturesLists[hWnd]->Render();
	}
	break;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void TexturesList::UpdateScrolling()
{
	SCROLLINFO scrollInfo;
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(scrollBar, SB_VERT, &scrollInfo);
	yOffset = scrollInfo.nPos + view.getSize().y * 0.5f;

	if (loadedTextures.size() > 0)
	{
		if (yOffset > loadedTextures.back().sprite.getGlobalBounds().top + loadedTextures.back().sprite.getGlobalBounds().height - view.getSize().y * 0.5f)
		{
			yOffset = loadedTextures.back().sprite.getGlobalBounds().top + loadedTextures.back().sprite.getGlobalBounds().height - view.getSize().y * 0.5f;
		}

		if (yOffset < view.getSize().y * 0.5f)
		{
			yOffset = view.getSize().y * 0.5f;
		}
	}

	view.setCenter(0.0f, yOffset);
}

void TexturesList::LoadTextures(HINSTANCE hInstance)
{
	OPENFILENAME ofn = {};
	WCHAR fileName[4096] = {};

	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
	ofn.hInstance = hInstance;
	ofn.hwndOwner = renderer.getSystemHandle();
	ofn.lpstrFilter = GetFileFilter();
	ofn.lpstrDefExt = L"png";
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = fileName;
	ofn.lpstrTitle = L"Load textures";
	ofn.nMaxFileTitle = 0;
	ofn.nMaxFile = sizeof(fileName);

	if (!GetOpenFileName(&ofn))
	{
		return;
	}

	std::wstring dir = L"";

	int i = 0;
	for (; fileName[i] != NULL; i++)
	{
		dir += fileName[i];
	}
	i++;

	sf::String path = dir;
	if (!AddFromFile(path.toAnsiString()))
	{
		for (; i < 4096; i++)
		{
			std::wstring str = L"";
			while (fileName[i] != NULL)
			{
				str += fileName[i];
				i++;
			}

			if (str == L"\0")
			{
				break;
			}

			sf::String imagePath = dir + L"\\" + str;

			AddFromFile(imagePath.toAnsiString());
		}
	}
}

bool TexturesList::AddFromFile(std::string filePath)
{
	sf::Texture texture;

	if (!texture.loadFromFile(filePath.c_str()))
	{
		return false;
	}

	AddTexture(&texture);

	return true;
}

void TexturesList::DeleteTexture(std::list<SourceTexture>::iterator iter)
{
	if (activeTexture == iter)
	{
		if (activeTexture == loadedTextures.begin())
			activeTexture++;
		else
			activeTexture--;
	}

	loadedTextures.erase(iter);

	if (loadedTextures.size() == 0)
	{
		Application::SetSaveButtonsActive(false);
	}

	UpdateViewport();
	UpdateTexturesSprites();
	UpdateActiveTextureRect();
	UpdateScrollBarInfo();
}

void TexturesList::DeleteActiveTexture()
{
	DeleteTexture(activeTexture);
}

void TexturesList::UpdateActiveTextureRect()
{
	if (loadedTextures.size() == 0)
	{
		return;
	}

	activeTextureRect.setPosition(sf::Vector2f(activeTexture->sprite.getGlobalBounds().left, activeTexture->sprite.getGlobalBounds().top));
	activeTextureRect.setSize(sf::Vector2f(activeTexture->sprite.getGlobalBounds().width, activeTexture->sprite.getGlobalBounds().height));
}

void TexturesList::UpdateTexturesSprites()
{
	float yPosition = 0.0f;
	for (auto iter = loadedTextures.begin(); iter != loadedTextures.end(); iter++)
	{
		float width = (float)view.getSize().x / (float)iter->texture.getSize().x;
		float height = width;

		yPosition += height * iter->texture.getSize().y * 0.5f;

		iter->sprite.setScale(width * 0.8f, height * 0.8f);
		iter->sprite.setOrigin(
			iter->texture.getSize().x * 0.5f,
			iter->texture.getSize().y * 0.5f);
		iter->sprite.setPosition(0.0f, yPosition);

		iter->index.setPosition(iter->sprite.getGlobalBounds().left, iter->sprite.getGlobalBounds().top);
		iter->resolution.setPosition(iter->sprite.getGlobalBounds().left, iter->sprite.getGlobalBounds().top + iter->sprite.getGlobalBounds().height - 16);

		yPosition += height * iter->texture.getSize().y * 0.5f;
	}
}

void TexturesList::UpdateViewport()
{
	RECT windowRect;
	GetWindowRect(renderField, &windowRect);

	view.setSize((float)(windowRect.right - windowRect.left), (float)(windowRect.bottom - windowRect.top));
	view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

	UpdateScrollBarInfo();
	UpdateScrolling();
}

void TexturesList::UpdateScrollBarInfo()
{
	SCROLLINFO scrollInfo = {};
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	scrollInfo.nPage = (int)view.getSize().y;
	scrollInfo.nPos = 0;
	scrollInfo.nMin = 0;
	if (loadedTextures.size() == 0)
	{
		scrollInfo.nMax = (int)view.getSize().y;
	}
	else
	{
		scrollInfo.nMax = (int)std::max((float)(loadedTextures.back().sprite.getGlobalBounds().top + (float)loadedTextures.back().sprite.getGlobalBounds().height), view.getSize().y);
	}
	SetScrollInfo(scrollBar, SB_VERT, &scrollInfo, true);
}

SourceTexture* TexturesList::GetActiveTexture()
{
	if (loadedTextures.size() == 0)
	{
		return NULL;
	}

	return &(*activeTexture);
}

void TexturesList::Render()
{
	renderer.clear(sf::Color(20, 20, 20, 255));

	if (loadedTextures.size() == 0)
	{
		renderer.display();
		return;
	}

	sf::View oldView = renderer.getView();
	renderer.setView(view);

	sf::RectangleShape background;
	background.setFillColor(sf::Color(20, 20, 20, 255));
	background.setPosition(view.getCenter().x - view.getSize().x * 0.5f, view.getCenter().y - view.getSize().y * 0.5f);
	background.setSize(sf::Vector2f(view.getSize().x, view.getSize().y));

	renderer.draw(background);

	for (auto iter = loadedTextures.begin(); iter != loadedTextures.end(); iter++)
	{
		renderer.draw(iter->sprite);
		renderer.draw(iter->index);
		renderer.draw(iter->resolution);
	}

	renderer.draw(activeTextureRect);

	renderer.setView(oldView);

	renderer.display();
}

void TexturesList::Update(bool& needUpdateActiveTexture)
{
	sf::View oldView = renderer.getView();
	renderer.setView(view);

	needUpdateActiveTexture = false;

	for (auto iter = loadedTextures.begin(); iter != loadedTextures.end(); iter++)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			sf::Vector2i pixel = sf::Mouse::getPosition(renderer);
			sf::Vector2f mousePosition = renderer.mapPixelToCoords(pixel);

			if (mousePosition.x > iter->sprite.getGlobalBounds().left &&
				mousePosition.x < iter->sprite.getGlobalBounds().left + iter->sprite.getGlobalBounds().width &&
				mousePosition.y > iter->sprite.getGlobalBounds().top &&
				mousePosition.y < iter->sprite.getGlobalBounds().top + iter->sprite.getGlobalBounds().height)
			{
				activeTexture = iter;

				needUpdateActiveTexture = true;

				UpdateActiveTextureRect();
			}
		}
	}

	renderer.setView(oldView);
}

void TexturesList::Scroll(int delta)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(renderer);
	if (mousePosition.x < 0 ||
		mousePosition.y < 0 ||
		mousePosition.x >= (int)renderer.getSize().x ||
		mousePosition.y >= (int)renderer.getSize().y)
	{
		return;
	}

	SCROLLINFO scrollInfo;
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(scrollBar, SB_VERT, &scrollInfo);

	scrollInfo.nPos -= delta;

	SetScrollInfo(scrollBar, SB_VERT, &scrollInfo, true);

	UpdateScrolling();
	Render();
}

void TexturesList::ApplyCropForEachTexture(sf::IntRect cropRect)
{
	ApplyCrop(cropRect, loadedTextures);
}

void TexturesList::CropTexturesAsAnimation()
{
	CropAsAnimation(loadedTextures);
}

std::list<SourceTexture>* TexturesList::GetLoadedTextures()
{
	return &loadedTextures;
}

void TexturesList::Clear()
{
	loadedTextures.clear();

	UpdateViewport();
	UpdateTexturesSprites();
	UpdateActiveTextureRect();
	UpdateScrollBarInfo();
	UpdateScrolling();
}

void TexturesList::SaveImages()
{
	OPENFILENAME ofn = {};
	wchar_t fileName[256] = {};
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.hwndOwner = Application::GetMainWindowHandle();
	ofn.lpstrDefExt = L"png";
	ofn.lpstrFile = fileName;
	ofn.lpstrFilter = GetFileFilter();
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lpstrTitle = L"Save images";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = sizeof(fileName);
	ofn.nMaxFileTitle = 0;
	
	if (!GetSaveFileName(&ofn))
	{
		return;
	}

	std::wstring path = fileName;
	std::wstring ext = L"";
	ext = path.substr(ofn.nFileExtension - 1, path.capacity() - ofn.nFileExtension + 1);
	path.erase(ofn.nFileExtension - 1, path.capacity() - ofn.nFileExtension + 1);
	int index = 0;
	for (auto iter = loadedTextures.begin(); iter != loadedTextures.end(); iter++, index++)
	{
		sf::RenderTexture target;
		target.create(iter->cropRect.width, iter->cropRect.height);
		target.clear(sf::Color(255, 255, 255, 0));
		
		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f((float)iter->cropRect.width, (float)iter->cropRect.height));
		rect.setTexture(&iter->texture);
		rect.setTextureRect(iter->cropRect);

		target.draw(rect);
		target.display();

		sf::Image image;
		image = target.getTexture().copyToImage();

		wchar_t number[10] = {};
		_itow_s(index, number, 9, 10);

		std::wstring finalPath = L"";
		finalPath.append(path);
		finalPath.append(number);
		finalPath.append(ext);

		sf::String saveTo = finalPath.c_str();
		image.saveToFile(saveTo);
	}
}

void TexturesList::SaveActiveTexture()
{
	OPENFILENAME ofn = {};
	wchar_t fileName[256] = {};
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.hwndOwner = Application::GetMainWindowHandle();
	ofn.lpstrDefExt = L"png";
	ofn.lpstrFile = fileName;
	ofn.lpstrFilter = GetFileFilter();
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lpstrTitle = L"Save images";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = sizeof(fileName);
	ofn.nMaxFileTitle = 0;

	if (!GetSaveFileName(&ofn))
	{
		MessageBox(NULL, L"Failed saving image", L"Error", MB_ICONERROR | MB_OK);
		return;
	}

	sf::RenderTexture target;
	target.create(activeTexture->cropRect.width, activeTexture->cropRect.height);
	
	sf::RectangleShape rect;
	rect.setPosition(0, 0);
	rect.setSize(sf::Vector2f((float)activeTexture->cropRect.width, (float)activeTexture->cropRect.height));
	rect.setTexture(&activeTexture->texture);
	rect.setTextureRect(activeTexture->cropRect);

	target.draw(rect);
	target.display();

	sf::Image image = target.getTexture().copyToImage();
	sf::String path = fileName;
	image.saveToFile(path);
}

void TexturesList::AddTexture(sf::Texture* texture)
{
	wchar_t indexStr[10] = L"";
	_itow_s((int)loadedTextures.size(), indexStr, 9, 10);

	loadedTextures.push_back({ *texture, sf::Sprite(), sf::Text(indexStr, *Application::GetFont(), 14), sf::Text("", *Application::GetFont(), 14) });
	loadedTextures.back().index.setOutlineColor(sf::Color(255, 255, 255, 255));
	loadedTextures.back().resolution.setOutlineColor(sf::Color(255, 255, 255, 255));

	std::wstring resolution;
	wchar_t xRes[256] = {};
	_itow_s((int)loadedTextures.back().texture.getSize().x, xRes, 255, 10);
	wchar_t yRes[256] = {};
	_itow_s((int)loadedTextures.back().texture.getSize().y, yRes, 255, 10);
	resolution.append(xRes);
	resolution.append(L"x");
	resolution.append(yRes);
	loadedTextures.back().resolution.setString(resolution);

	float aspectRatio = loadedTextures.back().texture.getSize().x / (float)loadedTextures.back().texture.getSize().y;
	float width = (float)view.getSize().x / (float)loadedTextures.back().texture.getSize().x;
	float height = width;

	loadedTextures.back().sprite.setTexture(loadedTextures.back().texture);

	loadedTextures.back().cropRect = GetOptimizedCropRect(&loadedTextures.back().texture);

	activeTexture = loadedTextures.begin();

	UpdateViewport();
	UpdateTexturesSprites();
	UpdateActiveTextureRect();
	UpdateScrollBarInfo();

	Application::SetSaveButtonsActive(true);
}