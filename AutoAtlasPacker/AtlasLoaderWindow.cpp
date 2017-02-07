#include "stdafx.h"
#include "AtlasLoaderWindow.h"
#include "AtlasLoaderResource.h"
#include "Application.h"
#include "TextureUtility.h"

AtlasLoaderWindow* AtlasLoaderWindow::singleton;

AtlasLoaderWindow::AtlasLoaderWindow(TexturesList* texturesList)
{
	this->texturesList = texturesList;

	OPENFILENAME ofn = {};
	wchar_t fileName[256] = {};
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.hwndOwner = Application::GetMainWindowHandle();
	ofn.lpstrDefExt = L"png";
	ofn.lpstrFile = fileName;
	ofn.lpstrInitialDir = L"C:\\";
	ofn.lpstrFilter = GetFileFilter();
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = sizeof(fileName);
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = L"Load atlas";

	if (!GetOpenFileName(&ofn))
	{
		return;
	}

	sf::String path = fileName;
	atlasTexture.loadFromFile(path);

	loaded = true;
}

void AtlasLoaderWindow::Open(TexturesList* texturesList)
{
	singleton = new AtlasLoaderWindow(texturesList);

	if (singleton->loaded)
	{
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATLASLOADER), Application::GetMainWindowHandle(), (DLGPROC)AtlasLoader_WndProc);
	}
	else
	{
		delete singleton;
	}
}

