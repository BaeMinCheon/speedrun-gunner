#include "MessageDialog.h"

MessageDialog::MessageDialog()
{
	m_isInitialized = false;
	m_pGraphics = NULL;
	m_isVisible = false;
	m_fontColor = NS_MessageDialog::FONT_COLOR;
	m_borderColor = NS_MessageDialog::BORDER_COLOR;
	m_backColor = NS_MessageDialog::BACK_COLOR;
	m_buttonColor = NS_MessageDialog::BUTTON_COLOR;
	m_buttonFontColor = NS_MessageDialog::BUTTON_FONT_COLOR;
	m_x = static_cast<float>(NS_MessageDialog::X);
	m_y = static_cast<float>(NS_MessageDialog::Y);
	m_height = NS_MessageDialog::HEIGHT;
	m_width = NS_MessageDialog::WIDTH;
	m_textRect.bottom = NS_MessageDialog::Y + NS_MessageDialog::HEIGHT - NS_MessageDialog::MARGIN;
	m_textRect.left = NS_MessageDialog::X + NS_MessageDialog::MARGIN;
	m_textRect.right = NS_MessageDialog::X + NS_MessageDialog::WIDTH - NS_MessageDialog::MARGIN;
	m_textRect.top = NS_MessageDialog::Y + NS_MessageDialog::MARGIN;
	m_dialogVerts = NULL;
	m_borderVerts = NULL;
	m_leftButtonVerts = NULL;
	m_rightButtonVerts = NULL;
	m_buttonType = 0;
}

MessageDialog::~MessageDialog()
{
	onLostDevice();
}

bool MessageDialog::initialize(Graphics* pG, Input* pI, HWND hwnd)
{
	try
	{
		m_pGraphics = pG;
		m_pInput = pI;
		m_hwnd = hwnd;

		if (!m_dxFont.initialize(m_pGraphics, NS_MessageDialog::FONT_HEIGHT, false,
			false, &std::string(NS_MessageDialog::FONT)))
		{
			return false;
		}
		m_dxFont.setFontColor(m_fontColor);
	}
	catch (...)
	{
		return false;
	}

	m_isInitialized = true;
	return true;
}

