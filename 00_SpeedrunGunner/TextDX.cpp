#include "TextDX.h"

TextDX::TextDX()
	: m_pGraphics{ nullptr }, m_font{ nullptr }
{
	m_color = SETCOLOR_ARGB(255, 255, 255, 255);

	m_rect.left = m_rect.top = 0;
	m_rect.right = GAME_WIDTH;
	m_rect.bottom = GAME_HEIGHT;

	m_angle = 0;
}

TextDX::~TextDX()
{
	SAFE_RELEASE(m_font);
}

bool TextDX::initialize(Graphics* pG, int hei,
	bool isBold, bool isItalic, const std::string* name)
{
	m_pGraphics = pG;

	UINT weight = FW_NORMAL;

	if (isBold)
	{
		weight = FW_BOLD;
	}

	if (FAILED(D3DXCreateFont(m_pGraphics->getDevice(), hei, 0, weight, 1, isItalic,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, name->c_str(), &m_font)))
	{
		return false;
	}
	else
	{
		D3DXMatrixTransformation2D(&m_matrix, nullptr, 0.0f, nullptr, nullptr, 0.0f, nullptr);

		return true;
	}
}

int TextDX::print(const std::string* str, int x, int y)
{
	if (!m_font)
	{
		return 0;
	}

	m_rect.left = x;
	m_rect.top = y;

	D3DXVECTOR2 center = D3DXVECTOR2(static_cast<float>(x), static_cast<float>(y));
	D3DXMatrixTransformation2D(&m_matrix, nullptr, 0.0f, nullptr, &center, m_angle, nullptr);

	m_pGraphics->getSprite()->SetTransform(&m_matrix);
	
	return m_font->DrawTextA(m_pGraphics->getSprite(),
		str->c_str(), -1, &m_rect, DT_LEFT, m_color);
}

int TextDX::print(const std::string* str, RECT* rect, UINT format)
{
	if (!m_font)
	{
		return 0;
	}

	D3DXMatrixTransformation2D(&m_matrix, nullptr, 0.0f, nullptr, nullptr, 0.0f, nullptr);
	
	m_pGraphics->getSprite()->SetTransform(&m_matrix);

	return m_font->DrawTextA(m_pGraphics->getSprite(),
		str->c_str(), -1, rect, format, m_color);
}

void TextDX::onLostDevice()
{
	if (m_font)
	{
		m_font->OnLostDevice();
	}
}

void TextDX::onResetDevice()
{
	if (m_font)
	{
		m_font->OnResetDevice();
	}
}