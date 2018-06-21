#pragma once

#ifndef _SPEEDRUNGUNNER_H
#define _SPEEDRUNGUNNER_H
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <fstream>
#include "Game.h"
#include "TextureManager.h"
#include "Image.h"
#include "Entity.h"
#include "Gunner.h"
#include "Bullet.h"

namespace NS_SpeedrunGunner
{
	const static char* FONT = "Arial Bold";
	const static COLOR_ARGB FONT_COLOR = NS_Graphics::YELLOW;
	const static int TILE_SIZE = 64;
	const static int MAP_HEIGHT = 64;
	const static int MAP_WIDTH = 14;
	const static float SCROLL_RATE = 10;
	const static float MAX_GUNNER_SPEED = 20;
	const static USHORT EMPTY = 0;
	const static USHORT MAX_COLLISION_TILE = 25;
	const static int TILE_EDGE_TOP = -TILE_SIZE / 2;
	const static int TILE_EDGE_BOTTOM = TILE_SIZE / 2;
	const static int TILE_EDGE_LEFT = -TILE_SIZE / 2;
	const static int TILE_EDGE_RIGHT = TILE_SIZE / 2;
	const static RECT TILE_EDGE = { TILE_EDGE_LEFT, TILE_EDGE_TOP, TILE_EDGE_RIGHT, TILE_EDGE_BOTTOM };
	const static float TILE_MASS = 10000.0f;
}

class SpeedrunGunner : public Game
{
public:
	SpeedrunGunner();
	virtual ~SpeedrunGunner();

	void initialize(HWND);

	void update() override;
	void processAI() override;
	void collision() override;
	void render() override;

	void releaseAll();
	void resetAll();

	void scoreCalc();
	void clientWaving();
	void playerRestart();
	void loadMap();

private:
	TextureManager		m_tileTM;
	Image				m_tileIMG;
	Entity				m_tileENT;

	TextureManager		m_gunnerTM;
	Gunner				m_gunner;

	TextureManager		m_objectTM;

	std::ifstream		m_mapFS;
	USHORT m_map[NS_SpeedrunGunner::MAP_HEIGHT][NS_SpeedrunGunner::MAP_WIDTH];

	TextDX				m_fontTime;

	float				m_mapX;
	float				m_mapY;

	bool				m_isStart;
	bool				m_isEnd;
	
	float				m_countTime;
	char				m_buffer[100];
	int					m_score;

	TextureManager		m_bushTM;
	Image				m_bushIMG;

	int					m_HP;
	float				m_deathTime;

	Bullet				m_bullet;
};

#endif