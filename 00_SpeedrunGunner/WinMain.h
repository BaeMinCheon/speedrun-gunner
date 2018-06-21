#pragma once

#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#include "SpeedrunGunner.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool createMainWindow(HWND*, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);