#include "stdafx.h"
#include "AtlasCreatorWindow.h"
#include "Application.h"
#include "AtlasCreatorResource.h"
#include "TextureUtility.h"

class Application;

AtlasCreatorWindow* AtlasCreatorWindow::singleton;
sf::Texture* AtlasCreatorWindow::generatedAtlas;

AtlasCreatorWindow::AtlasCreatorWindow(TexturesList* texturesList)
{
	this->texturesList = texturesList;

	startFrame = 0;
	endFrame = (int)texturesList->GetLoadedTextures()->size() - 1;
}

BOOL CALLBACK AtlasCreatorWindow::AtlasCreator_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			singleton->atlasCreatorWindow = hWnd;
			singleton->renderer.create(GetDlgItem(hWnd, IDC_PREVIEW));

			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			singleton->width = clientRect.right - clientRect.left;
			singleton->height = clientRect.bottom - clientRect.top;

			{
				wchar_t text[256] = {};
				_itow_s(singleton->atlasWidth, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_ATLASWIDTH), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->atlasHeight, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_ATLASHEIGHT), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->texturesPerWidth, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_TEXPERWIDTH), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->texturesPerHeight, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_TEXPERHEIGHT), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->startFrame, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_STARTFRAME), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->endFrame, text, _countof(text) - 1, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_ENDFRAME), text);
			}
		}
		return true;

		case WM_PAINT:
		{
			singleton->Render();
			ValidateRect(GetDlgItem(hWnd, IDC_PREVIEW), NULL);
		}
		break;

		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (LOWORD(wParam) == IDC_OK)
				{
					generatedAtlas = singleton->GenerateAtlas();
					EndDialog(hWnd, wParam);
					delete singleton;
					if (generatedAtlas == NULL)
					{
						return false;
					}
					return true;
				}

				if (LOWORD(wParam) == IDC_CANCEL)
				{
					EndDialog(hWnd, wParam);
					delete singleton;
					return false;
				}

				if (LOWORD(wParam) == IDC_BYROWS ||
					LOWORD(wParam) == IDC_BYCOLUMNS ||
					LOWORD(wParam) == IDC_INVERSED)
				{
					singleton->UpdatePreview();
				}
			}

			if (HIWORD(wParam) == EN_CHANGE)
			{
				if (LOWORD(wParam) == IDC_ATLASWIDTH)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_ATLASWIDTH), text, _countof(text) - 1);
					singleton->atlasWidth = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_ATLASHEIGHT)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_ATLASHEIGHT), text, _countof(text) - 1);
					singleton->atlasHeight = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_TEXPERWIDTH)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_TEXPERWIDTH), text, _countof(text) - 1);
					singleton->texturesPerWidth = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_TEXPERHEIGHT)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_TEXPERHEIGHT), text, _countof(text) - 1);
					singleton->texturesPerHeight = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_STARTFRAME)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_STARTFRAME), text, _countof(text) - 1);
					singleton->startFrame = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_ENDFRAME)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_ENDFRAME), text, _countof(text) - 1);
					singleton->endFrame = _wtoi(text);
				}

				singleton->UpdatePreview();
			}
		}
		break;
	}

	return false;
}

void AtlasCreatorWindow::Render()
{
	renderer.clear(sf::Color(255, 255, 255, 255));

	sf::RectangleShape textureBackground;
	textureBackground.setFillColor(sf::Color(0, 0, 0, 255));
	textureBackground.setPosition(0, 0);
	textureBackground.setSize(sf::Vector2f(textureWidth, textureHeight));

	sf::RectangleShape rect;
	rect.setPosition(0, 0);
	rect.setSize(sf::Vector2f(textureWidth, textureHeight));
	rect.setTexture(GenerateAtlas());

	renderer.draw(textureBackground);
	renderer.draw(rect);

	renderer.display();
}