void AtlasLoaderWindow::GenerateTextures()
{
	if (texturesPerWidth <= 0 || texturesPerHeight <= 0)
	{
		MessageBox(NULL, L"Failed creating textures", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	int width = atlasTexture.getSize().x / texturesPerWidth;
	int height = atlasTexture.getSize().y / texturesPerHeight;

	bool inversed = Button_GetCheck(GetDlgItem(atlasLoaderWindow, IDC_INVERSED)) == BST_CHECKED;

	int x = inversed ? texturesPerWidth - 1 : 0;
	int y = inversed ? texturesPerHeight - 1 : 0;

	int index = 0;
	while (true)
	{
		sf::RenderTexture target;
		target.create(width, height);
		target.clear(sf::Color(255, 255, 255, 0));

		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f((float)width, (float)height));
		rect.setTexture(&atlasTexture);
		rect.setTextureRect(sf::IntRect(x * width, y * height, width, height));

		target.draw(rect);
		target.display();

		sf::Texture* texture = new sf::Texture(target.getTexture());
		texturesList->AddTexture(texture);

		if (Button_GetCheck(GetDlgItem(atlasLoaderWindow, IDC_BYROWS)) == BST_CHECKED)
		{
			inversed ? x-- : x++;
			if (inversed ? x < 0 : x >= texturesPerWidth)
			{
				x = inversed ? texturesPerWidth - 1 : 0;

				inversed ? y-- : y++;
				if (inversed ? y < 0 : y >= texturesPerHeight)
				{
					break;
				}
			}
		}
		else
		{
			inversed ? y-- : y++;
			if (inversed ? y < 0 : y >= texturesPerHeight)
			{
				y = inversed ? texturesPerHeight - 1 : 0;

				inversed ? x-- : x++;
				if (inversed ? x < 0 : x >= texturesPerWidth)
				{
					break;
				}
			}
		}

		index++;
		if (index >= totalCount)
		{
			break;
		}
	}
}

void AtlasLoaderWindow::UpdatePreview()
{
	RECT clientRect;
	GetClientRect(GetDlgItem(atlasLoaderWindow, IDC_PREVIEW), &clientRect);

	int max = clientRect.bottom - clientRect.top;

	textureWidth = atlasTexture.getSize().x;
	textureHeight = atlasTexture.getSize().y;

	float k = max / (float)textureWidth;
	textureWidth = (int)(k * textureWidth);
	textureHeight = (int)(k * textureHeight);

	if (textureHeight > max)
	{
		float k = max / (float)textureHeight;
		textureHeight = (int)(k * textureHeight);
		textureWidth = (int)(k * textureWidth);
	}

	InvalidateRect(atlasLoaderWindow, NULL, true);
}

void AtlasLoaderWindow::Render()
{
	renderer.clear();

	sf::RectangleShape rect;
	rect.setPosition(0, 0);
	rect.setSize(sf::Vector2f((float)textureWidth, (float)textureHeight));
	rect.setTexture(&atlasTexture, true);

	renderer.draw(rect);

	float width = textureWidth / (float)texturesPerWidth;
	float height = textureHeight / (float)texturesPerHeight;

	bool inversed = Button_GetCheck(GetDlgItem(atlasLoaderWindow, IDC_INVERSED)) == BST_CHECKED;

	int index = 0;
	int x = inversed ? texturesPerWidth - 1 : 0;
	int y = inversed ? texturesPerHeight - 1 : 0;

	while (true)
	{
		sf::RectangleShape rect;
		rect.setPosition(x * width, y * height);
		rect.setSize(sf::Vector2f(width, height));
		rect.setOutlineColor(sf::Color(255, 0, 0, 125));
		rect.setOutlineThickness(0.5f);
		rect.setFillColor(sf::Color(255, 255, 255, 0));

		if (index >= totalCount)
		{
			rect.setFillColor(sf::Color(255, 0, 0, 255));
		}

		renderer.draw(rect);

		if (Button_GetCheck(GetDlgItem(atlasLoaderWindow, IDC_BYROWS)) == BST_CHECKED)
		{
			inversed ? x-- : x++;
			if (inversed ? x < 0 : x >= texturesPerWidth)
			{
				x = inversed ? texturesPerWidth - 1 : 0;

				inversed ? y-- : y++;
				if (inversed ? y < 0 : y >= texturesPerHeight)
				{
					break;
				}
			}
		}
		else
		{
			inversed ? y-- : y++;
			if (inversed ? y < 0 : y >= texturesPerHeight)
			{
				y = inversed ? texturesPerHeight - 1 : 0;

				inversed ? x-- : x++;
				if (inversed ? x < 0 : x >= texturesPerWidth)
				{
					break;
				}
			}
		}

		index++;
	}

	renderer.display();
}

BOOL CALLBACK AtlasLoaderWindow::AtlasLoader_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			singleton->atlasLoaderWindow = hWnd;
			singleton->renderer.create(GetDlgItem(hWnd, IDC_PREVIEW));

			{
				wchar_t text[256] = {};
				_itow_s(singleton->texturesPerWidth, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_TEXPERHORIZONTAL), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->texturesPerHeight, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_TEXPERVERTICAL), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->totalCount, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_TOTALCOUNT), text);
			}

			singleton->UpdatePreview();
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
			if (HIWORD(wParam) == EN_CHANGE)
			{
				if (LOWORD(wParam) == IDC_TEXPERHORIZONTAL)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_TEXPERHORIZONTAL), text, 255);

					singleton->texturesPerWidth = _wtoi(text);
					singleton->totalCount = singleton->texturesPerWidth * singleton->texturesPerHeight;

					{
						wchar_t text[256] = {};
						_itow_s(singleton->totalCount, text, 255, 10);
						Edit_SetText(GetDlgItem(hWnd, IDC_TOTALCOUNT), text);
					}

					singleton->UpdatePreview();
				}

				if (LOWORD(wParam) == IDC_TEXPERVERTICAL)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_TEXPERVERTICAL), text, 255);

					singleton->texturesPerHeight = _wtoi(text);
					singleton->totalCount = singleton->texturesPerWidth * singleton->texturesPerHeight;

					{
						wchar_t text[256] = {};
						_itow_s(singleton->totalCount, text, 255, 10);
						Edit_SetText(GetDlgItem(hWnd, IDC_TOTALCOUNT), text);
					}

					singleton->UpdatePreview();
				}

				if (LOWORD(wParam) == IDC_TOTALCOUNT)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_TOTALCOUNT), text, 255);

					singleton->totalCount = _wtoi(text);

					singleton->UpdatePreview();
				}
			}

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (LOWORD(wParam) == IDOK)
				{
					singleton->GenerateTextures();
					EndDialog(hWnd, wParam);
					delete singleton;
					singleton = NULL;
					return true;
				}

				if (LOWORD(wParam) == IDCANCEL)
				{
					EndDialog(hWnd, wParam);
					delete singleton;
					singleton = NULL;
					return false;
				}

				if (LOWORD(wParam) == IDC_BYROWS ||
					LOWORD(wParam) == IDC_BYCOLUMNS ||
					LOWORD(wParam) == IDC_INVERSED)
				{
					singleton->UpdatePreview();
				}
			}
		}
		return true;
	}

	return false;
}