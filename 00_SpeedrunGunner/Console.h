#pragma once

#ifndef _CONSOLE_H
#define _CONSOLE_H
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <deque>
#include "Constants.h"
#include "Graphics.h"
#include "Input.h"
#include "TextDX.h"

namespace NS_Console
{
	const UINT WIDTH = 500;
	const UINT HEIGHT = 400;
	const UINT X = 5;
	const UINT Y = 5;
	const UINT MARGIN = 4;
	const static char* FONT = "Courier New";
	const int FONT_HEIGHT = 14;
	const COLOR_ARGB FONT_COLOR = NS_Graphics::WHITE;
	const COLOR_ARGB BACK_COLOR = SETCOLOR_ARGB(192, 128, 128, 128);
	const int MAX_LINES = 256;
}

class Console
{
public:
	Console();
	virtual ~Console();

	bool initialize(Graphics*, Input*);
	void draw();
	void showHide();
	void print(const std::string*);
	
	bool getVisible()					{ return m_isVisible; }
	void makeShow()						{ m_isVisible = true; }
	void makeHide()						{ m_isVisible = false; }
	std::string getCommand();
	std::string getInput()				{ return m_inputStr; }

	void clearInput()					{ m_inputStr = ""; }
	void onLostDevice();
	void onResetDevice();

private:
	Graphics*					m_pGraphics;
	Input*						m_pInput;
	TextDX*						m_pFont;
	float						m_x;
	float						m_y;
	int							m_rows;
	std::string					m_commandStr;
	std::string					m_inputStr;
	std::deque<std::string>		m_textQueue;
	RECT						m_rect;
	COLOR_ARGB					m_fontColor;
	COLOR_ARGB					m_backColor;
	VertexC						m_vertices[4];
	LP_VERTEXBUFFER				m_vertexBuffer;
	int							m_scrollAmount;
	bool						m_isInitialized;
	bool						m_isVisible;
};

#endif