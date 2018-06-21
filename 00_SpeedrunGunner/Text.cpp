#include "Text.h"

Text::Text()
	: Image(), m_fontFile{nullptr}, m_pGraphics{nullptr}
{
	m_color = NS_Graphics::WHITE;
	m_backColor = TRANSCOLOR;            
	m_align = NS_Text::LEFT;
	m_width = NS_Text::GRID_WIDTH - 3;     
	m_height = NS_Text::GRID_HEIGHT - 3;
	m_fontHeight = NS_Text::FONT_HEIGHT;

	m_spriteData.width = 0;
	m_spriteData.height = 0;
	m_spriteData.rect.bottom = 1;         
	m_spriteData.rect.right = 1;

	for (int row = 0; row < NS_Text::ROWS; row++)        
	{
		for (int col = 0; col < NS_Text::COLUMNS; col++) 
		{
			m_fontData[row][col].left = MAXINT;
			m_fontData[row][col].right = 0;
		}
	}
	
	m_isProportional = false;
	m_proportionalSpacing = NS_Text::PROPORTIONAL_SPACING;
	m_isUnderline = false;
	m_isBold = false;
	m_tabSize = NS_Text::TAB_SIZE;
}

Text::~Text()
{
	Image::~Image();
}

bool Text::initialize(Graphics* pG, const char* filename)
{
	try
	{
		m_pGraphics = pG;

		UINT wid;
		UINT hei;
		if (FAILED(m_pGraphics->loadTextureSysMem(filename, TRANSCOLOR,
			&wid, &hei, &m_fontTexture)))
		{
			SAFE_RELEASE(m_fontTexture);
			return false;
		}

		D3DLOCKED_RECT rect;
		if (FAILED(m_fontTexture->LockRect(0, &rect, nullptr, D3DLOCK_READONLY)))
		{
			SAFE_RELEASE(m_fontTexture);
			return false;
		}

		for (DWORD row = 0; row < NS_Text::ROWS; ++row)
		{
			for (DWORD col = 0; col < NS_Text::COLUMNS; ++col)
			{
				m_fontData[row][col].left = MAXINT;
				m_fontData[row][col].right = 0;

				for (DWORD y = row * NS_Text::GRID_HEIGHT + 1;
					y < (row + 1) * NS_Text::GRID_HEIGHT - 1; ++y)
				{
					DWORD* pBits = (DWORD*)((BYTE*)rect.pBits + y * rect.Pitch);

					for (DWORD x = col * NS_Text::GRID_WIDTH + 1;
						x < (col + 1) * NS_Text::GRID_WIDTH - 1; ++x)
					{
						DWORD dwPixel = pBits[x];

						if (dwPixel & 0xff000000)
						{
							if (x < m_fontData[row][col].left)
							{
								m_fontData[row][col].left = x;
							}
							if (x > m_fontData[row][col].right)
							{
								m_fontData[row][col].right = x;
							}
						}
					}
				}
			}
		}

		m_fontTexture->UnlockRect(0);
		SAFE_RELEASE(m_fontTexture);

		if (!m_fontTM.initialize(m_pGraphics, filename))
		{
			return false;
		}
		if (!Image::initialize(m_pGraphics, NS_Text::FONT_WIDTH, NS_Text::FONT_HEIGHT, 0, &m_fontTM))
		{
			return false;
		}
	}
	catch (...)
	{
		return false;
	}

	return true;                 
}

void Text::setXY(int x2, int y2)
{
	if (x2 < 0 || y2 < 0)
		return;
	m_spriteData.x = (float)x2;
	m_spriteData.y = (float)y2;
}

void Text::print(const std::string* str)
{
	print(str, static_cast<int>(m_spriteData.x), static_cast<int>(m_spriteData.y));
}

void Text::print(const std::string* str, int x, int y, NS_Text::Alignment al)
{
	m_align = al;             
	print(str, x, y);
	m_align = NS_Text::LEFT;
}