sf::Texture* AtlasCreatorWindow::GenerateAtlas()
{
	sf::RenderTexture target;
	target.create(atlasWidth, atlasHeight);
	target.clear(sf::Color(255, 255, 255, 0));

	if (atlasWidth <= 0 || atlasHeight <= 0 || texturesPerWidth <= 0 || texturesPerHeight <= 0)
	{
		return NULL;
	}

	float texWidth = atlasWidth / (float)texturesPerWidth;
	float texHeight = atlasHeight / (float)texturesPerHeight;

	int x = 0;
	int y = 0;
	bool inversed = Button_GetCheck(GetDlgItem(atlasCreatorWindow, IDC_INVERSED)) == BST_CHECKED;
	int index = inversed ? (int)texturesList->GetLoadedTextures()->size() - 1 : 0;
	for (
		auto iter = inversed ? --texturesList->GetLoadedTextures()->end() : texturesList->GetLoadedTextures()->begin();
		inversed ? true : (iter != texturesList->GetLoadedTextures()->end());
		(inversed ? iter-- : iter++), (inversed ? index-- : index++))
	{
		if (index < startFrame || index > endFrame)
		{
			continue;
		}

		sf::RectangleShape rect;
		rect.setPosition(x * texWidth, y * texHeight);
		rect.setSize(sf::Vector2f(texWidth, texHeight));
		rect.setTexture(&iter->texture, true);
		rect.setTextureRect(iter->cropRect);

		target.draw(rect);

		if (Button_GetCheck(GetDlgItem(atlasCreatorWindow, IDC_BYROWS)) == BST_CHECKED)
		{
			x++;
			if (x >= texturesPerWidth)
			{
				x = 0;
				y++;
				if (y >= texturesPerHeight)
				{
					break;
				}
			}
		}
		else
		{
			y++;
			if (y >= texturesPerHeight)
			{
				y = 0;
				x++;
				if (x >= texturesPerWidth)
				{
					break;
				}
			}
		}

		if (inversed && iter == texturesList->GetLoadedTextures()->begin())
		{
			break;
		}
	}

	target.display();

	sf::Texture* texture;
	texture = new sf::Texture(target.getTexture());
	return texture;
}

void AtlasCreatorWindow::Open(TexturesList* texturesList)
{
	singleton = new AtlasCreatorWindow(texturesList);

	if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATLASCREATOR), Application::GetMainWindowHandle(), (DLGPROC)AtlasCreator_WndProc) == IDC_OK)
	{
		SaveTexture(generatedAtlas);
	}
}

void AtlasCreatorWindow::UpdatePreview()
{
	RECT clientRect;
	GetClientRect(GetDlgItem(atlasCreatorWindow, IDC_PREVIEW), &clientRect);

	int max = clientRect.bottom - clientRect.top;

	float aspectRatio = atlasWidth / (float)atlasHeight;

	int sizeX, sizeY;

	if (aspectRatio > 1.0f)
	{
		sizeX = max;
		sizeY = (int)(max / aspectRatio);
	}
	else
	{
		sizeY = max;
		sizeX = (int)(aspectRatio * max);
	}

	textureWidth = (float)sizeX;
	textureHeight = (float)sizeY;

	InvalidateRect(atlasCreatorWindow, NULL, true);
}

void AtlasCreatorWindow::SaveTexture(sf::Texture* texture)
{
	OPENFILENAME fileName = {};
	wchar_t path[256] = {};
	fileName.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
	fileName.hInstance = (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE);
	fileName.hwndOwner = Application::GetMainWindowHandle();
	fileName.lpstrDefExt = L"png";
	fileName.lpstrInitialDir = L"C:\\";
	fileName.lpstrTitle = L"Save atlas";
	fileName.lStructSize = sizeof(OPENFILENAME);
	fileName.nMaxFile = sizeof(fileName);
	fileName.nMaxFileTitle = 0;
	fileName.lpstrFile = path;
	fileName.lpstrFilter = GetFileFilter();
	
	if (!GetSaveFileName(&fileName))
	{
		MessageBox(NULL, L"Failed saveing image", L"Error", MB_OK);

		return;
	}

	sf::Image image;
	image = texture->copyToImage();

	sf::String savePath = path;
	image.saveToFile(savePath);
}