void MessageDialog::prepareVerts()
{
	SAFE_RELEASE(m_dialogVerts);
	SAFE_RELEASE(m_borderVerts);
	SAFE_RELEASE(m_leftButtonVerts);
	SAFE_RELEASE(m_rightButtonVerts);

	m_vtx[0].x = m_x;
	m_vtx[0].y = m_y;
	m_vtx[0].z = 0.0f;
	m_vtx[0].RHW = 1.0f;
	m_vtx[0].color = m_borderColor;

	m_vtx[1].x = m_x + m_width;
	m_vtx[1].y = m_y;
	m_vtx[1].z = 0.0f;
	m_vtx[1].RHW = 1.0f;
	m_vtx[1].color = m_borderColor;

	m_vtx[2].x = m_x + m_width;
	m_vtx[2].y = m_y + m_height;
	m_vtx[2].z = 0.0f;
	m_vtx[2].RHW = 1.0f;
	m_vtx[2].color = m_borderColor;

	m_vtx[3].x = m_x;
	m_vtx[3].y = m_y + m_height;
	m_vtx[3].z = 0.0f;
	m_vtx[3].RHW = 1.0f;
	m_vtx[3].color = m_borderColor;

	m_pGraphics->createVertexBuffer(m_vtx, sizeof m_vtx, &m_borderVerts);

	m_vtx[0].x = m_x + NS_MessageDialog::BORDER;
	m_vtx[0].y = m_y + NS_MessageDialog::BORDER;
	m_vtx[0].z = 0.0f;
	m_vtx[0].RHW = 1.0f;
	m_vtx[0].color = m_backColor;

	m_vtx[1].x = m_x + m_width - NS_MessageDialog::BORDER;
	m_vtx[1].y = m_y + NS_MessageDialog::BORDER;
	m_vtx[1].z = 0.0f;
	m_vtx[1].RHW = 1.0f;
	m_vtx[1].color = m_backColor;

	m_vtx[2].x = m_x + m_width - NS_MessageDialog::BORDER;
	m_vtx[2].y = m_y + m_height - NS_MessageDialog::BORDER;
	m_vtx[2].z = 0.0f;
	m_vtx[2].RHW = 1.0f;
	m_vtx[2].color = m_backColor;

	m_vtx[3].x = m_x + NS_MessageDialog::BORDER;
	m_vtx[3].y = m_y + m_height - NS_MessageDialog::BORDER;
	m_vtx[3].z = 0.0f;
	m_vtx[3].RHW = 1.0f;
	m_vtx[3].color = m_backColor;

	m_pGraphics->createVertexBuffer(m_vtx, sizeof m_vtx, &m_dialogVerts);

	m_vtx[0].x = m_x + m_width / 2.0f - NS_MessageDialog::BUTTON_WIDTH / 2.0f;
	m_vtx[0].y = m_y + m_height - NS_MessageDialog::BORDER - NS_MessageDialog::MARGIN - NS_MessageDialog::BUTTON_HEIGHT;
	m_vtx[0].z = 0.0f;
	m_vtx[0].RHW = 1.0f;
	m_vtx[0].color = m_buttonColor;

	m_vtx[1].x = m_x + m_width / 2.0f + NS_MessageDialog::BUTTON_WIDTH / 2.0f;
	m_vtx[1].y = m_vtx[0].y;
	m_vtx[1].z = 0.0f;
	m_vtx[1].RHW = 1.0f;
	m_vtx[1].color = m_buttonColor;

	m_vtx[2].x = m_vtx[1].x;
	m_vtx[2].y = m_vtx[0].y + NS_MessageDialog::BUTTON_HEIGHT;
	m_vtx[2].z = 0.0f;
	m_vtx[2].RHW = 1.0f;
	m_vtx[2].color = m_buttonColor;

	m_vtx[3].x = m_vtx[0].x;
	m_vtx[3].y = m_vtx[2].y;
	m_vtx[3].z = 0.0f;
	m_vtx[3].RHW = 1.0f;
	m_vtx[3].color = m_buttonColor;

	m_pGraphics->createVertexBuffer(m_vtx, sizeof m_vtx, &m_leftButtonVerts);

	m_leftButtonRect.left = (long)m_vtx[0].x;
	m_leftButtonRect.right = (long)m_vtx[1].x;
	m_leftButtonRect.top = (long)m_vtx[0].y;
	m_leftButtonRect.bottom = (long)m_vtx[2].y;

	m_vtx[0].x = m_x + m_width - NS_MessageDialog::BUTTON_WIDTH*1.2f;
	m_vtx[0].y = m_y + m_height - NS_MessageDialog::BORDER - NS_MessageDialog::MARGIN - NS_MessageDialog::BUTTON_HEIGHT;
	m_vtx[0].z = 0.0f;
	m_vtx[0].RHW = 1.0f;
	m_vtx[0].color = m_buttonColor;

	m_vtx[1].x = m_vtx[0].x + NS_MessageDialog::BUTTON_WIDTH;
	m_vtx[1].y = m_vtx[0].y;
	m_vtx[1].z = 0.0f;
	m_vtx[1].RHW = 1.0f;
	m_vtx[1].color = m_buttonColor;

	m_vtx[2].x = m_vtx[1].x;
	m_vtx[2].y = m_vtx[0].y + NS_MessageDialog::BUTTON_HEIGHT;
	m_vtx[2].z = 0.0f;
	m_vtx[2].RHW = 1.0f;
	m_vtx[2].color = m_buttonColor;

	m_vtx[3].x = m_vtx[0].x;
	m_vtx[3].y = m_vtx[2].y;
	m_vtx[3].z = 0.0f;
	m_vtx[3].RHW = 1.0f;
	m_vtx[3].color = m_buttonColor;
	m_pGraphics->createVertexBuffer(m_vtx, sizeof m_vtx, &m_rightButtonVerts);

	m_rightButtonRect.left = (long)m_vtx[0].x;
	m_rightButtonRect.right = (long)m_vtx[1].x;
	m_rightButtonRect.top = (long)m_vtx[0].y;
	m_rightButtonRect.bottom = (long)m_vtx[2].y;
}

