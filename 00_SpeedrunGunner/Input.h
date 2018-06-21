#pragma once

#ifndef _INPUT_H
#define _INPUT_H
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <XInput.h>
#include "Constants.h"
#include "GameError.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC (USHORT) 0x01
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE (USHORT) 0x02
#endif

namespace NS_Input
{
	const int KEYS_ARRAY_LEN = 256;
	const UCHAR KEYS_DOWN = 1;
	const UCHAR KEYS_PRESSED = 2;
	const UCHAR MOUSE = 4;
	const UCHAR TEXT_IN = 8;
	const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;

	const DWORD GAMEPAD_THUMBSTICK_DEADZONE = (DWORD)(0.20f * 0X7FFF);
	const DWORD GAMEPAD_TRIGGER_DEADZONE = 30;                     
	const DWORD MAX_CONTROLLERS = 4;

	const DWORD GAMEPAD_DPAD_UP = 0x0001;
	const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
	const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
	const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
	const DWORD GAMEPAD_START_BUTTON = 0x0010;
	const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
	const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
	const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
	const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
	const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
	const DWORD GAMEPAD_A = 0x1000;
	const DWORD GAMEPAD_B = 0x2000;
	const DWORD GAMEPAD_X = 0x4000;
	const DWORD GAMEPAD_Y = 0x8000;
}

struct ControllerState
{
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;
	float vibrateTimeLeft;
	float vibrateTimeRight;
	bool isConnected;
};

class Input
{
public:
	Input();
	virtual ~Input();

	void initialize(HWND, bool);

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	
	void keyDown(WPARAM);
	void keyUp(WPARAM);
	void keyIn(WPARAM);
	
	bool isKeyDown(UCHAR) const;
	bool wasKeyPressed(UCHAR) const;
	bool anyKeyPressed() const;

	void clearKeyPress(UCHAR);
	void clear(UCHAR);
	void clearAll();
	void clearTextIn();

	std::string getTextIn()			{ return m_textIn; }
	char getCharIn()				{ return m_charIn; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	void mouseIn(LPARAM);
	void mouseRawIn(LPARAM);

	void setMouseLButton(bool b)	{ m_isMouseLButton = b; }
	void setMouseMButton(bool b)	{ m_isMouseMButton = b; }
	void setMouseRButton(bool b)	{ m_isMouseRButton = b; }
	void setMouseXButton(WPARAM wParam)
									{ 
										m_isMouseX1Button = 
											(wParam & MK_XBUTTON1) ? true : false;
										m_isMouseX2Button = 
											(wParam & MK_XBUTTON2) ? true : false;
									}

	int getMouseX() const			{ return m_mouseX; }
	int getMouseY() const			{ return m_mouseY; }
	int getMouseRawX() const		{ return m_mouseRawX; }
	int getMOuseRawY() const		{ return m_mouseRawY; }
	bool getMouseLButton() const	{ return m_isMouseLButton; }
	bool getMouseMButton() const	{ return m_isMouseMButton; }
	bool getMouseRButton() const	{ return m_isMouseRButton; }
	bool getMouseX1Button() const	{ return m_isMouseX1Button; }
	bool getMouseX2Button() const	{ return m_isMouseX2Button; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	void checkController();
	void readController();

	const ControllerState* getControllerState(UINT n);
	const WORD getGamepadButtons(UINT n);
	bool getGamepadDPadUp(UINT n);
	bool getGamepadDPadDown(UINT n);
	bool getGamepadDPadLeft(UINT n);
	bool getGamepadDPadRight(UINT n);
	bool getGamepadStart(UINT n);
	bool getGamepadBack(UINT n);
	bool getGamepadLeftThumb(UINT n);
	bool getGamepadRightThumb(UINT n);
	bool getGamepadLeftShoulder(UINT n);
	bool getGamepadRightShoulder(UINT n);
	bool getGamepadA(UINT n);
	bool getGamepadB(UINT n);
	bool getGamepadX(UINT n);
	bool getGamepadY(UINT n);
	BYTE getGamepadLeftTrigger(UINT n);
	BYTE getGamepadRightTrigger(UINT n);
	SHORT getGamepadThumbLX(UINT n);
	SHORT getGamepadThumbLY(UINT n);
	SHORT getGamepadThumbRX(UINT n);
	SHORT getGamepadThumbRY(UINT n);

	void gamePadVibrateLeft(UINT n, WORD speed, float sec);
	void gamePadVibrateRight(UINT n, WORD speed, float sec);
	void vibrateControllers(float frameTime);

private:
	bool				m_keysDown[NS_Input::KEYS_ARRAY_LEN];
	bool				m_keysPressed[NS_Input::KEYS_ARRAY_LEN];
	std::string			m_textIn;
	char				m_charIn;
	bool				m_isNewline;
	int					m_mouseX;
	int					m_mouseY;
	int					m_mouseRawX;
	int					m_mouseRawY;
	RAWINPUTDEVICE		m_RID[1];
	bool				m_isMouseCaptured;
	bool				m_isMouseLButton;
	bool				m_isMouseMButton;
	bool				m_isMouseRButton;
	bool				m_isMouseX1Button;
	bool				m_isMouseX2Button;
	ControllerState		m_controllerState[NS_Input::MAX_CONTROLLERS];
};

#endif