#pragma once

#ifndef _GUNNER_H
#define _GUNNER_H
#define WIN32_LEAN_AND_MEAN

#include "Constants.h"
#include "Entity.h"

namespace NS_Gunner
{
	const int   WIDTH = 64;
	const int   HEIGHT = 64;
	const int   X = GAME_WIDTH / 2 - WIDTH / 2;
	const int   Y = GAME_HEIGHT / 6 - HEIGHT;
	const int   EDGE_TOP = -10;
	const int   EDGE_BOTTOM = 10;
	const int   EDGE_LEFT = -28;
	const int   EDGE_RIGHT = 28;
	const float SPEED = 150;
	const float MAX_SPEED = 1000;
	const float ROTATION_RATE = 3.14f;
	const float MASS = 50.0f;
	const int   TEXTURE_COLS = 2;
	const int   START_FRAME = 0;
	const int   END_FRAME = 1;
	const float ANIMATION_DELAY = 0.25f;
	const float BOUNCINESS = 0.5f;
}

enum GunnerState
{
	NORMAL = 1,
	RUNNING,
	AIMING,
	STUCK,
	DEAD,
	INBUSH
};

class Gunner : public Entity
{
public:
	Gunner();
	~Gunner();

	virtual bool initialize(Game*, int, int, int, TextureManager*);

	void update(float);
	virtual void draw(COLOR_ARGB = NS_Graphics::WHITE);

	void setVelocityX(float x)		{ m_velocity.x = x; }
	void setVelocityY(float y)		{ m_velocity.y = y; }
	void setState(GunnerState s)	{ m_state = s; }

	GunnerState getState()			{ return m_state; }

private:
	GunnerState		m_state;
	Image			m_body01;
	Image			m_body02;
	Image			m_dead;
};

#endif