#pragma once

#ifndef _TEXTUREMANAGER_H
#define _TEXTUREMANAGER_H
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <string>
#include <fstream>
#include "Graphics.h"
#include "Constants.h"

class TextureManager
{
public:
	TextureManager();
	virtual ~TextureManager();

	virtual bool initialize(Graphics*, std::string);

	virtual void onLostDevice();
	virtual void onResetDevice();

	LP_TEXTURE getTexture(std::size_t i = 0) const
	{
		if (i < m_textureVector.size())
		{
			return m_textureVector[i];
		}
		else
		{
			return nullptr;
		}
	}
	UINT getWidth(std::size_t i = 0) const
	{
		if (i < m_widthVector.size())
		{
			return m_widthVector[i];
		}
		else
		{
			return 0;
		}
	}
	UINT getHeight(std::size_t i = 0) const
	{
		if (i < m_heightVector.size())
		{
			return m_heightVector[i];
		}
		else
		{
			return 0;
		}
	}

private:
	Graphics*						m_pGraphics;

	std::vector<std::string>		m_filenameVector;
	std::vector<LP_TEXTURE>			m_textureVector;
	std::vector<UINT>				m_widthVector;
	std::vector<UINT>				m_heightVector;

	bool							m_isInitialized;
	HRESULT							m_HR;
};

#endif