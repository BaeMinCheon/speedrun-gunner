#include "WinMain.h"

HWND g_hwnd = nullptr;
SpeedrunGunner* g_pGame = nullptr;

int WINAPI WinMain(HINSTANCE hIstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	MSG msg;
	g_pGame = new SpeedrunGunner();

	if (!createMainWindow(&g_hwnd, hIstance, nCmdShow))
	{
		return 1;
	}

	try
	{
		g_pGame->initialize(g_hwnd);

		for (;;)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				g_pGame->run(g_hwnd);
			}
		}

		SAFE_DELETE(g_pGame);
		return msg.wParam;
	}
	catch (const GameError& e)
	{
		g_pGame->deleteAll();
		DestroyWindow(g_hwnd);
		MessageBox(nullptr, e.getMessage(), "ERROR", MB_OK);
	}
	catch (...)
	{
		g_pGame->deleteAll();
		DestroyWindow(g_hwnd);
		MessageBox(nullptr, "Unknown Error Occured", "ERROR", MB_OK);
	}

	return 0;
}

bool createMainWindow(HWND* p_hwnd, HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WinProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = nullptr;
	wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcx.lpszMenuName = nullptr;
	wcx.lpszClassName = CLASS_NAME;
	wcx.hIconSm = nullptr;

	if (!RegisterClassEx(&wcx))
	{
		return false;
	}

	DWORD style = WS_OVERLAPPEDWINDOW;
	if (IS_FULLSCREEN)
	{
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	}

	*p_hwnd = CreateWindow
	(
		CLASS_NAME,
		GAME_TITLE,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		GAME_WIDTH,
		GAME_HEIGHT,
		static_cast<HWND>(nullptr),
		static_cast<HMENU>(nullptr),
		hInstance,
		static_cast<LPVOID>(nullptr)
	);
	if (!(*p_hwnd))
	{
		return false;
	}

	if (IS_FULLSCREEN)
	{
		RECT clientRect;
		GetClientRect(*p_hwnd, &clientRect);
		MoveWindow
		(
			*p_hwnd,
			0,
			0,
			GAME_WIDTH + (GAME_WIDTH - clientRect.right),
			GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),
			true
		);
	}

	ShowWindow(*p_hwnd, nCmdShow);
	return true;
}

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_pGame->messageHandle(hwnd, msg, wParam, lParam);
}