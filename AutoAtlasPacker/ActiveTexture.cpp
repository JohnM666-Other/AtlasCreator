#include "stdafx.h"
#include "ActiveTexture.h"
#include "TextureUtility.h"

ActiveTexture::ActiveTexture(UINT left, UINT top, UINT width, UINT height, HWND parent, TexturesList* texturesList)
{
	this->texturesList = texturesList;

	this->width = width;
	this->height = height;
	this->left = left;
	this->top = top;

	zoom = 1.0f;

	paramsPanelRelativeWidth = 0.2f;

	{
		WNDCLASS wnd;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hbrBackground = GetSysColorBrush(COLOR_MENU);
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.hIcon = NULL;
		wnd.hInstance = (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE);
		wnd.lpfnWndProc = ParamsPanel_WndProc;
		wnd.lpszClassName = L"ParamsPanel";
		wnd.lpszMenuName = NULL;
		wnd.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&wnd);
	}

	{
		WNDCLASS wnd;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hbrBackground = GetSysColorBrush(COLOR_MENU);
		wnd.hCursor = NULL;
		wnd.hIcon = NULL;
		wnd.hInstance = (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE);
		wnd.lpfnWndProc = DefWindowProc;
		wnd.lpszClassName = L"ActiveTexture";
		wnd.lpszMenuName = NULL;
		wnd.style = CS_VREDRAW | CS_HREDRAW;
		RegisterClass(&wnd);
	}

	renderField = CreateWindow(
		L"ActiveTexture", L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN,
		left, top, (int)(width * (1.0f - paramsPanelRelativeWidth)), height,
		Application::GetMainWindowHandle(), NULL, NULL, NULL);

	paramsPanel = CreateWindow(
		L"ParamsPanel", L"Properties",
		WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN,
		(int)(left + width * (1 - paramsPanelRelativeWidth)), 0, (int)(width * paramsPanelRelativeWidth), height,
		parent, NULL, NULL, this);

	leftFieldTitle = CreateWindow(
		L"STATIC", L"Left crop:",
		WS_VISIBLE | WS_CHILD,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.07f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, NULL, NULL, NULL);

	leftField = CreateWindowEx(
		0, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.1f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_LEFTFIELD, NULL, NULL);

	topFieldTitle = CreateWindow(
		L"STATIC", L"Top crop:",
		WS_VISIBLE | WS_CHILD,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.16f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, NULL, NULL, NULL);

	topField = CreateWindowEx(
		0, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.19f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_TOPFIELD, NULL, NULL);

	widthFieldTitle = CreateWindow(
		L"STATIC", L"Width:",
		WS_VISIBLE | WS_CHILD,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.25f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, NULL, NULL, NULL);

	widthField = CreateWindowEx(
		0, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.28f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_WIDTHFIELD, NULL, NULL);

	heightFieldTitle = CreateWindow(
		L"STATIC", L"Height:",
		WS_CHILD | WS_VISIBLE,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.34f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, NULL, NULL, NULL);

	heightField = CreateWindowEx(
		0, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.37f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_HEIGHTFIELD, NULL, NULL);

	applyToEverything = CreateWindowEx(
		0, L"BUTTON", L"Apply crop to every texture",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.43f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_APPLYTOEVERYTHING, NULL, NULL);

	optimizeCropRectWindow = CreateWindowEx(
		0, L"BUTTON", L"Optimize crop rect",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.49f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_OPTIMIZECROPRECT, NULL, NULL);

	cropAsAnimation = CreateWindowEx(
		0, L"BUTTON", L"Crop as animation",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.55f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_CROPASANIMATION, NULL, NULL);

	moveToCenter = CreateWindowEx(
		0, L"BUTTON", L"Move crop to center",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.61f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_MOVETOCENTER, NULL, NULL);

	noCrop = CreateWindowEx(
		0, L"BUTTON", L"No crop",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.67f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_NOCROP, NULL, NULL);

	lockCenter = CreateWindowEx(
		0, L"BUTTON", L"Lock center",
		WS_CHILD | WS_VISIBLE | BS_FLAT | BS_CHECKBOX,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.73f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_LOCKCENTER, NULL, NULL);

	saveTexture = CreateWindowEx(
		0, L"BUTTON", L"Save texture",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.79f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_SAVEIMAGE, NULL, NULL);

	deleteTexture = CreateWindowEx(
		0, L"BUTTON", L"Delete texture",
		WS_CHILD | WS_VISIBLE | BS_FLAT,
		(int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.85f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f),
		paramsPanel, (HMENU)ID_DELETETEXTURE, NULL, NULL);

	renderer.create(renderField);

	cropRectWindow = new CropRectWindow(this, 3);

	UpdateViewport();
	UpdateActiveTextureSprite();
}

