#pragma once

#ifndef _IMAGE_H
#define _IMAGE_H
#define WIN32_LEAN_AND_MEAN

#include "TextureManager.h"
#include "Constants.h"

class Image
{
public:
	Image();
	virtual ~Image();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	virtual bool initialize(Graphics*, int, int, int, TextureManager*);
	virtual void draw(COLOR_ARGB = NS_Graphics::WHITE);
	virtual void draw(UINT);
	virtual void draw(COLOR_ARGB, UINT);
	virtual void draw(SpriteData, COLOR_ARGB = NS_Graphics::WHITE, UINT = 0);
	virtual void update(float);

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	const virtual SpriteData& getSpriteInfo()	{ return m_spriteData; }
	virtual bool  getVisible()					{ return m_isVisible; }
	virtual float getX()						{ return m_spriteData.x; }
	virtual float getY()						{ return m_spriteData.y; }
	virtual float getScale()					{ return m_spriteData.scale; }
	virtual int   getWidth()					{ return m_spriteData.width; }
	virtual int   getHeight()					{ return m_spriteData.height; }
	virtual float getCenterX()					{ return m_spriteData.x + m_spriteData.width / 2 * getScale(); }
	virtual float getCenterY()					{ return m_spriteData.y + m_spriteData.height / 2 * getScale(); }
	virtual float getDegrees()					{ return m_spriteData.angle*(180.0f / PI); }
	virtual float getRadians()					{ return m_spriteData.angle; }
	virtual float getFrameDelay()				{ return m_frameDelay; }
	virtual int   getStartFrame()				{ return m_startFrame; }
	virtual int   getEndFrame()					{ return m_endFrame; }
	virtual int   getCurrentFrame()				{ return m_currentFrame; }
	virtual RECT  getSpriteDataRect()			{ return m_spriteData.rect; }
	virtual bool  getAnimationComplete()		{ return m_isAnimComplete; }
	virtual COLOR_ARGB getColorFilter()			{ return m_colorFilter; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	virtual void setX(float newX)								{ m_spriteData.x = newX; }
	virtual void setY(float newY)								{ m_spriteData.y = newY; }
	virtual void setScale(float s)								{ m_spriteData.scale = s; }
	virtual void setDegrees(float deg)							{ m_spriteData.angle = deg*(PI / 180.0f); }
	virtual void setRadians(float rad)							{ m_spriteData.angle = rad; }
	virtual void setVisible(bool v)								{ m_isVisible = v; }
	virtual void setFrameDelay(float d)							{ m_frameDelay = d; }
	virtual void setFrames(int s, int e)						{ m_startFrame = s; m_endFrame = e; }
	virtual void setCurrentFrame(int c);
	virtual void setRect();
	virtual void setSpriteDataRect(RECT r)						{ m_spriteData.rect = r; }
	virtual void setLoop(bool lp)								{ m_isLoop = lp; }
	virtual void setAnimationComplete(bool a)					{ m_isAnimComplete = a; };
	virtual void setColorFilter(COLOR_ARGB color)				{ m_colorFilter = color; }
	virtual void setTextureManager(TextureManager *textureM)	{ m_pTM = textureM; }
	virtual void setFlipHOR(bool b)								{ m_spriteData.isFlipedHOR = b; }
	virtual void setFlipVER(bool b)								{ m_spriteData.isFlipedVER = b; }

protected:
	Graphics*			m_pGraphics;
	TextureManager*		m_pTM;
	SpriteData			m_spriteData;
	COLOR_ARGB			m_colorFilter;
	int					m_cols;
	int					m_startFrame;
	int					m_endFrame;
	int					m_currentFrame;
	float				m_frameDelay;
	float				m_animTimer;
	HRESULT				m_HR;
	bool				m_isLoop;
	bool				m_isVisible;
	bool				m_isInitialized;
	bool				m_isAnimComplete;
};

#endif