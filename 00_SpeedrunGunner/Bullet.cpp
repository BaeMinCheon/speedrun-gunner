#include <random>
#include "Bullet.h"

Bullet::Bullet()
	: Entity()
{
	m_isActive = false;

	m_spriteData.width = NS_Bullet::WIDTH;
	m_spriteData.height = NS_Bullet::HEIGHT;
	m_spriteData.rect.bottom = NS_Bullet::HEIGHT;
	m_spriteData.rect.right = NS_Bullet::WIDTH;

	m_cols = NS_Bullet::TEXTURE_COLS;
	m_frameDelay = NS_Bullet::ANIMATION_DELAY;
	m_startFrame = NS_Bullet::START_FRAME;
	m_endFrame = NS_Bullet::END_FRAME;
	m_currentFrame = m_startFrame;

	m_radius = NS_Bullet::COLLISION_RADIUS;
	m_isVisible = false;
	m_fireTimer = 0.0f;
	m_mass = NS_Bullet::MASS;
	m_collisionType = NS_Entity::CIRCLE;
}

Bullet::~Bullet()
{
	;
}

void Bullet::update(float frameTime)
{
	Image::update(frameTime);

	m_fireTimer -= frameTime;

	if (m_isVisible == false)
	{
		return;
	}

	if (m_fireTimer < 0)
	{
		m_isVisible = false;
		m_isActive = false;
	}

	m_spriteData.x += frameTime * m_velocity.x;
	m_spriteData.y += frameTime * m_velocity.y;
}

void Bullet::fire(Gunner* pG)
{
	static std::random_device RD;
	static std::mt19937_64 rand(RD());
	static std::uniform_int_distribution<int> range(0, 4);

	if (m_fireTimer <= 0.0f)
	{
		m_velocity.x = +(float)sin(pG->getRadians()) * NS_Bullet::SPEED;
		m_velocity.y = -(float)cos(pG->getRadians()) * NS_Bullet::SPEED;
		m_spriteData.x = pG->getCenterX() - m_spriteData.width / 2.0f;
		m_spriteData.y = pG->getCenterY() - m_spriteData.height / 2.0f;

		m_isVisible = true;
		m_isActive = true;
		m_fireTimer = NS_Bullet::FIRE_DELAY;

		if (pG->getState() != GunnerState::AIMING)
		{
			int delta = 0;
			switch (range(rand))
			{
			case 0:
				delta = -75;
				break;
			case 1:
				delta = +75;
				break;
			case 2:
				delta = -150;
				break;
			case 3:
				delta = +150;
				break;
			}
			m_velocity.x += delta;
		}
	}
}

void Bullet::hide()
{
	m_isVisible = false;
	m_isActive = false;
}