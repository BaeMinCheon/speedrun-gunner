#pragma once

#ifndef _TEXTDX_H
#define _TEXTDX_H
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Constants.h"
#include "Graphics.h"

class TextDX
{
public:
	TextDX();
	virtual ~TextDX();

	virtual bool initialize(Graphics*, int, bool, bool, const std::string*);
	virtual int print(const std::string*, int, int);
	virtual int print(const std::string*, RECT*, UINT);

	virtual float getDegrees()					{ return m_angle * 180.0f / PI; }
	virtual float getRadians()					{ return m_angle; }
	virtual COLOR_ARGB getFontColor()			{ return m_color; }
	
	virtual void setDegrees(float d)			{ m_angle = d * PI / 180.0f; }
	virtual void setRadians(float r)			{ m_angle = r; }
	virtual void setFontColor(COLOR_ARGB c)		{ m_color = c; }
	
	virtual void onLostDevice();
	virtual void onResetDevice();

private:
	Graphics*		m_pGraphics;
	COLOR_ARGB		m_color;
	LP_DXFONT		m_font;
	RECT			m_rect;
	D3DXMATRIX		m_matrix;
	float			m_angle;
};

#endif