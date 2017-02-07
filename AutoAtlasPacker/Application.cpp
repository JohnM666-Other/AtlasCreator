#include "stdafx.h"
#include "Application.h"
#include "AtlasCreatorWindow.h"
#include "AtlasLoaderWindow.h"
#include "AnimationPreview.h"
#include "IconResource.h"

Application* Application::singleton;
UINT Application::SCREEN_WIDTH = 1280;
UINT Application::SCREEN_HEIGHT = 720;

int Application::Run(HINSTANCE hInstance)
{
	singleton = this;

	Init(hInstance);

	MSG  msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			deltaTime = timer.getElapsedTime().asSeconds();
			timer.restart();

			Render();

			AnimationPreview::Update();

			bool needActiveTextureUpdate;
			texturesList->Update(needActiveTextureUpdate);

			if (needActiveTextureUpdate)
			{
				activeTexture->UpdateActiveTextureSprite();
			}
		}
	}

	UnregisterClass(L"MainWindow", hInstance);

	return 0;
}

void Application::Init(HINSTANCE hInstance)
{
	RECT windowRect = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, true);

	{
		WNDCLASS wnd;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hbrBackground = NULL;
		wnd.hCursor = NULL;
		wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
		wnd.hInstance = hInstance;
		wnd.lpfnWndProc = MainWindow_WndProc;
		wnd.lpszClassName = L"MainWindow";
		wnd.lpszMenuName = NULL;
		wnd.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&wnd);
	}

	mainWindow = CreateWindow(L"MainWindow", L"Auto atlas packer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, HWND_DESKTOP, NULL, hInstance, NULL);

	ShowWindow(mainWindow, SW_SHOW);
	UpdateWindow(mainWindow);

	if (!font.loadFromFile("Arial.ttf"))
	{
		MessageBox(NULL, L"Failed loading font", L"Error", MB_OK);

		exit(1);
	}

	InitMenu();

	InitCommonControls();

	texturesList = new TexturesList(0, 0, (int)(SCREEN_WIDTH * 0.15f), SCREEN_HEIGHT);
	activeTexture = new ActiveTexture((int)(SCREEN_WIDTH * 0.15f), 0, (int)(SCREEN_WIDTH * 0.85f), SCREEN_HEIGHT, mainWindow, texturesList);
}

void Application::InitMenu()
{
	menu = CreateMenu();
	HMENU filePopupMenu = CreatePopupMenu();

	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT_PTR)filePopupMenu, L"File");
	
	AppendMenu(filePopupMenu, MF_STRING, ID_LOADIMAGES, L"Load images");
	AppendMenu(filePopupMenu, MF_STRING, ID_LOADATLAS, L"Load atlas");
	AppendMenu(filePopupMenu, MF_STRING, ID_SAVEIMAGES, L"Save images");
	AppendMenu(filePopupMenu, MF_STRING, ID_SAVEATLAS, L"Save atlas");
	AppendMenu(filePopupMenu, MF_SEPARATOR, 0, L"Seprator");
	AppendMenu(filePopupMenu, MF_STRING, ID_ANIMATION, L"Animation");
	AppendMenu(filePopupMenu, MF_STRING, ID_CLEAR, L"Clear");
	AppendMenu(filePopupMenu, MF_SEPARATOR, 0, L"Separator");
	AppendMenu(filePopupMenu, MF_STRING, ID_EXIT, L"Exit");

	SetMenu(mainWindow, menu);

	SetSaveButtonsActive(false);
}

void Application::SetSaveButtonsActive(bool flag)
{
	EnableMenuItem(singleton->menu, ID_SAVEATLAS, flag ? MF_ENABLED : MF_DISABLED);
	EnableMenuItem(singleton->menu, ID_SAVEIMAGES, flag ? MF_ENABLED : MF_DISABLED);
	EnableMenuItem(singleton->menu, ID_ANIMATION, flag ? MF_ENABLED : MF_DISABLED);
}

LRESULT CALLBACK Application::MainWindow_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		SendMessage(singleton->activeTexture->GetParamsPanelHandle(), msg, wParam, lParam);
		break;

	case WM_COMMAND:
		{
			int commandID = LOWORD(wParam);
			if (commandID == ID_LOADIMAGES)
			{
				singleton->texturesList->LoadTextures(GetModuleHandle(NULL));
				singleton->activeTexture->UpdateActiveTextureSprite();
			}
			if (commandID == ID_SAVEATLAS)
			{
				AtlasCreatorWindow::Open(singleton->texturesList);
			}
			if (commandID == ID_CLEAR)
			{
				singleton->texturesList->Clear();
				singleton->activeTexture->UpdateActiveTextureSprite();
				SetSaveButtonsActive(false);
			}
			if (commandID == ID_SAVEIMAGES)
			{
				singleton->texturesList->SaveImages();
			}
			if (commandID == ID_LOADATLAS)
			{
				AtlasLoaderWindow::Open(singleton->texturesList);
				singleton->activeTexture->UpdateActiveTextureSprite();
			}
			if (commandID == ID_ANIMATION)
			{
				AnimationPreview::Open(singleton->texturesList);
			}
			if (commandID == ID_EXIT)
			{
				DestroyWindow(hWnd);
			}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			singleton->texturesList->Scroll((int)(delta * 0.5f));
		}
		break;

	case WM_SIZE:
		SCREEN_WIDTH = LOWORD(lParam);
		SCREEN_HEIGHT = HIWORD(lParam);
		if (singleton->activeTexture != NULL)
		{
			singleton->activeTexture->ResizeWindow((int)(SCREEN_WIDTH * 0.15f), 0, (int)(SCREEN_WIDTH * 0.85f), SCREEN_HEIGHT);
		}
		if (singleton->texturesList != NULL)
		{
			singleton->texturesList->ResizeWindow(0, 0, (int)(SCREEN_WIDTH * 0.15f), SCREEN_HEIGHT);
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Application::Render()
{
	texturesList->Render();
	activeTexture->Render();
}

int Application::GetAtlasSize(std::list<sf::Texture> textures)
{
	int atlasSize = (int)sqrt(textures.size()) + 1;

	return atlasSize;
}

int Application::GetImageSize(int atlasSize)
{
	int totalSize = atlasSize * 256;
	int imageSize = (int)(log(totalSize) / log(2)) + 1;

	return imageSize;
}

sf::Font* Application::GetFont()
{
	return &singleton->font;
}

UINT Application::GetScreenWidth()
{
	return SCREEN_WIDTH;
}

UINT Application::GetScreenHeight()
{
	return SCREEN_HEIGHT;
}

HWND Application::GetMainWindowHandle()
{
	return singleton->mainWindow;
}