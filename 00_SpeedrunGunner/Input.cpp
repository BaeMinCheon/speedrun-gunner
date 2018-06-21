#include "Input.h"

Input::Input()
{
	for (int i = 0; i < NS_Input::KEYS_ARRAY_LEN; ++i)
	{
		m_keysDown[i] = m_keysPressed[i] = false;
	}
	m_isNewline = true;
	m_textIn = "";
	m_charIn = 0;

	m_mouseX = m_mouseY = m_mouseRawX = m_mouseRawY = 0;
	m_isMouseLButton = m_isMouseMButton = m_isMouseRButton = false;
	m_isMouseX1Button = m_isMouseX2Button = false;

	for (int i = 0; i < NS_Input::MAX_CONTROLLERS; ++i)
	{
		m_controllerState[i].vibrateTimeLeft = 0;
		m_controllerState[i].vibrateTimeRight = 0;
	}
}

Input::~Input()
{
	if (m_isMouseCaptured)
	{
		ReleaseCapture();
	}
}

void Input::initialize(HWND hwnd, bool isCap)
{
	try
	{
		m_isMouseCaptured = isCap;

		m_RID[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		m_RID[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		m_RID[0].dwFlags = RIDEV_INPUTSINK;
		m_RID[0].hwndTarget = hwnd;
		RegisterRawInputDevices(m_RID, 1, sizeof(m_RID[0]));

		if (m_isMouseCaptured)
		{
			SetCapture(hwnd);
		}

		ZeroMemory(m_controllerState, sizeof(ControllerState) * NS_Input::MAX_CONTROLLERS);
		checkController();
	}
	catch (...)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing input system"));
	}
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Input::keyDown(WPARAM wParam)
{
	if (wParam < NS_Input::KEYS_ARRAY_LEN)
	{
		m_keysDown[wParam] = true;
		m_keysPressed[wParam] = true;
	}
}

void Input::keyUp(WPARAM wParam)
{
	if (wParam < NS_Input::KEYS_ARRAY_LEN)
	{
		m_keysDown[wParam] = false;
	}
}

void Input::keyIn(WPARAM wParam)
{
	if (m_isNewline)
	{
		m_textIn.clear();
		m_isNewline = false;
	}

	if (wParam == '\b')
	{
		if (m_textIn.length() > 0)
		{
			m_textIn.erase(m_textIn.size() - 1);
		}
	}
	else
	{
		m_textIn += wParam;
		m_charIn = wParam;
	}

	if (wParam == '\r')
	{
		m_isNewline = true;
	}
}

bool Input::isKeyDown(UCHAR key) const
{
	if (key < NS_Input::KEYS_ARRAY_LEN)
	{
		return m_keysDown[key];
	}
	else
	{
		return false;
	}
}

bool Input::wasKeyPressed(UCHAR key) const
{
	if (key < NS_Input::KEYS_ARRAY_LEN)
	{
		return m_keysPressed[key];
	}
	else
	{
		return false;
	}
}

bool Input::anyKeyPressed() const
{
	for (int i = 0; i < NS_Input::KEYS_ARRAY_LEN; ++i)
	{
		if (m_keysPressed[i] == true)
		{
			return true;
		}
	}
	return false;
}

void Input::clearKeyPress(UCHAR key)
{
	if (key < NS_Input::KEYS_ARRAY_LEN)
	{
		m_keysPressed[key] = false;
	}
}

void Input::clear(UCHAR what)
{
	if (what & NS_Input::KEYS_DOWN)
	{
		for (int i = 0; i < NS_Input::KEYS_ARRAY_LEN; ++i)
		{
			m_keysDown[i] = false;
		}
	}
	if (what & NS_Input::KEYS_PRESSED)
	{
		for (int i = 0; i < NS_Input::KEYS_ARRAY_LEN; ++i)
		{
			m_keysPressed[i] = false;
		}
	}
	if (what & NS_Input::MOUSE)
	{
		m_mouseX = m_mouseY = m_mouseRawX = m_mouseRawY = 0;
	}
	if (what & NS_Input::TEXT_IN)
	{
		clearTextIn();
	}
}

void Input::clearAll()
{
	clear(NS_Input::KEYS_MOUSE_TEXT);
}

void Input::clearTextIn()
{
	m_textIn.clear();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Input::mouseIn(LPARAM lParam)
{
	m_mouseX = GET_X_LPARAM(lParam);
	m_mouseY = GET_Y_LPARAM(lParam);
}

void Input::mouseRawIn(LPARAM lParam)
{
	UINT dwSize = 40;
	static BYTE LPB[40];

	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, LPB, &dwSize, sizeof(RAWINPUTHEADER));
	RAWINPUT* pRaw = reinterpret_cast<RAWINPUT*>(LPB);

	if (pRaw->header.dwType == RIM_TYPEMOUSE)
	{
		m_mouseRawX = pRaw->data.mouse.lLastX;
		m_mouseRawY = pRaw->data.mouse.lLastY;
	}
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Input::checkController()
{
	DWORD result;
	for (DWORD i = 0; i < NS_Input::MAX_CONTROLLERS; ++i)
	{
		result = XInputGetState(i, &m_controllerState[i].state);

		m_controllerState[i].isConnected = (result == ERROR_SUCCESS) ? true : false;
	}
}

void Input::readController()
{
	DWORD result;
	for (DWORD i = 0; i < NS_Input::MAX_CONTROLLERS; ++i)
	{
		if (m_controllerState[i].isConnected)
		{
			result = XInputGetState(i, &m_controllerState[i].state);

			if (result == ERROR_DEVICE_NOT_CONNECTED)
			{
				m_controllerState[i].isConnected = false;
			}
		}
	}
}

const ControllerState* Input::getControllerState(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return &m_controllerState[n];
}

const WORD Input::getGamepadButtons(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return m_controllerState[n].state.Gamepad.wButtons;
}

bool Input::getGamepadDPadUp(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return ((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_DPAD_UP) != 0);
}

bool Input::getGamepadDPadDown(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return ((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_DPAD_DOWN) != 0);
}

bool Input::getGamepadDPadLeft(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return ((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_DPAD_LEFT) != 0);
}

bool Input::getGamepadDPadRight(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_DPAD_RIGHT) != 0);
}

bool Input::getGamepadStart(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_START_BUTTON) != 0);
}