void Text::print(const std::string* str, int x, int y)
{
	UCHAR ch = 0;
	UCHAR chN = 0;
	std::string str2;
	m_width = NS_Text::FONT_WIDTH;
	int scaledWidth = static_cast<int>(NS_Text::FONT_WIDTH * m_spriteData.scale);
	float saveY = 0;
	int tabX = 0;
	int tabW = 0;

	m_spriteData.x = static_cast<float>(x);
	m_spriteData.y = static_cast<float>(y);
	doAlign(str);

	for (UINT i = 0; i<str->length(); i++)
	{
		ch = str->at(i);
		if (ch > NS_Text::MIN_CHAR && ch <= NS_Text::MAX_CHAR)
		{
			chN = ch - NS_Text::MIN_CHAR;
			m_spriteData.rect.top = chN / NS_Text::COLUMNS * NS_Text::GRID_HEIGHT + 1;
			m_spriteData.rect.bottom = m_spriteData.rect.top + NS_Text::FONT_HEIGHT;
			
			if (m_isProportional)
			{
				m_spriteData.rect.left =
					m_fontData[chN / NS_Text::COLUMNS][chN % NS_Text::COLUMNS].left;
				m_spriteData.rect.right =
					m_fontData[chN / NS_Text::COLUMNS][chN % NS_Text::COLUMNS].right + 1;
				m_width =
					m_spriteData.rect.right - m_spriteData.rect.left;
				
				if (m_width >= NS_Text::FONT_WIDTH)
				{
					m_width = NS_Text::FONT_WIDTH;
					m_spriteData.rect.left =
						chN % NS_Text::COLUMNS * NS_Text::GRID_WIDTH + 1;
					m_spriteData.rect.right =
						m_spriteData.rect.left + NS_Text::FONT_WIDTH;
				}
				else
				{
					m_width += m_proportionalSpacing;
				}

				scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
				drawChar(ch);
			}
			else
			{
				m_width = NS_Text::FONT_WIDTH;
				m_spriteData.rect.left = chN % NS_Text::COLUMNS * NS_Text::GRID_WIDTH + 1;
				m_spriteData.rect.right = m_spriteData.rect.left + NS_Text::FONT_WIDTH;
				drawChar(ch);
			}

			m_spriteData.x += scaledWidth;
		}
		else
		{
			switch (ch)
			{

			case ' ':
				if (m_isProportional)
				{
					m_width = NS_Text::FONT_WIDTH / 2;
					scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
				}
				drawChar(' ');
				m_spriteData.x += scaledWidth;
				break;

			case '\n':
				m_spriteData.x = (float)x;
				m_spriteData.y += static_cast<int>(m_height * m_spriteData.scale);
				saveY = m_spriteData.y;
				str2 = str->substr(i, str->length());
				doAlign(&str2);
				m_spriteData.y = saveY;
				break;

			case '\r':                            
				m_spriteData.x = (float)x;
				str2 = str->substr(i, str->length());
				doAlign(&str2);
				break;

			case '\t':                            
				m_width = NS_Text::FONT_WIDTH;
				scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
				tabX = static_cast<int>(m_spriteData.x) / (scaledWidth * m_tabSize);
				tabX = (tabX + 1) * scaledWidth * m_tabSize;
				tabW = tabX - static_cast<int>(m_spriteData.x);
				while (tabW > 0)
				{
					if (tabW >= scaledWidth)
					{
						drawChar(' ');
						m_spriteData.x += scaledWidth;
					}
					else
					{
						m_width = tabW;
						drawChar(' ');
						m_spriteData.x += tabW;
					}
					tabW -= scaledWidth;
				}
				break;

			case '\b':
				m_spriteData.x -= scaledWidth;
				if (m_spriteData.x < 0)
					m_spriteData.x = 0;
				break;

			case '\v':                            
				m_spriteData.y += static_cast<int>(m_height * m_spriteData.scale);
				break;

			case 0x01:                            
				m_spriteData.rect.top = 1;
				m_spriteData.rect.bottom = 1 + NS_Text::FONT_HEIGHT;
				m_spriteData.rect.left = 1;
				m_spriteData.rect.right = 1 + NS_Text::FONT_WIDTH;
				draw(m_spriteData);
				m_spriteData.x += scaledWidth;
				break;
			}
		}
	}
}

void Text::doAlign(const std::string* str)
{
	if (!(m_spriteData.texture))
	{
		return;
	}

	UINT wid;
	UINT hei;

	switch (m_align)
	{

	case NS_Text::CENTER:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.x -= wid / 2;
		break;

	case NS_Text::RIGHT:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.x -= wid;
		break;

	case NS_Text::CENTER_MIDDLE:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.x -= wid / 2;
		m_spriteData.y -= hei / 2;
		break;

	case NS_Text::CENTER_BOTTOM:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.x -= wid / 2;
		m_spriteData.y -= hei;
		break;

	case NS_Text::LEFT_BOTTOM:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.y -= hei;
		break;

	case NS_Text::RIGHT_BOTTOM:
		getWidthHeight(str, &wid, &hei);
		m_spriteData.x -= wid;
		m_spriteData.y -= hei;
		break;

	}
}