ActiveTexture::~ActiveTexture()
{
	UnregisterClass(L"ParamsPanel", (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE));
	UnregisterClass(L"ActiveTexture", (HINSTANCE)GetWindowLongPtr(Application::GetMainWindowHandle(), GWLP_HINSTANCE));

	delete cropRectWindow;
}

void ActiveTexture::SetParamsPanelActive(bool flag)
{
	Edit_Enable(leftField, flag);
	Edit_Enable(topField, flag);
	Edit_Enable(widthField, flag);
	Edit_Enable(heightField, flag);
	Button_Enable(applyToEverything, flag);
	Button_Enable(optimizeCropRectWindow, flag);
	Button_Enable(cropAsAnimation, flag);
	Button_Enable(moveToCenter, flag);
	Button_Enable(noCrop, flag);
	Button_Enable(lockCenter, flag);
	Button_Enable(saveTexture, flag);
	Button_Enable(deleteTexture, flag);
}

void ActiveTexture::ResizeWindow(UINT left, UINT top, UINT width, UINT height)
{
	this->width = width;
	this->height = height;
	this->left = left;
	this->top = top;

	SetWindowPos(renderField, HWND_TOP, left, top, (int)(width * (1.0f - paramsPanelRelativeWidth)), height, 0);
	SetWindowPos(paramsPanel, HWND_TOP, (int)(left + width * (1.0f - paramsPanelRelativeWidth)), 0, (int)(width * paramsPanelRelativeWidth), height, 0);
	SetWindowPos(leftFieldTitle, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.07f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(leftField, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.1f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(topFieldTitle, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.16f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(topField, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.19f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(widthFieldTitle, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.25f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(widthField, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.28f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(heightFieldTitle, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.34f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(heightField, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.37f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(applyToEverything, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.43f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(optimizeCropRectWindow, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.49f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(cropAsAnimation, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.55f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(moveToCenter, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.61f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(noCrop, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.67f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(lockCenter, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.73f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(saveTexture, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.79f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);
	SetWindowPos(deleteTexture, HWND_TOP, (int)(width * paramsPanelRelativeWidth * 0.1f), (int)(height * 0.85f), (int)(width * paramsPanelRelativeWidth * 0.8f), (int)(height * 0.03f), 0);

	renderer.setSize(sf::Vector2u((int)(width * (1.0f - paramsPanelRelativeWidth)), height));

	UpdateViewport();
	UpdateActiveTextureSprite();
}

LRESULT CALLBACK ActiveTexture::ParamsPanel_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static std::map<HWND, ActiveTexture*> activeTextureWindows;

	switch (msg)
	{

	case WM_CREATE:
		{
			LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
			activeTextureWindows.insert({ hWnd, (ActiveTexture*)createStruct->lpCreateParams });
		}
		break;

	case WM_COMMAND:
		{
			if (activeTextureWindows[hWnd]->texturesList->GetActiveTexture() == NULL)
			{
				break;
			}

			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (LOWORD(wParam) == ID_APPLYTOEVERYTHING)
				{
					activeTextureWindows[hWnd]->texturesList->ApplyCropForEachTexture(activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->cropRect);
				}

				if (LOWORD(wParam) == ID_OPTIMIZECROPRECT)
				{
					activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->cropRect = GetOptimizedCropRect(&activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->texture);
					activeTextureWindows[hWnd]->UpdateCropRectText();
				}

				if (LOWORD(wParam) == ID_CROPASANIMATION)
				{
					activeTextureWindows[hWnd]->texturesList->CropTexturesAsAnimation();
					activeTextureWindows[hWnd]->UpdateCropRectText();
				}

				if (LOWORD(wParam) == ID_LOCKCENTER)
				{
					if (IsDlgButtonChecked(hWnd, ID_LOCKCENTER) & BST_CHECKED)
					{
						CheckDlgButton(hWnd, ID_LOCKCENTER, BST_UNCHECKED);
						activeTextureWindows[hWnd]->cropRectWindow->SetLockCenter(false);
					}
					else
					{
						CheckDlgButton(hWnd, ID_LOCKCENTER, BST_CHECKED);
						activeTextureWindows[hWnd]->cropRectWindow->SetLockCenter(true);
					}
				}

				if (LOWORD(wParam) == ID_MOVETOCENTER)
				{
					activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->cropRect = MoveCropToCenter(
						activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->cropRect,
						&activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->texture);

					activeTextureWindows[hWnd]->UpdateCropRectText();
				}

				if (LOWORD(wParam) == ID_NOCROP)
				{
					activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->cropRect = NoCrop(
						&activeTextureWindows[hWnd]->texturesList->GetActiveTexture()->texture);

					activeTextureWindows[hWnd]->UpdateCropRectText();
				}

				if (LOWORD(wParam) == ID_SAVEIMAGE)
				{
					activeTextureWindows[hWnd]->texturesList->SaveActiveTexture();
				}

				if (LOWORD(wParam) == ID_DELETETEXTURE)
				{
					activeTextureWindows[hWnd]->texturesList->DeleteActiveTexture();

					activeTextureWindows[hWnd]->UpdateActiveTextureSprite();
				}
			}

			if (HIWORD(wParam) == EN_KILLFOCUS)
			{
				activeTextureWindows[hWnd]->UpdateCropRectText();
			}

			if (LOWORD(wParam) == ID_LEFTFIELD)
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					wchar_t text[10];
					GetWindowText(activeTextureWindows[hWnd]->leftField, text, 9);

					int value;
					value = _wtoi(text);

					SourceTexture* activeTexture = activeTextureWindows[hWnd]->texturesList->GetActiveTexture();

					activeTexture->cropRect.left = value;

					activeTextureWindows[hWnd]->CheckCropRect();

					activeTextureWindows[hWnd]->cropRectWindow->SetRect(activeTexture->cropRect, &activeTextureWindows[hWnd]->activeTextureSprite, activeTextureWindows[hWnd]->zoom);
				}
			}
			if (LOWORD(wParam) == ID_TOPFIELD)
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					wchar_t text[10];
					GetWindowText(activeTextureWindows[hWnd]->topField, text, 9);

					int value;
					value = _wtoi(text);

					SourceTexture* activeTexture = activeTextureWindows[hWnd]->texturesList->GetActiveTexture();

					activeTexture->cropRect.top = value;

					activeTextureWindows[hWnd]->CheckCropRect();

					activeTextureWindows[hWnd]->cropRectWindow->SetRect(activeTexture->cropRect, &activeTextureWindows[hWnd]->activeTextureSprite, activeTextureWindows[hWnd]->zoom);
				}
			}
			if (LOWORD(wParam) == ID_WIDTHFIELD)
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					wchar_t text[10];
					GetWindowText(activeTextureWindows[hWnd]->widthField, text, 9);

					int value;
					value = _wtoi(text);

					SourceTexture* activeTexture = activeTextureWindows[hWnd]->texturesList->GetActiveTexture();

					activeTexture->cropRect.width = value;

					activeTextureWindows[hWnd]->CheckCropRect();

					activeTextureWindows[hWnd]->cropRectWindow->SetRect(activeTexture->cropRect, &activeTextureWindows[hWnd]->activeTextureSprite, activeTextureWindows[hWnd]->zoom);
				}
			}
			if (LOWORD(wParam) == ID_HEIGHTFIELD)
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					wchar_t text[10];
					GetWindowText(activeTextureWindows[hWnd]->heightField, text, 9);

					int value;
					value = _wtoi(text);

					SourceTexture* activeTexture = activeTextureWindows[hWnd]->texturesList->GetActiveTexture();

					activeTexture->cropRect.height = value;

					activeTextureWindows[hWnd]->CheckCropRect();

					activeTextureWindows[hWnd]->cropRectWindow->SetRect(activeTexture->cropRect, &activeTextureWindows[hWnd]->activeTextureSprite, activeTextureWindows[hWnd]->zoom);
				}
			}
		}
		break;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ActiveTexture::Render()
{
	sf::Event event;
	while (renderer.pollEvent(event))
	{
		if (event.type == event.MouseButtonPressed)
		{
			cropRectWindow->OnMouseButtonDown(&renderer, &view);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			cropRectWindow->OnMouseButton(&renderer, &view);
		}
		else if (event.type == event.MouseButtonReleased)
		{
			cropRectWindow->OnMouseButtonUp(&renderer);
		}
		else if (event.type == event.MouseMoved)
		{
			cropRectWindow->OnMouseMove(&renderer, &view);
		}
	}

	renderer.clear();

	if (texturesList->GetActiveTexture() == NULL)
	{
		renderer.display();
		return;
	}

	sf::View oldView = renderer.getView();
	renderer.setView(view);

	renderer.draw(activeTextureSprite);
	cropRectWindow->Render(&renderer, zoom);

	renderer.setView(oldView);

	renderer.display();
}

void ActiveTexture::UpdateActiveTextureSprite()
{
	SourceTexture* activeTexture = texturesList->GetActiveTexture();
	
	if (activeTexture == NULL)
	{
		SetParamsPanelActive(false);

		return;
	}

	SetParamsPanelActive(true);

	activeTextureSprite.setTexture(activeTexture->texture, true);
	activeTextureSprite.setOrigin(activeTexture->texture.getSize().x * 0.5f, activeTexture->texture.getSize().y * 0.5f);

	int width = activeTexture->texture.getSize().x;
	int height = activeTexture->texture.getSize().y;

	zoom = std::min((float)renderer.getSize().x / width, (float)renderer.getSize().y / height) * 0.9f;

	UpdateViewport();

	activeTextureSprite.setPosition(0.0f, 0.0f);

	cropRectWindow->SetRect(activeTexture->cropRect, &activeTextureSprite, zoom);
	UpdateCropRectText();
}

void ActiveTexture::UpdateCropRectText()
{
	SourceTexture* activeTexture = texturesList->GetActiveTexture();

	if (activeTexture == NULL)
	{
		return;
	}

	wchar_t left[10] = {};
	wchar_t top[10] = {};
	wchar_t width[10] = {};
	wchar_t height[10] = {};

	_itow_s(activeTexture->cropRect.left, left, 9, 10);
	_itow_s(activeTexture->cropRect.top, top, 9, 10);
	_itow_s(activeTexture->cropRect.width, width, 9, 10);
	_itow_s(activeTexture->cropRect.height, height, 9, 10);

	SetWindowText(leftField, left);
	SetWindowText(topField, top);
	SetWindowText(widthField, width);
	SetWindowText(heightField, height);
}

void ActiveTexture::UpdateViewport()
{
	RECT windowRect;
	GetClientRect(renderField, &windowRect);

	view.setSize((float)(windowRect.right - windowRect.left) / zoom, (float)(windowRect.bottom - windowRect.top) / zoom);
	view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	view.setCenter(0.0f, 0.0f);
}

HWND ActiveTexture::GetParamsPanelHandle()
{
	return paramsPanel;
}

void ActiveTexture::CheckCropRect()
{
	SourceTexture* activeTexture = texturesList->GetActiveTexture();

	if (activeTexture == NULL)
	{
		return;
	}

	activeTexture->cropRect.left = std::max(0, activeTexture->cropRect.left);
	activeTexture->cropRect.top = std::max(0, activeTexture->cropRect.top);
	activeTexture->cropRect.width = std::min(activeTexture->sprite.getTextureRect().width - activeTexture->cropRect.left, activeTexture->cropRect.width);
	activeTexture->cropRect.height = std::min(activeTexture->sprite.getTextureRect().height - activeTexture->cropRect.top, activeTexture->cropRect.height);
}

void ActiveTexture::SetCropRectDataFromWindow()
{
	texturesList->GetActiveTexture()->cropRect = cropRectWindow->GetRect(&activeTextureSprite);
	UpdateCropRectText();
}

HWND ActiveTexture::GetRenderFieldHandle()
{
	return renderField;
}

sf::Sprite* ActiveTexture::GetActiveTextureSprite()
{
	return &activeTextureSprite;
}