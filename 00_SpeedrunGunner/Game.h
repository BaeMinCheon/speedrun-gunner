#pragma once

#ifndef _GAME_H
#define _GAME_H
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <mmsystem.h>
#include "Constants.h"
#include "GameError.h"
#include "Graphics.h"
#include "Input.h"
#include "TextDX.h"
#include "Console.h"
#include "MessageDialog.h"

namespace NS_Game
{
	const static char* FONT = "Arial Bold";
	const int POINT_SIZE = 20;
	const COLOR_ARGB FONT_COLOR = SETCOLOR_ARGB(255, 255, 255, 255);
}

class Game
{
public:
	Game();
	virtual ~Game();

	LRESULT messageHandle(HWND, UINT, WPARAM, LPARAM);
	virtual void initialize(HWND);
	virtual void run(HWND);
	virtual void releaseAll();
	virtual void resetAll();
	virtual void deleteAll();
	virtual void renderGame();
	virtual void handleLostGraphicsDevice();

	void setDisplayMode(NS_Graphics::DISPLAY_MOE = NS_Graphics::TOGGLE);
	virtual void consoleCommand();

	Graphics* getGraphics()				{ return m_pGraphics; }
	Input* getInput()					{ return m_pInput; }
	void exitGame()						{ PostMessage(m_hwnd, WM_DESTROY, 0, 0); }

	virtual void update() = 0;
	virtual void processAI() = 0;
	virtual void collision() = 0;
	virtual void render() = 0;

protected:
	Graphics*		m_pGraphics;
	Input*			m_pInput;
	HWND			m_hwnd;
	HRESULT			m_HR;
	LARGE_INTEGER	m_timeStart;
	LARGE_INTEGER	m_timeEnd;
	LARGE_INTEGER	m_timerFreq;
	float			m_frameTime;

	Console*		m_pConsole;
	std::string		m_commandStr;
	TextDX*			m_pFont;
	float			m_FPS;
	bool			m_isFPSon;

	MessageDialog*	m_pMessageDialog;

	DWORD			m_sleepTime;
	bool			m_isPaused;
	bool			m_isInitialized;
};

#endif