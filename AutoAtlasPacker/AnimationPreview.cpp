#include "stdafx.h"
#include "AnimationPreview.h"
#include "AnimationPreviewResource.h"
#include "Application.h"

AnimationPreview* AnimationPreview::singleton;

AnimationPreview::AnimationPreview(TexturesList* texturesList)
{
	currentFrame = 0;

	frames = new SourceTexture[texturesList->GetLoadedTextures()->size()];
	framesCount = (int)texturesList->GetLoadedTextures()->size();

	int index = 0;
	for (auto iter = texturesList->GetLoadedTextures()->begin(); iter != texturesList->GetLoadedTextures()->end(); iter++, index++)
	{
		frames[index] = *iter;
	}

	startFrame = 0;
	endFrame = framesCount - 1;
}

void AnimationPreview::Open(TexturesList* texturesList)
{
	singleton = new AnimationPreview(texturesList);

	singleton->animationPreview = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ANIMATIONPREVIEW), Application::GetMainWindowHandle(), (DLGPROC)AnimationPreview_WndProc);
	ShowWindow(singleton->animationPreview, SW_SHOW);
}

void AnimationPreview::UpdatePreview()
{
	RECT clientRect;
	GetClientRect(GetDlgItem(animationPreview, IDC_PREVIEW), &clientRect);

	int max = clientRect.bottom - clientRect.top;

	textureWidth = frames[currentFrame].cropRect.width;
	textureHeight = frames[currentFrame].cropRect.height;

	float k = max / (float)textureWidth;
	textureWidth = (int)(k * textureWidth);
	textureHeight = (int)(k * textureHeight);

	if (textureHeight > max)
	{
		float k = max / (float)textureHeight;
		textureWidth = (int)(textureWidth * k);
		textureHeight = (int)(textureHeight * k);
	}

	totalTime += timer.getElapsedTime().asSeconds();
	currentFrame = (int)(totalTime * FPS) % (endFrame - startFrame + 1);
	if (Button_GetCheck(GetDlgItem(animationPreview, IDC_INVERSED)) == BST_CHECKED)
	{
		currentFrame = endFrame - currentFrame;
	}
	else
	{
		currentFrame += startFrame;
	}
	timer.restart();

	InvalidateRect(animationPreview, NULL, true);
}

void AnimationPreview::Render()
{
	renderer.clear();

	sf::RectangleShape rect;
	rect.setPosition(0, 0);
	rect.setSize(sf::Vector2f((float)textureWidth, (float)textureHeight));
	rect.setTexture(&frames[currentFrame].texture);
	rect.setTextureRect(frames[currentFrame].cropRect);

	renderer.draw(rect);

	renderer.display();
}

BOOL CALLBACK AnimationPreview::AnimationPreview_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			singleton->animationPreview = hWnd;

			{
				wchar_t text[256] = {};
				_itow_s(singleton->FPS, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_FPS), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->startFrame, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_STARTFRAME), text);
			}

			{
				wchar_t text[256] = {};
				_itow_s(singleton->endFrame, text, 255, 10);
				Edit_SetText(GetDlgItem(hWnd, IDC_ENDFRAME), text);
			}

			singleton->renderer.create(GetDlgItem(hWnd, IDC_PREVIEW));

			singleton->timer.restart();

			singleton->UpdatePreview();
		}
		return true;

		case WM_PAINT:
		{
			singleton->Render();
		}
		break;

		case WM_COMMAND:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				if (LOWORD(wParam) == IDC_FPS)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_FPS), text, 255);
					singleton->FPS = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_STARTFRAME)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_STARTFRAME), text, 255);
					singleton->startFrame = _wtoi(text);
				}

				if (LOWORD(wParam) == IDC_ENDFRAME)
				{
					wchar_t text[256] = {};
					Edit_GetText(GetDlgItem(hWnd, IDC_ENDFRAME), text, 255);
					singleton->endFrame = _wtoi(text);
				}
			}

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (LOWORD(wParam) == IDOK)
				{
					DestroyWindow(hWnd);
					delete singleton;
					singleton = NULL;
					return true;
				}
			}
		}
		return true;
	}

	return false;
}

void AnimationPreview::Update()
{
	if (singleton != NULL)
	{
		singleton->UpdatePreview();
	}
}

AnimationPreview::~AnimationPreview()
{
	delete[] frames;
}