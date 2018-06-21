#include "Game.h"

Game::Game()
	: m_pGraphics{ nullptr }, m_pInput{ new Input() },
	m_isPaused{ false }, m_isInitialized{ false }, m_pConsole{ nullptr },
	m_pFont{ nullptr }, m_isFPSon{ false }, m_pMessageDialog{ nullptr }
{
	;
}

Game::~Game()
{
	deleteAll();
	ShowCursor(true);
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

LRESULT Game::messageHandle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_isInitialized)
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			m_pInput->keyDown(wParam);
			return 0;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			m_pInput->keyUp(wParam);
			return 0;
		case WM_CHAR:
			m_pInput->keyIn(wParam);
			return 0;

		case WM_MOUSEMOVE:
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_INPUT:
			m_pInput->mouseRawIn(lParam);
			return 0;
		case WM_LBUTTONDOWN:
			m_pInput->setMouseLButton(true);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_LBUTTONUP:
			m_pInput->setMouseLButton(false);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_MBUTTONDOWN:
			m_pInput->setMouseMButton(true);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_MBUTTONUP:
			m_pInput->setMouseMButton(false);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_RBUTTONDOWN:
			m_pInput->setMouseRButton(true);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_RBUTTONUP:
			m_pInput->setMouseRButton(false);
			m_pInput->mouseIn(lParam);
			return 0;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			m_pInput->setMouseXButton(lParam);
			m_pInput->mouseIn(lParam);
			return 0;

		case WM_DEVICECHANGE:
			m_pInput->checkController();
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Game::initialize(HWND hwnd)
{
	m_hwnd = hwnd;
	
	m_pGraphics = new Graphics();
	m_pGraphics->initialize(m_hwnd, GAME_WIDTH, GAME_HEIGHT, IS_FULLSCREEN);

	m_pInput->initialize(m_hwnd, false);

	m_pFont = new TextDX();
	if (!m_pFont->initialize(m_pGraphics, NS_Game::POINT_SIZE, false, false,
		&std::string(NS_Game::FONT)))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing directx text"));
	}
	m_pFont->setFontColor(NS_Game::FONT_COLOR);

	m_pConsole = new Console();
	m_pConsole->initialize(m_pGraphics, m_pInput);
	m_pConsole->print(&std::string("--- CONSOLE ---"));

	m_pMessageDialog = new MessageDialog();
	m_pMessageDialog->initialize(m_pGraphics, m_pInput, m_hwnd);

	if (!QueryPerformanceFrequency(&m_timerFreq))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing high resolution timer"));
	}
	QueryPerformanceCounter(&m_timeStart);

	m_isInitialized = true;
}

void Game::run(HWND hwnd)
{
	if (!m_pGraphics)
	{
		return;
	}

	QueryPerformanceCounter(&m_timeEnd);
	m_frameTime = static_cast<float>(m_timeEnd.QuadPart - m_timeStart.QuadPart) /
		static_cast<float>(m_timerFreq.QuadPart);

	if (m_frameTime < MIN_FRAME_TIME)
	{
		m_sleepTime = static_cast<DWORD>((MIN_FRAME_TIME - m_frameTime) * 1000);
		timeBeginPeriod(1);
		Sleep(m_sleepTime);
		timeEndPeriod(1);
		return;
	}

	if (m_frameTime > 0.0f)
	{
		m_FPS = (m_FPS * 0.99f) + (0.01f / m_frameTime);
	}
	if (m_frameTime > MAX_FRAME_TIME)
	{
		m_frameTime = MAX_FRAME_TIME;
	}
	m_timeStart = m_timeEnd;

	if (!m_isPaused)
	{
		update();
		processAI();
		collision();
		m_pInput->vibrateControllers(m_frameTime);
	}
	renderGame();
	
	m_pInput->readController();
	if (m_pInput->wasKeyPressed(CONSOLE_KEY))
	{
		m_pConsole->showHide();
		m_isPaused = m_pConsole->getVisible();
	}
	consoleCommand();
	m_pMessageDialog->update();

	if (m_pInput->isKeyDown(ALT_KEY) && m_pInput->wasKeyPressed(ENTER_KEY))
	{
		setDisplayMode(NS_Graphics::TOGGLE);
	}
	if (m_pInput->isKeyDown(ESC_KEY))
	{
		setDisplayMode(NS_Graphics::WINDOW);
	}
	m_pInput->clear(NS_Input::KEYS_PRESSED);
}

void Game::releaseAll()
{
	SAFE_ON_LOST_DEVICE(m_pConsole);
	m_pFont->onLostDevice();
}

void Game::resetAll()
{
	m_pFont->onResetDevice();
	SAFE_ON_RESET_DEVICE(m_pConsole);
}

void Game::deleteAll()
{
	releaseAll();
	SAFE_DELETE(m_pGraphics);
	SAFE_DELETE(m_pInput);
	SAFE_DELETE(m_pConsole);
	m_isInitialized = false;
}

void Game::renderGame()
{
	const int BUF_SIZE = 20;
	static char buffer[BUF_SIZE] = "";

	if (SUCCEEDED(m_pGraphics->beginScene()))
	{
		render();

		m_pGraphics->beginSprite();
		if (m_isFPSon)
		{
			_snprintf_s(buffer, BUF_SIZE, "FPS %d", static_cast<int>(m_FPS));
			m_pFont->print(&std::string(buffer), GAME_WIDTH - 100, GAME_HEIGHT - 28);
		}
		m_pGraphics->endSprite();
		
		m_pConsole->draw();
		m_pMessageDialog->draw();

		m_pGraphics->endScene();
	}

	handleLostGraphicsDevice();
	m_pGraphics->showBackBuffer();
}

void Game::handleLostGraphicsDevice()
{
	m_HR = m_pGraphics->getDeviceState();
	
	if (FAILED(m_HR))
	{
		if (m_HR == D3DERR_DEVICELOST)
		{
			Sleep(100);
			return;
		}
		else if (m_HR == D3DERR_DEVICENOTRESET)
		{
			releaseAll();
			m_HR = m_pGraphics->reset();
			
			if (FAILED(m_HR))
			{
				return;
			}

			resetAll();
		}
	}
}

void Game::setDisplayMode(NS_Graphics::DISPLAY_MOE mode)
{
	releaseAll();
	m_pGraphics->changeDisplayMode(mode);
	resetAll();
}

void Game::consoleCommand()
{
	m_commandStr = m_pConsole->getCommand();
	if (m_commandStr == "")
	{
		return;
	}

	if (m_commandStr == "help")
	{
		m_pConsole->print(&std::string("Console Commands : "));
		m_pConsole->print(&std::string("FPS : toggle display of Frames Per Second"));
		return;
	}

	if (m_commandStr == "FPS")
	{
		m_isFPSon = !m_isFPSon;

		if (m_isFPSon)
		{
			m_pConsole->print(&std::string("FPS on"));
		}
		else
		{
			m_pConsole->print(&std::string("FPS off"));
		}
	}
}