const void MessageDialog::draw()
{
	if (!m_isVisible || m_pGraphics == NULL || !m_isInitialized)
		return;

	m_pGraphics->drawQuad(&m_borderVerts);
	m_pGraphics->drawQuad(&m_dialogVerts);
	m_pGraphics->drawQuad(&m_leftButtonVerts);
	if (m_buttonType != NS_MessageDialog::JUST_OK)
	{
		m_pGraphics->drawQuad(&m_rightButtonVerts);
	}
	
	m_pGraphics->beginSprite();

	if (m_string.size() == 0)
	{
		return;
	}

	m_dxFont.setFontColor(m_fontColor);
	m_dxFont.print(&m_string, &m_textRect, DT_CENTER | DT_WORDBREAK);

	m_dxFont.setFontColor(m_buttonFontColor);
	m_dxFont.print(&std::string(NS_MessageDialog::BUTTON1_TEXT[m_buttonType]),
		&m_leftButtonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	m_dxFont.print(&std::string(NS_MessageDialog::BUTTON2_TEXT[m_buttonType]),
		&m_rightButtonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	m_pGraphics->endSprite();
}

void MessageDialog::update()
{
	if (!m_isInitialized || !m_isVisible)
	{
		return;
	}
	else if (m_pInput->wasKeyPressed(NS_MessageDialog::DIALOG_CLOSE_KEY))
	{
		m_isVisible = false;
		m_isButtonClicked = 1;
		return;
	}

	if (m_pGraphics->isFullscreen() == false)
	{
		RECT clientRect;
		GetClientRect(m_hwnd, &clientRect);
		m_screenRatioX = (float)GAME_WIDTH / clientRect.right;
		m_screenRatioY = (float)GAME_HEIGHT / clientRect.bottom;
	}

	if (m_pInput->getMouseLButton())
	{
		if (m_pInput->getMouseX()*m_screenRatioX >= m_leftButtonRect.left &&
			m_pInput->getMouseX()*m_screenRatioX <= m_leftButtonRect.right &&
			m_pInput->getMouseY()*m_screenRatioY >= m_leftButtonRect.top &&
			m_pInput->getMouseY()*m_screenRatioY <= m_leftButtonRect.bottom)
		{
			m_isVisible = false;
			m_isButtonClicked = 1;
			return;
		}

		if (m_buttonType != NS_MessageDialog::JUST_OK)
		{
			if (m_pInput->getMouseX()*m_screenRatioX >= m_rightButtonRect.left &&
				m_pInput->getMouseX()*m_screenRatioX <= m_rightButtonRect.right &&
				m_pInput->getMouseY()*m_screenRatioY >= m_rightButtonRect.top &&
				m_pInput->getMouseY()*m_screenRatioY <= m_rightButtonRect.bottom)
			{
				m_isVisible = false;
				m_isButtonClicked = 2;
			}
		}
	}
}

void MessageDialog::print(const std::string* str)
{
	if (!m_isInitialized || m_isVisible)
	{
		return;
	}
	m_string = *str + std::string("\n\n\n\n");

	m_textRect.left = (long)(m_x + NS_MessageDialog::MARGIN);
	m_textRect.right = (long)(m_x + NS_MessageDialog::WIDTH - NS_MessageDialog::MARGIN);
	m_textRect.top = (long)(m_y + NS_MessageDialog::MARGIN);
	m_textRect.bottom = (long)(m_y + NS_MessageDialog::HEIGHT - NS_MessageDialog::MARGIN);

	m_dxFont.print(&m_string, &m_textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	m_height = m_textRect.bottom - (int)m_y + NS_MessageDialog::BORDER + NS_MessageDialog::MARGIN;

	prepareVerts();
	m_isButtonClicked = 0;
	m_isVisible = true;
}

void MessageDialog::onLostDevice()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_dxFont.onLostDevice();
	SAFE_RELEASE(m_dialogVerts);
	SAFE_RELEASE(m_borderVerts);
	SAFE_RELEASE(m_leftButtonVerts);
	SAFE_RELEASE(m_rightButtonVerts);
}

void MessageDialog::onResetDevice()
{
	if (!m_isInitialized)
	{
		return;
	}

	prepareVerts();
	m_dxFont.onResetDevice();
}