bool Input::getGamepadBack(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_BACK_BUTTON) != 0);
}

bool Input::getGamepadLeftThumb(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_LEFT_THUMB) != 0);
}

bool Input::getGamepadRightThumb(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_RIGHT_THUMB) != 0);
}

bool Input::getGamepadLeftShoulder(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_LEFT_SHOULDER) != 0);
}

bool Input::getGamepadRightShoulder(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_RIGHT_SHOULDER) != 0);
}

bool Input::getGamepadA(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_A) != 0);
}

bool Input::getGamepadB(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_B) != 0);
}

bool Input::getGamepadX(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_X) != 0);
}

bool Input::getGamepadY(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return bool((m_controllerState[n].state.Gamepad.wButtons & NS_Input::GAMEPAD_Y) != 0);
}

BYTE Input::getGamepadLeftTrigger(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.bLeftTrigger);
}

BYTE Input::getGamepadRightTrigger(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.bRightTrigger);
}

SHORT Input::getGamepadThumbLX(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.sThumbLX);
}

SHORT Input::getGamepadThumbLY(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.sThumbLY);
}

SHORT Input::getGamepadThumbRX(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.sThumbRX);
}

SHORT Input::getGamepadThumbRY(UINT n)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	return (m_controllerState[n].state.Gamepad.sThumbRY);
}

void Input::gamePadVibrateLeft(UINT n, WORD speed, float sec)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	m_controllerState[n].vibration.wLeftMotorSpeed = speed;
	m_controllerState[n].vibrateTimeLeft = sec;
}

void Input::gamePadVibrateRight(UINT n, WORD speed, float sec)
{
	if (n > NS_Input::MAX_CONTROLLERS - 1)
	{
		n = NS_Input::MAX_CONTROLLERS - 1;
	}
	m_controllerState[n].vibration.wRightMotorSpeed = speed;
	m_controllerState[n].vibrateTimeRight = sec;
}

void Input::vibrateControllers(float frameTime)
{
	for (int i = 0; i < NS_Input::MAX_CONTROLLERS; ++i)
	{
		if (m_controllerState[i].isConnected)
		{
			m_controllerState[i].vibrateTimeLeft -= frameTime;
			if (m_controllerState[i].vibrateTimeLeft < 0)
			{
				m_controllerState[i].vibrateTimeLeft = 0;
				m_controllerState[i].vibration.wLeftMotorSpeed = 0;
			}

			m_controllerState[i].vibrateTimeRight -= frameTime;
			if (m_controllerState[i].vibrateTimeRight < 0)
			{
				m_controllerState[i].vibrateTimeRight = 0;
				m_controllerState[i].vibration.wRightMotorSpeed = 0;
			}

			XInputSetState(i, &m_controllerState[i].vibration);
		}
	}
}