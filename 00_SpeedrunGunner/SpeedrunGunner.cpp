#include <random>

#include "SpeedrunGunner.h"

SpeedrunGunner::SpeedrunGunner()
{
	m_mapX = m_mapY = m_countTime = 0.0f;
	m_HP = 0;
	m_isStart = m_isEnd = false;
	m_deathTime = 2.0f;
}

SpeedrunGunner::~SpeedrunGunner()
{
	releaseAll();
}

void SpeedrunGunner::initialize(HWND hwnd)
{
	Game::initialize(hwnd);

	if (!m_tileTM.initialize(m_pGraphics, TILES))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing tile texture"));
	}
	if (!m_tileIMG.initialize(m_pGraphics, 0, 0, 0, &m_tileTM))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing tile image"));
	}

	if (!m_gunnerTM.initialize(m_pGraphics, GUNNER_TEXTURE))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing gunner texture"));
	}
	if (!m_gunner.initialize(this, NS_Gunner::WIDTH, NS_Gunner::HEIGHT,
		NS_Gunner::TEXTURE_COLS, &m_gunnerTM))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing gunner"));
	}
	m_gunner.setFrames(NS_Gunner::START_FRAME, NS_Gunner::END_FRAME);
	m_gunner.setCurrentFrame(NS_Gunner::START_FRAME);
	m_gunner.setMass(NS_Gunner::MASS);
	m_gunner.setDegrees(0.0f);
	m_gunner.setX(static_cast<float>(GAME_WIDTH / 2 - m_gunner.getWidth() / 2));
	m_gunner.setY(static_cast<float>(GAME_HEIGHT - m_gunner.getHeight()));
	m_gunner.setCollisionType(NS_Entity::CIRCLE);

	loadMap();
	m_mapX = -NS_SpeedrunGunner::TILE_SIZE * 2;
	m_mapY = -NS_SpeedrunGunner::TILE_SIZE * (NS_SpeedrunGunner::MAP_HEIGHT - 5);

	m_pMessageDialog->setButtonType(NS_MessageDialog::JUST_OK);
	m_pMessageDialog->setVisible(false);

	m_fontTime.initialize(m_pGraphics, 20, false, false,
		&std::string(NS_SpeedrunGunner::FONT));
	m_fontTime.setFontColor(NS_SpeedrunGunner::FONT_COLOR);

	if (!m_bushTM.initialize(m_pGraphics, BUSHANIM))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing tile texture"));
	}
	if (!m_bushIMG.initialize(m_pGraphics, 640, 480, 0, &m_bushTM))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing tile image"));
	}
	m_bushIMG.setFrames(0, 5);
	m_bushIMG.setCurrentFrame(0);
	m_bushIMG.setFrameDelay(0.1f);

	if (!m_bullet.initialize(this, NS_Bullet::WIDTH, NS_Bullet::HEIGHT,
		NS_Bullet::TEXTURE_COLS, &m_gunnerTM))
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on initializing bullet"));
	}
	m_bullet.setFrames(NS_Bullet::START_FRAME, NS_Bullet::END_FRAME);
	m_bullet.setCurrentFrame(NS_Bullet::START_FRAME);
}

