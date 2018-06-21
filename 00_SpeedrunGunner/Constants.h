#pragma once

#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define SAFE_DELETE(ptr)	\
{							\
	if (ptr)				\
	{						\
		delete(ptr);		\
		(ptr) = nullptr;	\
	}						\
}

#define SAFE_RELEASE(ptr)	\
{							\
	if (ptr)				\
	{						\
		(ptr)->Release();	\
		(ptr) = nullptr;	\
	}						\
}

#define SAFE_DELETE_ARRAY(ptr)	\
{								\
	if (ptr)					\
	{							\
		delete[](ptr);			\
		(ptr) = nullptr;		\
	}							\
}

#define SAFE_ON_LOST_DEVICE(ptr)	\
{									\
	if (ptr)						\
	{								\
		(ptr)->onLostDevice();		\
	}								\
}

#define SAFE_ON_RESET_DEVICE(ptr)	\
{									\
	if (ptr)						\
	{								\
		(ptr)->onResetDevice();		\
	}								\
}

#define TRANSCOLOR SETCOLOR_ARGB(0, 255, 0, 255)

const static char* CLASS_NAME = "Speedrun Gunner";
const static char* GAME_TITLE = "Speedrun Gunner";
const bool IS_FULLSCREEN = false;
const UINT GAME_WIDTH = 640;
const UINT GAME_HEIGHT = 480;

const UCHAR ESC_KEY = VK_ESCAPE;
const UCHAR ALT_KEY = VK_MENU;
const UCHAR ENTER_KEY = VK_RETURN;
const UCHAR CONSOLE_KEY = VK_OEM_3;
const UCHAR LEFT_KEY = VK_LEFT;
const UCHAR UP_KEY = VK_UP;
const UCHAR RIGHT_KEY = VK_RIGHT;
const UCHAR DOWN_KEY = VK_DOWN;

const static char* TILES = "pictures/tiles.txt";
const static char* GUNNER_TEXTURE = "pictures/gunner.png";
const static char* MAP = "map/M01.txt";
const static char* ENDLINE = "pictures/endLine.png";
const static char* BUSHANIM = "pictures/bushAnim.png";

const float PI = 3.141592f;
const float FRAME_RATE = 200.0f;
const float MIN_FRAME_RATE = 10.0f;
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE;
const float GRAVITY = 6.67428e-11f;
const float MASS_PLANET = 1.0e14f;
const float MASS_SHIP = 5.0f;

enum WEAPON
{
	TORPEDO,
	SHIP,
	PLANET
};

const static char* WAVE_BANK = "audio/Win/WavesExample1.xwb";
const static char* SOUND_BANK = "audio/Win/SoundsExample1.xsb";
const static char* BEEP1 = "beep1";
const static char* BEEP2 = "beep2";
const static char* BEEP3 = "beep3";
const static char* BEEP4 = "beep4";
const static char* HIT = "hit";

#endif