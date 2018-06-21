#include "Graphics.h"

Graphics::Graphics()
	: m_direct3d{ nullptr }, m_device3d{ nullptr }, m_sprite{ nullptr },
	m_isFullscreen{ false }, m_width{ GAME_WIDTH }, m_height{ GAME_HEIGHT },
	m_backColor{ NS_Graphics::BACK_COLOR }
{
	;
}

Graphics::~Graphics()
{
	releaseAll();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Graphics::releaseAll()
{
	SAFE_RELEASE(m_sprite);
	SAFE_RELEASE(m_device3d);
	SAFE_RELEASE(m_direct3d);
}

void Graphics::initialize(HWND hwnd, int wid, int hei, bool full)
{
	m_hwnd = hwnd;
	m_width = wid;
	m_height = hei;
	m_isFullscreen = full;

	m_direct3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_direct3d)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing Direct3D"));
	}

	initD3DPP();

	if (m_isFullscreen)
	{
		if (isAdapterCompatible())
		{
			m_d3dPP.FullScreen_RefreshRateInHz = m_displayMode.RefreshRate;
		}
		else
		{
			throw(GameError(NS_GameError::FATAL_ERROR,
				"No support on this resolution or format"));
		}
	}

	D3DCAPS9 caps;
	m_HR = m_direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	DWORD behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
		behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	m_HR = m_direct3d->CreateDevice
	(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hwnd,
		behavior,
		&m_d3dPP,
		&m_device3d
	);
	if (FAILED(m_HR))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on creating Direct3D device"));
	}

	m_HR = D3DXCreateSprite(m_device3d, &m_sprite);
	if (FAILED(m_HR))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on creating Direct3D sprite"));
	}

	m_device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

HRESULT Graphics::loadTexture(const char* filename, COLOR_ARGB transColor,
	UINT* wid, UINT* hei, LP_TEXTURE* pTexture)
{
	D3DXIMAGE_INFO info;

	try
	{
		if (!filename)
		{
			*pTexture = nullptr;
			return D3DERR_INVALIDCALL;
		}

		m_HR = D3DXGetImageInfoFromFile(filename, &info);
		if (m_HR != D3D_OK)
		{
			return m_HR;
		}

		*wid = info.Width;
		*hei = info.Height;

		m_HR = D3DXCreateTextureFromFileEx
		(
			m_device3d,
			filename,
			info.Width,
			info.Height,
			1,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			transColor,
			&info,
			nullptr,
			pTexture
		);
	}
	catch (...)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on loading texture"));
	}

	return m_HR;
}

HRESULT Graphics::loadTextureSysMem(const char* filename, COLOR_ARGB transColor,
	UINT* wid, UINT* hei, LP_TEXTURE* pTexture)
{
	D3DXIMAGE_INFO info;

	try
	{
		if (!filename)
		{
			*pTexture = nullptr;
			return D3DERR_INVALIDCALL;
		}

		m_HR = D3DXGetImageInfoFromFile(filename, &info);
		if (m_HR != D3D_OK)
		{
			return m_HR;
		}

		*wid = info.Width;
		*hei = info.Height;

		m_HR = D3DXCreateTextureFromFileEx
		(
			m_device3d,
			filename,
			info.Width,
			info.Height,
			1,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_SYSTEMMEM,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			transColor,
			&info,
			nullptr,
			pTexture
		);
	}
	catch (...)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on loading texture in SysMem"));
	}

	return m_HR;
}

HRESULT Graphics::showBackBuffer()
{
	return m_device3d->Present(nullptr, nullptr, nullptr, nullptr);
}

bool Graphics::isAdapterCompatible()
{
	UINT mode = m_direct3d->
		GetAdapterModeCount(D3DADAPTER_DEFAULT, m_d3dPP.BackBufferFormat);
	for (UINT i = 0; i < mode; ++i)
	{
		m_direct3d->EnumAdapterModes
		(
			D3DADAPTER_DEFAULT,
			m_d3dPP.BackBufferFormat,
			i,
			&m_displayMode
		);

		if (m_displayMode.Height == m_d3dPP.BackBufferHeight &&
			m_displayMode.Width == m_d3dPP.BackBufferWidth &&
			m_displayMode.RefreshRate >= m_d3dPP.FullScreen_RefreshRateInHz)
		{
			return true;
		}
	}
	return false;
}

