#include "Gunner.h"

Gunner::Gunner()
	: Entity()
{
	m_spriteData.width = NS_Gunner::WIDTH;
	m_spriteData.height = NS_Gunner::HEIGHT;
	m_spriteData.x = NS_Gunner::X;
	m_spriteData.y = NS_Gunner::Y;
	m_spriteData.rect.bottom = NS_Gunner::HEIGHT;
	m_spriteData.rect.right = NS_Gunner::WIDTH;
	
	m_edge.top = NS_Gunner::EDGE_TOP;
	m_edge.bottom = NS_Gunner::EDGE_BOTTOM;
	m_edge.left = NS_Gunner::EDGE_LEFT;
	m_edge.right = NS_Gunner::EDGE_RIGHT;
	
	m_velocity.x = m_velocity.y = 0;
	m_frameDelay = NS_Gunner::ANIMATION_DELAY;
	m_startFrame = NS_Gunner::START_FRAME;
	m_endFrame = NS_Gunner::END_FRAME;
	m_currentFrame = m_startFrame;
	
	m_radius = static_cast<float>(NS_Gunner::WIDTH / 3.0);
	m_collisionType = NS_Entity::ROTATED_BOX;
	m_mass = NS_Gunner::MASS;
	m_bounciness = NS_Gunner::BOUNCINESS;

	m_state = GunnerState::NORMAL;
}

Gunner::~Gunner()
{
	;
}

bool Gunner::initialize(Game* pG, int wid, int hei, int nCols, TextureManager* pTM)
{
	m_body01.initialize(pG->getGraphics(), wid, hei, nCols, pTM);
	m_body01.setCurrentFrame(2);

	m_body02.initialize(pG->getGraphics(), wid, hei, nCols, pTM);
	m_body02.setCurrentFrame(3);

	m_dead.initialize(pG->getGraphics(), wid, hei, nCols, pTM);
	m_dead.setFrames(4, 5);
	m_dead.setCurrentFrame(4);
	m_dead.setFrameDelay(1.0f);

	return Entity::initialize(pG, wid, hei, nCols, pTM);
}

void Gunner::update(float frameTime)
{
	Entity::update(frameTime);

	bool isMovingLeft = false;
	bool isMovingRight = false;
	bool isAiming = false;
	bool isRunning = false;

	{	// dead process
		if (m_state == GunnerState::DEAD)
		{
			m_spriteData.angle = 0.0f;
			m_dead.update(frameTime);
			return;
		}
	}

	{	// key input
		isMovingLeft = m_pInput->isKeyDown(LEFT_KEY);
		isMovingRight = m_pInput->isKeyDown(RIGHT_KEY);
		isAiming = m_pInput->isKeyDown(0x41);
		isRunning = m_pInput->isKeyDown(0x57);
	}

	{	// determine state
		if (m_state == GunnerState::STUCK)
		{
			;
		}
		else if (isAiming)
		{
			m_state = GunnerState::AIMING;
		}
		else if (isRunning)
		{
			m_state = GunnerState::RUNNING;
		}
		else
		{
			m_state = GunnerState::NORMAL;
		}
	}

	{	// gunner is moving
		if (isMovingLeft)
		{
			m_velocity.x = -NS_Gunner::SPEED;
			m_spriteData.angle = -NS_Gunner::ROTATION_RATE * 1 / 4;
		}
		if (isMovingRight)
		{
			m_velocity.x = +NS_Gunner::SPEED;
			m_spriteData.angle = +NS_Gunner::ROTATION_RATE * 1 / 4;
		}
		m_velocity.y = -NS_Gunner::SPEED;
	}

	{	// state process
		switch (m_state)
		{
		case GunnerState::AIMING:
			m_frameDelay = NS_Gunner::ANIMATION_DELAY * 2;
			m_velocity.x /= 2.0f;
			m_velocity.y /= 2.0f;
			break;

		case GunnerState::RUNNING:
			m_frameDelay = NS_Gunner::ANIMATION_DELAY / 2;
			m_velocity.x *= 1.5f;
			m_velocity.y *= 1.5f;
			break;

		case GunnerState::STUCK:
			m_velocity.x /= 4.0f;
			m_velocity.y /= 4.0f;
			break;

		default:
			m_frameDelay = NS_Gunner::ANIMATION_DELAY;
			break;
		}
	}

	{	// velocity calculation
		if (isMovingLeft && isMovingRight)
		{
			m_velocity.x = 0;
			m_spriteData.angle = 0.0f;
		}
		else if (isMovingLeft || isMovingRight)
		{
			m_spriteData.x += m_velocity.x * frameTime;
		}
		else
		{
			m_velocity.x = 0;
			m_spriteData.angle = 0.0f;
		}
		m_spriteData.y += m_velocity.y * frameTime;
	}
}

void Gunner::draw(COLOR_ARGB color)
{
	if (m_state == GunnerState::DEAD)
	{
		m_dead.draw(m_spriteData);
		return;
	}

	Image::draw(color, 0);

	if(m_state == GunnerState::RUNNING
	|| m_state == GunnerState::STUCK)
	{
		m_body02.draw(m_spriteData);
	}
	else
	{
		m_body01.draw(m_spriteData);
	}
}