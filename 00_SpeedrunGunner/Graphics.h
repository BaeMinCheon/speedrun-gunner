#pragma once

#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include "Constants.h"
#include "GameError.h"

#define LP_3DDEVICE			LPDIRECT3DDEVICE9
#define LP_3D				LPDIRECT3D9
#define LP_SPRITE			LPD3DXSPRITE
#define LP_TEXTURE			LPDIRECT3DTEXTURE9
#define VECTOR2				D3DXVECTOR2
#define LP_DXFONT			LPD3DXFONT
#define LP_VERTEXBUFFER		LPDIRECT3DVERTEXBUFFER9

#define COLOR_ARGB	DWORD
#define SETCOLOR_ARGB(a, r, g, b)	\
	(COLOR_ARGB)	((a) & 0xff) << 24 | ((r) & 0xff) << 16 | ((g) & 0xff) << 8 | ((b) & 0xff)
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

namespace NS_Graphics
{
	const COLOR_ARGB ORANGE		=	D3DCOLOR_ARGB(255, 255, 165,   0);
	const COLOR_ARGB BROWN		=	D3DCOLOR_ARGB(255, 139,  69,  19);
	const COLOR_ARGB LTGRAY		=	D3DCOLOR_ARGB(255, 192, 192, 192);
	const COLOR_ARGB GRAY		=	D3DCOLOR_ARGB(255, 128, 128, 128);
	const COLOR_ARGB OLIVE		=	D3DCOLOR_ARGB(255, 128, 128,   0);
	const COLOR_ARGB PURPLE		=	D3DCOLOR_ARGB(255, 128,   0, 128);
	const COLOR_ARGB MAROON		=	D3DCOLOR_ARGB(255, 128,   0,   0);
	const COLOR_ARGB TEAL		=	D3DCOLOR_ARGB(255,   0, 128, 128);
	const COLOR_ARGB GREEN		=	D3DCOLOR_ARGB(255,   0, 128,   0);
	const COLOR_ARGB NAVY		=	D3DCOLOR_ARGB(255,   0,   0, 128);
	const COLOR_ARGB WHITE		=	D3DCOLOR_ARGB(255, 255, 255, 255);
	const COLOR_ARGB YELLOW		=	D3DCOLOR_ARGB(255, 255, 255,   0);
	const COLOR_ARGB MAGENTA	=	D3DCOLOR_ARGB(255, 255,   0, 255);
	const COLOR_ARGB RED		=	D3DCOLOR_ARGB(255, 255,   0,   0);
	const COLOR_ARGB CYAN		=	D3DCOLOR_ARGB(255,   0, 255, 255);
	const COLOR_ARGB LIME		=	D3DCOLOR_ARGB(255,   0, 255,   0);
	const COLOR_ARGB BLUE		=	D3DCOLOR_ARGB(255,   0,   0, 255);
	const COLOR_ARGB BLACK		=	D3DCOLOR_ARGB(255,   0,   0,   0);
	const COLOR_ARGB FILTER		=	D3DCOLOR_ARGB(	0,	 0,	  0,   0);
	const COLOR_ARGB ALPHA25	=	D3DCOLOR_ARGB( 64, 255, 255, 255);
	const COLOR_ARGB ALPHA50	=	D3DCOLOR_ARGB(128, 255, 255, 255);
	const COLOR_ARGB BACK_COLOR = NAVY;

	enum DISPLAY_MOE
	{
		TOGGLE,
		FULLSCREEN,
		WINDOW
	};
}

struct SpriteData
{
	int			width;
	int			height;
	float		x;
	float		y;
	float		scale;
	float		angle;
	RECT		rect;
	LP_TEXTURE	texture;
	bool		isFlipedHOR;
	bool		isFlipedVER;
};

struct VertexC
{
	float x;
	float y;
	float z;
	float RHW;
	unsigned long color;
};

class Graphics
{
public:
	Graphics();
	virtual ~Graphics();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	void		releaseAll();
	void		initialize(HWND, int, int, bool);
	HRESULT		loadTexture(const char*, COLOR_ARGB, UINT*, UINT*, LP_TEXTURE*);
	HRESULT		loadTextureSysMem(const char*, COLOR_ARGB, UINT*, UINT*, LP_TEXTURE*);
	HRESULT		showBackBuffer();
	bool		isAdapterCompatible();
	void		drawSprite(const SpriteData*, COLOR_ARGB = NS_Graphics::WHITE);
	HRESULT		reset();
	void		changeDisplayMode(NS_Graphics::DISPLAY_MOE = NS_Graphics::TOGGLE);
	HRESULT		createVertexBuffer(VertexC*, UINT, LP_VERTEXBUFFER*);
	bool		drawQuad(LP_VERTEXBUFFER*);

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	HRESULT		beginScene();
	HRESULT		endScene();
	void		beginSprite();
	void		endSprite();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	LP_3D			get3D()				{ return m_direct3d; }
	LP_3DDEVICE		getDevice()			{ return m_device3d; }
	LP_SPRITE		getSprite()			{ return m_sprite; }
	HDC				getDC()				{ return GetDC(m_hwnd); }
	HRESULT			getDeviceState()	{
											if (!m_device3d)
											{
												return E_FAIL;
											}
											else
											{
												return m_device3d->TestCooperativeLevel();
											}
										}
	bool			isFullscreen()		{ return m_isFullscreen; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	void			setBackColor(COLOR_ARGB c) { m_backColor = c; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	static float vector2Length(const VECTOR2* pV)
	{
		return D3DXVec2Length(pV);
	}
	static float vector2Dot(const VECTOR2* pV1, const VECTOR2* pV2)
	{
		return D3DXVec2Dot(pV1, pV2);
	}
	static void vector2Normalize(VECTOR2* pV)
	{
		D3DXVec2Normalize(pV, pV);
	}
	static VECTOR2* vector2Transform(VECTOR2* pV, D3DXMATRIX* pM)
	{
		return D3DXVec2TransformCoord(pV, pV, pM);
	}

private:
	LP_3D					m_direct3d;
	LP_3DDEVICE				m_device3d;
	LP_SPRITE				m_sprite;
	D3DPRESENT_PARAMETERS	m_d3dPP;
	D3DDISPLAYMODE			m_displayMode;

	HRESULT					m_HR;
	HWND					m_hwnd;
	bool					m_isFullscreen;
	int						m_width;
	int						m_height;
	COLOR_ARGB				m_backColor;

	void initD3DPP();
};

#endif