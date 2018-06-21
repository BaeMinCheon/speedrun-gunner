#include "TextureManager.h"

TextureManager::TextureManager()
	: m_pGraphics{ nullptr }
{
	m_isInitialized = false;
}

TextureManager::~TextureManager()
{
	for (std::size_t i = 0; i < m_textureVector.size(); ++i)
	{
		SAFE_RELEASE(m_textureVector[i]);
	}
}

bool TextureManager::initialize(Graphics* pG, std::string file)
{
	bool success = true;

	try
	{
		m_pGraphics = pG;

		if (file.rfind(".txt") == file.size() - 4)
		{
			std::ifstream fs(file.c_str());
			if (!fs)
			{
				return false;
			}

			std::string name;
			while (std::getline(fs, name))
			{
				m_filenameVector.push_back(name);
				m_widthVector.push_back(0);
				m_heightVector.push_back(0);
				m_textureVector.push_back(nullptr);
			}
			fs.close();
		}
		else
		{
			m_filenameVector.push_back(file);
			m_widthVector.push_back(0);
			m_heightVector.push_back(0);
			m_textureVector.push_back(nullptr);
		}

		for (std::size_t i = 0; i < m_filenameVector.size(); ++i)
		{
			if (FAILED(m_pGraphics->loadTexture(m_filenameVector[i].c_str(),
				TRANSCOLOR, &m_widthVector[i], &m_heightVector[i], &m_textureVector[i])))
			{
				success = false;
			}
		}

		m_isInitialized = true;
		return success;
	}
	catch (...)
	{
		return false;
	}
}

void TextureManager::onLostDevice()
{
	try
	{
		if (m_isInitialized)
		{
			for (std::size_t i = 0; i < m_textureVector.size(); ++i)
			{
				SAFE_RELEASE(m_textureVector[i]);
			}
		}
	}
	catch (...)
	{
		throw(GameError(NS_GameError::WARNING,
			"Warning : TextureManager::onLostDevice() attempted to access an invalid texture"));
	}
}

void TextureManager::onResetDevice()
{
	if (m_isInitialized)
	{
		for (std::size_t i = 0; i < m_filenameVector.size(); ++i)
		{
			if (FAILED(m_pGraphics->loadTexture(m_filenameVector[i].c_str(),
				TRANSCOLOR, &m_widthVector[i], &m_heightVector[i], &m_textureVector[i])))
			{
				SAFE_RELEASE(m_textureVector[i]);
			}
		}
	}
}