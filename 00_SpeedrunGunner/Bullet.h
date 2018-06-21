#pragma once

#ifndef _BULLET_H
#define _BULLET_H
#define WIN32_LEAN_AND_MEAN

#include "Constants.h"
#include "Gunner.h"

namespace NS_Bullet
{
	const int   WIDTH = 64;
	const int   HEIGHT = 64;
	const int   COLLISION_RADIUS = 4;
	const float SPEED = 400;
	const float MASS = 300.0f;
	const float FIRE_DELAY = 1.0f;
	const int   TEXTURE_COLS = 2;
	const int   START_FRAME = 6;
	const int   END_FRAME = 6;
	const float ANIMATION_DELAY = 1.0f;
}

class Bullet : public Entity
{
public:
	Bullet();
	virtual ~Bullet();

	void update(float);
	void fire(Gunner*);
	void hide();

private:
	float	m_fireTimer;
};

#endif