void SpeedrunGunner::update()
{
	int ratioY = 6;
	float gunnerX;
	float gunnerY;

	{	// check start & end
		if (!m_isStart)
		{
			if (m_pInput->isKeyDown(VK_SPACE))
			{
				m_isStart = true;

				m_pInput->clearAll();

				m_HP = 3;
			}
			else
			{
				return;
			}
		}
		else if (m_isEnd)
		{
			m_pMessageDialog->setVisible(true);

			if (m_pMessageDialog->getButtonClicked() == 1)
			{
				loadMap();

				m_gunner.setX(static_cast<float>(GAME_WIDTH / 2 - m_gunner.getWidth() / 2));
				m_gunner.setY(static_cast<float>(GAME_HEIGHT - m_gunner.getHeight()));
				m_mapX = -NS_SpeedrunGunner::TILE_SIZE * 2;
				m_mapY = -NS_SpeedrunGunner::TILE_SIZE * (NS_SpeedrunGunner::MAP_HEIGHT - 5);

				m_isStart = m_isEnd = false;
				m_countTime = 0.0f;
				m_bullet.hide();

				m_pMessageDialog->setVisible(false);
				m_pMessageDialog->setButtonClicked(false);
			}
			else
			{
				return;
			}
		}
		else
		{
			m_countTime += m_frameTime;
		}
	}

	{	// update animation
		m_gunner.update(m_frameTime);
		m_bullet.update(m_frameTime);
		m_bushIMG.update(m_frameTime);

		if (m_gunner.getState() == GunnerState::DEAD)
		{
			if (m_deathTime > 0.0f)
			{
				m_deathTime -= m_frameTime;
				return;
			}
			else
			{
				m_deathTime = 2.0f;
				playerRestart();
			}
		}
		else if (m_pInput->isKeyDown(0x53))
		{
			if (m_gunner.getState() == GunnerState::NORMAL)
			{
				m_bullet.fire(&m_gunner);
			}
			else if (m_gunner.getState() == GunnerState::AIMING)
			{
				m_bullet.fire(&m_gunner);
			}
		}
	}
	
	{	// update movement
		gunnerX = m_gunner.getX();
		if (gunnerX < NS_SpeedrunGunner::TILE_SIZE * 2)
		{
			if (m_mapX <= 0)
			{
				m_mapX += NS_SpeedrunGunner::TILE_SIZE * 2 - gunnerX;
			}

			m_gunner.setX(NS_SpeedrunGunner::TILE_SIZE * 2);
		}
		else if (gunnerX > GAME_WIDTH - NS_SpeedrunGunner::TILE_SIZE * 3)
		{
			if (m_mapX > -NS_SpeedrunGunner::TILE_SIZE * 4)
			{
				m_mapX -= (gunnerX - GAME_WIDTH) + NS_SpeedrunGunner::TILE_SIZE * 3;
			}

			m_gunner.setX(GAME_WIDTH - NS_SpeedrunGunner::TILE_SIZE * 3);
		}

		gunnerY = m_gunner.getY();
		if (gunnerY < NS_SpeedrunGunner::TILE_SIZE * ratioY)
		{
			m_mapY += NS_SpeedrunGunner::TILE_SIZE * ratioY - gunnerY;
			m_gunner.setY(static_cast<float>(NS_SpeedrunGunner::TILE_SIZE * ratioY));
		}
	}
}

void SpeedrunGunner::processAI()
{
	;
}

void SpeedrunGunner::collision()
{
	if (m_gunner.getState() == GunnerState::DEAD)
	{
		return;
	}

	UINT GRow = static_cast<UINT>(
		(m_gunner.getY() + NS_SpeedrunGunner::TILE_SIZE / 2 - m_mapY)
		/ NS_SpeedrunGunner::TILE_SIZE);
	UINT GCol = static_cast<UINT>(
		(m_gunner.getX() + NS_SpeedrunGunner::TILE_SIZE / 2 - m_mapX)
		/ NS_SpeedrunGunner::TILE_SIZE);
	UINT BRow = static_cast<UINT>((m_bullet.getY() + NS_SpeedrunGunner::TILE_SIZE / 2 - m_mapY)
		/ NS_SpeedrunGunner::TILE_SIZE);
	UINT BCol = static_cast<UINT>((m_bullet.getX() + NS_SpeedrunGunner::TILE_SIZE / 2 - m_mapX)
		/ NS_SpeedrunGunner::TILE_SIZE);

	if (m_gunner.getState() == GunnerState::STUCK)
	{
		m_gunner.setState(GunnerState::NORMAL);
	}
	switch (m_map[GRow][GCol])
	{
	case 2:
	case 3:
	case 4:
		if (m_HP == 1)
		{
			m_gunner.setState(GunnerState::DEAD);
		}
		else
		{
			--m_HP;
			m_mapY -= NS_SpeedrunGunner::TILE_SIZE * 3;
			clientWaving();
		}
		break;
	case 5:
		m_gunner.setState(GunnerState::INBUSH);
		break;
	case 6:
	case 7:
		m_gunner.setState(GunnerState::STUCK);
		break;
	case 25:
		scoreCalc();
		m_isEnd = true;
		break;
	case 26:
		m_gunner.setState(GunnerState::DEAD);
		break;
	default:
		break;
	}

	if (m_bullet.getActive())
	{
		if (m_map[BRow][BCol] == 2)
		{
			m_map[BRow][BCol] = 3;
			m_bullet.hide();
		}
		else if (m_map[BRow][BCol] == 3)
		{
			m_map[BRow][BCol] = 1;
			m_bullet.hide();
		}
	}
}