void Graphics::drawSprite(const SpriteData* pSprite, COLOR_ARGB color)
{
	if (!pSprite->texture)
	{
		return;
	}

	D3DXVECTOR2 center = D3DXVECTOR2(static_cast<float>(pSprite->width / 2 * pSprite->scale),
		static_cast<float>(pSprite->height / 2 * pSprite->scale));
	D3DXVECTOR2 trans = D3DXVECTOR2(static_cast<float>(pSprite->x),
		static_cast<float>(pSprite->y));
	D3DXVECTOR2 scale(pSprite->scale, pSprite->scale);

	if (pSprite->isFlipedHOR)
	{
		scale.x *= -1;
		center.x -= static_cast<float>(pSprite->width * pSprite->scale);
		trans.x += static_cast<float>(pSprite->width * pSprite->scale);
	}
	if (pSprite->isFlipedVER)
	{
		scale.y *= -1;
		center.y -= static_cast<float>(pSprite->height * pSprite->scale);
		trans.y += static_cast<float>(pSprite->height * pSprite->scale);
	}

	D3DXMATRIX matrix;
	D3DXMatrixTransformation2D
	(
		&matrix,
		nullptr,
		0.0f,
		&scale,
		&center,
		static_cast<float>(pSprite->angle),
		&trans
	);

	m_sprite->SetTransform(&matrix);
	m_sprite->Draw(pSprite->texture, &pSprite->rect, nullptr, nullptr, color);
}

HRESULT Graphics::reset()
{
	initD3DPP();

	m_sprite->OnLostDevice();

	m_HR = m_device3d->Reset(&m_d3dPP);
	m_device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_sprite->OnResetDevice();

	return m_HR;
}

void Graphics::changeDisplayMode(NS_Graphics::DISPLAY_MOE mode)
{
	try
	{
		switch (mode)
		{
		case NS_Graphics::FULLSCREEN:
			if (m_isFullscreen)
			{
				return;
			}
			else
			{
				m_isFullscreen = true;
			}
			break;
		case NS_Graphics::WINDOW:
			if (!m_isFullscreen)
			{
				return;
			}
			else
			{
				m_isFullscreen = false;
			}
			break;
		default:
			m_isFullscreen = !m_isFullscreen;
			break;
		}

		reset();

		if (m_isFullscreen)
		{
			SetWindowLong(m_hwnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
		}
		else
		{
			SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(m_hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
				SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			RECT clientRect;
			GetClientRect(m_hwnd, &clientRect);
			MoveWindow
			(
				m_hwnd,
				0,
				0,
				GAME_WIDTH + (GAME_WIDTH - clientRect.right),
				GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),
				true
			);
		}
	}
	catch (...)
	{
		SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hwnd, HWND_TOP, 0, 0, GAME_WIDTH, GAME_HEIGHT,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		RECT clientRect;
		GetClientRect(m_hwnd, &clientRect);
		MoveWindow
		(
			m_hwnd,
			0,
			0,
			GAME_WIDTH + (GAME_WIDTH - clientRect.right),
			GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),
			true
		);
	}
}

HRESULT Graphics::createVertexBuffer(VertexC* pVC, UINT size, LP_VERTEXBUFFER* pVB)
{
	if (FAILED(m_device3d->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY,
		D3DFVF_VERTEX, D3DPOOL_DEFAULT, pVB, nullptr)))
	{
		return E_FAIL;
	}

	void* ptr;
	if (FAILED((*pVB)->Lock(0, size, &ptr, 0)))
	{
		return E_FAIL;
	}

	memcpy(ptr, pVC, size);
	(*pVB)->Unlock();

	return S_OK;
}

bool Graphics::drawQuad(LP_VERTEXBUFFER* pVB)
{
	if (!pVB)
	{
		return false;
	}

	m_device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	m_device3d->SetStreamSource(0, *pVB, 0, sizeof(VertexC));
	m_device3d->SetFVF(D3DFVF_VERTEX);
	HRESULT HR = m_device3d->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	m_device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	return FAILED(HR) ? false : true;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

HRESULT Graphics::beginScene()
{
	if (!m_device3d)
	{
		return E_FAIL;
	}
	else
	{
		m_device3d->Clear(0, nullptr, D3DCLEAR_TARGET, m_backColor, 1.0f, 0);

		return m_device3d->BeginScene();
	}
}

HRESULT Graphics::endScene()
{
	if (m_device3d)
	{
		return m_device3d->EndScene();
	}
	else
	{
		return E_FAIL;
	}
}

void Graphics::beginSprite()
{
	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void Graphics::endSprite()
{
	m_sprite->End();
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

void Graphics::initD3DPP()
{
	try
	{
		ZeroMemory(&m_d3dPP, sizeof(m_d3dPP));
		m_d3dPP.BackBufferWidth = m_width;
		m_d3dPP.BackBufferHeight = m_height;
		m_d3dPP.BackBufferFormat = D3DFMT_UNKNOWN;
		if (m_isFullscreen)
		{
			m_d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		m_d3dPP.BackBufferCount = 1;
		m_d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dPP.hDeviceWindow = m_hwnd;
		m_d3dPP.Windowed = !m_isFullscreen;
		m_d3dPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	catch (...)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing D3D PP"));
	}
}