#pragma once

#ifndef _TEXT_H
#define _TEXT_H
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "Image.h"
#include "Constants.h"

namespace NS_Text
{
	enum Alignment
	{
		LEFT,
		RIGHT,
		CENTER,
		CENTER_MIDDLE,
		CENTER_BOTTOM,
		LEFT_BOTTOM,
		RIGHT_BOTTOM
	};

	struct FontData
	{
		UINT left;
		UINT right;
	};

	const int FONT_BORDER = 3;
	const int FONT_WIDTH = 48;
	const int FONT_HEIGHT = 62;
	const int GRID_WIDTH = FONT_WIDTH + FONT_BORDER;
	const int GRID_HEIGHT = FONT_HEIGHT + FONT_BORDER;
	const int COLUMNS = 16;
	const int ROWS = 14;
	const int FRAMES = 1;
	const double ANIM_DELAY = 0.0;
	const int MAX_FONT_HEIGHT = 1000;
	const int MIN_CHAR = 0x0020;
	const int MAX_CHAR = 0x00FF;
	const int PROPORTIONAL_SPACING = 5;
	const int TAB_SIZE = 8;
	const char UNDERLINE = '_';
	const char SOLID = 0x7F;
	const int BOLD_SIZE = 4;
}

class Text : public Image
{
public:
	Text();
	virtual ~Text();

	virtual bool initialize(Graphics*, const char*);
	virtual void update(float) {};
	virtual void onLostDevice();
	virtual void onResetDevice();

	virtual void setXY(int x, int y);

	virtual void print(const std::string*);
	virtual void print(const std::string*, int, int);
	virtual void print(const std::string*, int, int, NS_Text::Alignment);

	virtual void getWidthHeight(const std::string*, UINT*, UINT*);
	virtual void setFontColor(COLOR_ARGB c)			{ m_color = c; }
	virtual COLOR_ARGB getFontColor()				{ return m_color; }
	virtual void setBackColor(COLOR_ARGB bc)		{ m_backColor = bc; }
	virtual COLOR_ARGB getBackColor()				{ return m_backColor; }
	virtual void setFontHeight(UINT height)
													{
														if (height && height <= NS_Text::MAX_FONT_HEIGHT)
														{
															m_fontHeight = height;
															m_spriteData.scale = static_cast<float>(height)
																/ static_cast<float>(NS_Text::FONT_HEIGHT);
														}
													}
	virtual UINT getFontHeight()					{ return m_fontHeight; }
	virtual void setProportional(bool p)			{ m_isProportional = p; }
	virtual bool getProportional()					{ return m_isProportional; }
	virtual void setProportionalSpacing(UINT s)		{ m_proportionalSpacing = s; }
	virtual UINT getProportionalSpacing()			{ return m_proportionalSpacing; }
	virtual void setUnderline(bool u)				{ m_isUnderline = u; }
	virtual bool getUnderline()						{ return m_isUnderline; }
	virtual void setBold(bool b)					{ m_isBold = b; }
	virtual bool getBold()							{ return m_isBold; }
	virtual void setTabSize(UINT size)
													{
														if (size > 0)
														{
															m_tabSize = size;
														}
													}
	virtual UINT getTabSize()						{ return m_tabSize; }

	virtual void doAlign(const std::string*);
	virtual void drawChar(UCHAR);

private:
	TextureManager			m_fontTM;
	LP_TEXTURE				m_fontTexture;        
	char*					m_fontFile;            
	Graphics*				m_pGraphics;          
	UINT					m_width;
	UINT					m_height;             
	NS_Text::FontData		m_fontData[NS_Text::ROWS][NS_Text::COLUMNS]; 
	COLOR_ARGB				m_color;             
	COLOR_ARGB				m_backColor;          
	UINT					m_fontHeight;             
	UINT					m_tabSize;                  
	UINT					m_proportionalSpacing;      
	bool					m_isProportional;              
	bool					m_isUnderline;
	bool					m_isBold;
	NS_Text::Alignment		m_align;
};

#endif