void SpeedrunGunner::render()
{
	m_pGraphics->beginSprite();

	if (m_gunner.getState() == GunnerState::INBUSH)
	{
		m_bushIMG.draw();
	}
	else
	{
		for (int r = 0; r < NS_SpeedrunGunner::MAP_HEIGHT; ++r)
		{
			for (int c = 0; c < NS_SpeedrunGunner::MAP_WIDTH; ++c)
			{
				if (m_map[r][c] >= 0)
				{
					m_tileIMG.setX(
						static_cast<float>(c * NS_SpeedrunGunner::TILE_SIZE) + m_mapX);
					m_tileIMG.setY(
						static_cast<float>(r * NS_SpeedrunGunner::TILE_SIZE) + m_mapY);

					if (m_tileIMG.getX() > -NS_SpeedrunGunner::TILE_SIZE
						&& m_tileIMG.getX() < GAME_WIDTH
						&& m_tileIMG.getY() > -NS_SpeedrunGunner::TILE_SIZE
						&& m_tileIMG.getY() < GAME_HEIGHT)
					{
						m_tileIMG.draw(static_cast<UINT>(m_map[r][c]));
					}
				}
			}
		}
		m_bullet.draw();
		m_gunner.draw();
	}

	_snprintf_s(m_buffer, 100, "TIME : %.2f    HP : %d", m_countTime, m_HP);
	m_fontTime.print(&std::string(m_buffer), 10, 10);


	m_pGraphics->endSprite();
}

void SpeedrunGunner::releaseAll()
{
	m_gunnerTM.onLostDevice();
	m_tileTM.onLostDevice();
	m_bushTM.onLostDevice();

	m_fontTime.onLostDevice();

	Game::releaseAll();
}

void SpeedrunGunner::resetAll()
{
	m_gunnerTM.onResetDevice();
	m_tileTM.onResetDevice();
	m_bushTM.onResetDevice();

	m_fontTime.onResetDevice();

	Game::resetAll();
}

void SpeedrunGunner::scoreCalc()
{
	if (m_countTime > 60.0f)
	{
		m_score = 0;
	}
	else
	{
		m_score = static_cast<int>(100.0f - (m_countTime - 20.0f) * 2.5f);

		if (m_score < 0)
		{
			m_score = 0;
		}
	}

	m_pMessageDialog->print(
		&(std::string("YOUR SCORE IS ") + std::to_string(m_score)));
}

void SpeedrunGunner::clientWaving()
{
	RECT originRect;
	RECT wavingRect;
	GetWindowRect(m_hwnd, &originRect);
	wavingRect = originRect;

	static std::random_device RD;
	static std::mt19937_64 rand(RD());
	static std::uniform_int_distribution<int> range(5, 15);

	for (int i = 0; i < 6; ++i)
	{
		if (i & 1)
		{
			MoveWindow(m_hwnd, wavingRect.left + range(rand), wavingRect.top + range(rand),
				GAME_WIDTH, GAME_HEIGHT, true);
		}
		else
		{
			MoveWindow(m_hwnd, wavingRect.left - range(rand), wavingRect.top - range(rand),
				GAME_WIDTH, GAME_HEIGHT, true);
		}
		Sleep(5);
	}

	MoveWindow(m_hwnd, originRect.left, originRect.top,
		GAME_WIDTH, GAME_HEIGHT, true);
}

void SpeedrunGunner::playerRestart()
{
	m_gunner.setState(GunnerState::NORMAL);
	m_HP = 3;

	m_gunner.setX(static_cast<float>(GAME_WIDTH / 2 - m_gunner.getWidth() / 2));
	m_gunner.setY(static_cast<float>(GAME_HEIGHT - m_gunner.getHeight()));

	m_mapX = -NS_SpeedrunGunner::TILE_SIZE * 2;
	m_mapY = -NS_SpeedrunGunner::TILE_SIZE * (NS_SpeedrunGunner::MAP_HEIGHT - 5);
}

void SpeedrunGunner::loadMap()
{
	m_mapFS.open(MAP);
	if (!m_mapFS)
	{
		throw(GameError(NS_GameError::FATAL_ERROR,
			"Error on loading map data"));
	}
	for (int r = 0; r < NS_SpeedrunGunner::MAP_HEIGHT; ++r)
	{
		for (int c = 0; c < NS_SpeedrunGunner::MAP_WIDTH; ++c)
		{
			m_mapFS >> m_map[r][c];
		}
	}
	m_mapFS.close();
}