void Text::getWidthHeight(const std::string* str, UINT* wid, UINT* hei)
{
	if (m_spriteData.texture == NULL)         // if no texture
		return;

	UCHAR ch = 0;
	UCHAR chN = 0;
	m_width = NS_Text::FONT_WIDTH;
	int scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
	int strW = 0;
	*hei = 0;
	int stringWidth = 0;

	for (UINT i = 0; i<str->length(); i++)
	{
		ch = str->at(i);

		if (ch > NS_Text::MIN_CHAR && ch <= NS_Text::MAX_CHAR)
		{
			chN = ch - NS_Text::MIN_CHAR;
			if (m_isProportional)
			{
				m_spriteData.rect.left = m_fontData[chN / NS_Text::COLUMNS][chN % NS_Text::COLUMNS].left;
				m_spriteData.rect.right = m_fontData[chN / NS_Text::COLUMNS][chN % NS_Text::COLUMNS].right + 1;
				m_width = m_spriteData.rect.right - m_spriteData.rect.left + m_proportionalSpacing;
				scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
			}
			else
			{
				m_width = NS_Text::FONT_WIDTH;
				m_spriteData.rect.left = chN % NS_Text::COLUMNS * NS_Text::GRID_WIDTH + 1;
				m_spriteData.rect.right = m_spriteData.rect.left + NS_Text::FONT_WIDTH;
			}
			stringWidth += scaledWidth;
		}
		else
		{
			switch (ch)
			{

			case ' ':
				if (m_isProportional)
				{
					m_width = (NS_Text::FONT_WIDTH) / 2;
					scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
				}
				stringWidth += scaledWidth;
				break;

			case '\n':
				if (strW == 0)
					strW = stringWidth;
				stringWidth = 0;
				hei += static_cast<int>(m_height*m_spriteData.scale);
				break;

			case '\r':
				if (strW == 0)
					strW = stringWidth;
				stringWidth = 0;
				break;

			case '\t':
				{
					m_width = NS_Text::FONT_WIDTH;
					scaledWidth = static_cast<int>(m_width * m_spriteData.scale);
					int tabX = static_cast<int>(m_spriteData.x) / (scaledWidth * m_tabSize);
					tabX = (tabX + 1) * scaledWidth * m_tabSize;
					int tabW = tabX - static_cast<int>(m_spriteData.x);
					while (tabW > 0)
					{
						if (tabW >= scaledWidth)
							stringWidth += scaledWidth;
						else
						{
							m_width = tabW;
							stringWidth += tabW;
						}
						tabW -= scaledWidth;
					}
				}
				break;

			case '\b':
				stringWidth -= scaledWidth;
				if (stringWidth < 0)
					stringWidth = 0;
				break;

			case 0x01:
				stringWidth += scaledWidth;
				break;

			}
		}
	}
	if (strW == 0)
		strW = stringWidth;
	*wid = strW;
	return;
}

void Text::drawChar(UCHAR ch)
{
	SpriteData sd2 = m_spriteData;

	if (m_backColor != static_cast<DWORD>(TRANSCOLOR))
	{
		m_spriteData.rect.top = (NS_Text::SOLID - NS_Text::MIN_CHAR) / NS_Text::COLUMNS * NS_Text::GRID_HEIGHT + 1;
		m_spriteData.rect.bottom = m_spriteData.rect.top + NS_Text::GRID_HEIGHT - 2;
		m_spriteData.rect.left = (NS_Text::SOLID - NS_Text::MIN_CHAR) % NS_Text::COLUMNS * NS_Text::GRID_WIDTH + 1;
		m_spriteData.rect.right = m_spriteData.rect.left + m_width;
		draw(m_backColor);
		m_spriteData.rect = sd2.rect;
	}

	if (m_isUnderline)
	{
		m_spriteData.rect.top = (NS_Text::UNDERLINE - NS_Text::MIN_CHAR) / NS_Text::COLUMNS * NS_Text::GRID_HEIGHT + 1;
		m_spriteData.rect.bottom = m_spriteData.rect.top + NS_Text::GRID_HEIGHT - 2;
		m_spriteData.rect.left = (NS_Text::UNDERLINE - NS_Text::MIN_CHAR) % NS_Text::COLUMNS * NS_Text::GRID_WIDTH + 1;
		m_spriteData.rect.right = m_spriteData.rect.left + m_width;
		draw(m_color);
		m_spriteData.rect = sd2.rect;
	}

	if (ch > NS_Text::MIN_CHAR && ch <= NS_Text::MAX_CHAR)
	{
		draw(m_spriteData, m_color);
		if (m_isBold)
		{
			m_spriteData.x += NS_Text::BOLD_SIZE*m_spriteData.scale;
			draw(m_spriteData, m_color);
			m_spriteData.x = sd2.x;
		}
	}
}

void Text::onLostDevice()
{
	try
	{
		if (m_isInitialized)
		{
			m_fontTM.onLostDevice();
		}
	}
	catch (...)
	{
		;
	}
}

void Text::onResetDevice()
{
	try
	{
		if (m_isInitialized)
		{
			m_fontTM.onResetDevice();
		}
	}
	catch (...)
	{
		;
	}
}
