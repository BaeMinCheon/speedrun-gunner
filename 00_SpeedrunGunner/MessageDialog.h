#pragma once

#ifndef _MESSAGEDIALOG_H
#define _MESSAGEDIALOG_H
#define WIN32_LEAN_AND_MEAN

#include <string>

#include "Constants.h"
#include "TextDX.h"
#include "Graphics.h"
#include "Input.h"

namespace NS_MessageDialog
{
	const UINT			WIDTH = 400;
	const UINT			HEIGHT = 100;
	const UINT			BORDER = 5;
	const UINT			MARGIN = 5;
	const char			FONT[] = "Arial";
	const int			FONT_HEIGHT = 18;
	const COLOR_ARGB	FONT_COLOR = NS_Graphics::WHITE;
	const COLOR_ARGB	BORDER_COLOR = D3DCOLOR_ARGB(192, 192, 192, 192);
	const COLOR_ARGB	BACK_COLOR = SETCOLOR_ARGB(255, 100, 100, 192);
	const UINT			X = GAME_WIDTH / 4;
	const UINT			Y = GAME_HEIGHT / 4;
	const UINT			BUTTON_WIDTH = (UINT)(FONT_HEIGHT * 4.5);
	const UINT			BUTTON_HEIGHT = FONT_HEIGHT + 4;
	const int			MAX_TYPE = 3;
	const int			OK_CANCEL = 0;
	const int			YES_NO = 1;
	const int			JUST_OK = 2;
	static const char*	BUTTON1_TEXT[MAX_TYPE] = { "OK", "YES", "OK" };
	static const char*	BUTTON2_TEXT[MAX_TYPE] = { "CANCEL", "NO", "" };
	const byte			DIALOG_CLOSE_KEY = VK_RETURN;
	const COLOR_ARGB	BUTTON_COLOR = NS_Graphics::GRAY;
	const COLOR_ARGB	BUTTON_FONT_COLOR = NS_Graphics::WHITE;
}

class MessageDialog
{
public:
	MessageDialog();
	virtual ~MessageDialog();

	bool initialize(Graphics*, Input*, HWND);
	void prepareVerts();
	const void draw();

	int getButtonClicked()					{ return m_isButtonClicked; }
	bool getVisible()						{ return m_isVisible; }

	void setFontColor(COLOR_ARGB fc)		{ m_fontColor = fc; }
	void setBorderColor(COLOR_ARGB bc)		{ m_borderColor = bc; }
	void setBackColor(COLOR_ARGB bc)		{ m_backColor = bc; }
	void setButtonColor(COLOR_ARGB bc)		{ m_buttonColor = bc; }
	void setButtonFontColor(COLOR_ARGB bfc) { m_buttonFontColor = bfc; }
	void setButtonClicked(bool b)			{ m_isButtonClicked = b; }
	void setVisible(bool v)					{ m_isVisible = v; }
	void setButtonType(UINT t)
											{
												if (t < NS_MessageDialog::MAX_TYPE)
												{
													m_buttonType = t;
												}
											}

	void print(const std::string*);
	void update();
	void onLostDevice();
	void onResetDevice();

private:
	Graphics*		m_pGraphics;
	Input*			m_pInput;
	TextDX			m_dxFont;
	float			m_x;
	float			m_y;
	UINT			m_height;
	UINT			m_width;
	std::string		m_string;
	RECT			m_textRect;
	RECT			m_leftButtonRect;
	RECT			m_rightButtonRect;
	COLOR_ARGB		m_fontColor;
	COLOR_ARGB		m_borderColor;
	COLOR_ARGB		m_backColor;
	COLOR_ARGB		m_buttonColor;
	COLOR_ARGB		m_buttonFontColor;
	VertexC			m_vtx[4];
	LP_VERTEXBUFFER m_dialogVerts;
	LP_VERTEXBUFFER m_borderVerts;
	LP_VERTEXBUFFER m_leftButtonVerts;
	LP_VERTEXBUFFER m_rightButtonVerts;
	int				m_isButtonClicked;
	int				m_buttonType;
	bool			m_isInitialized;
	bool			m_isVisible;
	HWND			m_hwnd;
	float			m_screenRatioX;
	float			m_screenRatioY;
};

#endif