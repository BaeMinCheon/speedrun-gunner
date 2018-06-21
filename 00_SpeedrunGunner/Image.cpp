#include "Image.h"

Image::Image()
	: m_pTM{ nullptr }, m_pGraphics{ nullptr }
{
	m_spriteData.width = m_spriteData.height = 2;
	m_spriteData.x = m_spriteData.y = 0.0;
	m_spriteData.scale = 1.0;
	m_spriteData.angle = 0.0;
	m_spriteData.rect.left = m_spriteData.rect.top = 0;
	m_spriteData.rect.right = m_spriteData.width;
	m_spriteData.rect.bottom = m_spriteData.height;

	m_isInitialized = false;
	m_cols = 1;
	m_startFrame = m_endFrame = m_currentFrame = 0;
	m_frameDelay = 1.0;
	m_animTimer = 0.0;
	m_isVisible = true;
	m_isLoop = true;
	m_isAnimComplete = false;
	m_colorFilter = NS_Graphics::WHITE;
}

Image::~Image()
{
	;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

bool Image::initialize(Graphics* pG, int wid, int hei, int nCols, TextureManager* pTM)
{
	try
	{
		m_pGraphics = pG;
		m_pTM = pTM;

		m_spriteData.texture = m_pTM->getTexture();
		m_spriteData.width = (wid == 0) ? m_pTM->getWidth() : wid;
		m_spriteData.height = (hei == 0) ? m_pTM->getHeight() : hei;
		m_cols = (nCols == 0) ? 1 : nCols;

		m_spriteData.rect.left = (m_currentFrame % m_cols) * m_spriteData.width;
		m_spriteData.rect.right = m_spriteData.rect.left + m_spriteData.width;
		m_spriteData.rect.top = (m_currentFrame / m_cols) * m_spriteData.height;
		m_spriteData.rect.bottom = m_spriteData.rect.top + m_spriteData.height;

		m_isInitialized = true;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void Image::draw(COLOR_ARGB color)
{
	draw(color, 0);
}

void Image::draw(UINT textureN)
{
	draw(NS_Graphics::WHITE, textureN);
}

void Image::draw(COLOR_ARGB color, UINT textureN)
{
	if (!m_isVisible || !m_pGraphics)
	{
		return;
	}
	
	m_spriteData.texture = m_pTM->getTexture(textureN);

	if (color == NS_Graphics::FILTER)
	{
		m_pGraphics->drawSprite(&m_spriteData, m_colorFilter);
	}
	else
	{
		m_pGraphics->drawSprite(&m_spriteData, color);
	}
}

void Image::draw(SpriteData pSD, COLOR_ARGB color, UINT textureN)
{
	if (!m_isVisible || !m_pGraphics)
	{
		return;
	}

	pSD.rect = m_spriteData.rect;
	pSD.texture = m_pTM->getTexture();

	if (color == NS_Graphics::FILTER)
	{
		m_pGraphics->drawSprite(&pSD, m_colorFilter);
	}
	else
	{
		m_pGraphics->drawSprite(&pSD, color);
	}
}

void Image::update(float frameTime)
{
	if (m_endFrame > m_startFrame)
	{
		m_animTimer += frameTime;
		if (m_animTimer > m_frameDelay)
		{
			m_animTimer -= m_frameDelay;
			++m_currentFrame;

			if (m_currentFrame < m_startFrame || m_currentFrame > m_endFrame)
			{
				if (m_isLoop)
				{
					m_currentFrame = m_startFrame;
				}
				else
				{
					m_currentFrame = m_endFrame;
					m_isAnimComplete = true;
				}
			}

			setRect();
		}
	}
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Image::setCurrentFrame(int c)
{
	if (c >= 0)
	{
		m_currentFrame = c;
		m_isAnimComplete = false;
		setRect();
	}
}

inline void Image::setRect()
{
	m_spriteData.rect.left = (m_currentFrame % m_cols) * m_spriteData.width;
	m_spriteData.rect.right = m_spriteData.rect.left + m_spriteData.width;
	m_spriteData.rect.top = (m_currentFrame / m_cols) * m_spriteData.height;
	m_spriteData.rect.bottom = m_spriteData.rect.top + m_spriteData.height;
}