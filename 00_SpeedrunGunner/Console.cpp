#include "Console.h"

Console::Console()
	: m_pGraphics{ nullptr }, m_pInput{ nullptr },
	m_pFont{ nullptr }, m_vertexBuffer{ nullptr }
{
	m_isInitialized = false;
	m_isVisible = false;

	m_fontColor = NS_Console::FONT_COLOR;
	m_backColor = NS_Console::BACK_COLOR;

	m_x = NS_Console::X;
	m_y = NS_Console::Y;

	m_rect.left = NS_Console::X + NS_Console::MARGIN;
	m_rect.top = NS_Console::Y + NS_Console::MARGIN;
	m_rect.right = NS_Console::X + NS_Console::WIDTH - NS_Console::MARGIN;
	m_rect.bottom = NS_Console::Y + NS_Console::HEIGHT - NS_Console::MARGIN;

	m_rows = 0;
	m_scrollAmount = 0;
}

Console::~Console()
{
	onLostDevice();
}

bool Console::initialize(Graphics* pG, Input* pI)
{
	try
	{
		m_pGraphics = pG;
		m_pInput = pI;

		m_vertices[0].x = m_x;
		m_vertices[0].y = m_y;
		m_vertices[0].z = 0.0f;
		m_vertices[0].RHW = 1.0f;
		m_vertices[0].color = m_backColor;

		m_vertices[1].x = m_x + NS_Console::WIDTH;
		m_vertices[1].y = m_y;
		m_vertices[1].z = 0.0f;
		m_vertices[1].RHW = 1.0f;
		m_vertices[1].color = m_backColor;

		m_vertices[2].x = m_x + NS_Console::WIDTH;
		m_vertices[2].y = m_y + NS_Console::HEIGHT;
		m_vertices[2].z = 0.0f;
		m_vertices[2].RHW = 1.0f;
		m_vertices[2].color = m_backColor;

		m_vertices[3].x = m_x;
		m_vertices[3].y = m_y + NS_Console::HEIGHT;
		m_vertices[3].z = 0.0f;
		m_vertices[3].RHW = 1.0f;
		m_vertices[3].color = m_backColor;

		m_pGraphics->createVertexBuffer(m_vertices, sizeof(m_vertices), &m_vertexBuffer);

		m_pFont = new TextDX();
		if (!m_pFont->initialize(m_pGraphics, NS_Console::FONT_HEIGHT,
			false, false, &std::string(NS_Console::FONT)))
		{
			return false;
		}
		m_pFont->setFontColor(m_fontColor);

		m_isInitialized = true;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void Console::draw()
{
	if (!m_isVisible || !m_pGraphics || !m_isInitialized)
	{
		return;
	}

	m_pGraphics->drawQuad(&m_vertexBuffer);

	if (!m_textQueue.size())
	{
		return;
	}

	m_pGraphics->beginSprite();

	m_rect.left = m_rect.top = 0;
	m_pFont->print(&std::string("|"), &m_rect, DT_CALCRECT);
	int rowHeight = m_rect.bottom + 2;
	if (rowHeight < 1)
	{
		rowHeight = 20;
	}
	m_rows = (NS_Console::HEIGHT - 2 * NS_Console::MARGIN) / rowHeight;
	m_rows -= 2;
	if (m_rows < 1)
	{
		m_rows = 5;
	}

	m_rect.left = static_cast<long>
		(m_x, NS_Console::MARGIN);
	m_rect.right = static_cast<long>
		(m_rect.right + NS_Console::WIDTH - NS_Console::MARGIN);
	m_rect.bottom = static_cast<long>
		(m_y + NS_Console::HEIGHT - 2 * NS_Console::MARGIN - 2 * rowHeight);
	for (int r = m_scrollAmount;
		r < m_rows + m_scrollAmount && r < static_cast<int>(m_textQueue.size());
		++r)
	{
		m_rect.top = m_rect.bottom - rowHeight;
		m_pFont->print(&std::string(m_textQueue[r]), &m_rect, DT_LEFT);
		m_rect.bottom -= rowHeight;
	}

	m_rect.bottom = static_cast<long>(m_y + NS_Console::HEIGHT - NS_Console::MARGIN);
	m_rect.top = m_rect.bottom - rowHeight;
	std::string prompt = ">";
	prompt += m_pInput->getTextIn();
	m_pFont->print(&std::string(prompt), &m_rect, DT_LEFT);

	m_pGraphics->endSprite();
}

void Console::showHide()
{
	if (m_isInitialized)
	{
		m_isVisible = !m_isVisible;
		m_pInput->clear(NS_Input::KEYS_PRESSED | NS_Input::TEXT_IN);
	}
}

void Console::print(const std::string* str)
{
	if (m_isInitialized)
	{
		m_textQueue.push_front(*str);
		
		if (m_textQueue.size() > NS_Console::MAX_LINES)
		{
			m_textQueue.pop_back();
		}
	}
}

std::string Console::getCommand()
{
	if (!m_isInitialized || !m_isVisible)
	{
		return "";
	}

	if (m_pInput->wasKeyPressed(CONSOLE_KEY))
	{
		makeHide();
	}

	if (m_pInput->wasKeyPressed(ESC_KEY))
	{
		return "";
	}

	if (m_pInput->wasKeyPressed(VK_UP))
	{
		++m_scrollAmount;
	}
	else if (m_pInput->wasKeyPressed(VK_DOWN))
	{
		--m_scrollAmount;
	}
	else if (m_pInput->wasKeyPressed(VK_PRIOR))
	{
		m_scrollAmount += m_rows;
	}
	else if (m_pInput->wasKeyPressed(VK_NEXT))
	{
		m_scrollAmount -= m_rows;
	}

	if (m_scrollAmount < 0)
	{
		m_scrollAmount = 0;
	}
	if (m_scrollAmount > NS_Console::MAX_LINES - 1)
	{
		m_scrollAmount = NS_Console::MAX_LINES - 1;
	}
	if (m_scrollAmount > static_cast<int>(m_textQueue.size() - 1))
	{
		m_scrollAmount = static_cast<int>(m_textQueue.size() - 1);
	}

	m_commandStr = m_pInput->getTextIn();
	m_pInput->clear(NS_Input::KEYS_DOWN | NS_Input::KEYS_PRESSED | NS_Input::MOUSE);

	if (!m_commandStr.length())
	{
		return "";
	}
	if (m_commandStr.at(m_commandStr.length() - 1) != '\r')
	{
		return "";
	}

	m_commandStr.erase(m_commandStr.length() - 1);
	m_pInput->clearTextIn();
	m_inputStr = m_commandStr;
	return m_commandStr;
}

void Console::onLostDevice()
{
	if (m_isInitialized)
	{
		m_pFont->onLostDevice();
		SAFE_RELEASE(m_vertexBuffer);
	}
}

void Console::onResetDevice()
{
	if (m_isInitialized)
	{
		m_pGraphics->createVertexBuffer(m_vertices, sizeof(m_vertices), &m_vertexBuffer);
		m_pFont->onResetDevice();
	}
}