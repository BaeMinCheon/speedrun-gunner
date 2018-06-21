#include "Entity.h"

Entity::Entity()
	: Image()
{
	m_edge.left = m_edge.top = -1;
	m_edge.right = m_edge.bottom = 1;

	m_radius = m_mass = 1.0f;
	m_velocity.x = m_velocity.y = 0.0f;
	m_deltaV.x = m_deltaV.y = 0.0f;

	m_isActive = true;
	m_isRotatedBox = false;
	m_collisionType = NS_Entity::CIRCLE;

	m_health = 100;
	m_gravity = NS_Entity::GRAVITY;
	m_bounciness = 1.0f;
}

Entity::~Entity()
{
	;
}

bool Entity::initialize(Game* pG, int wid, int hei, int nCols, TextureManager* pTM)
{
	m_pInput = pG->getInput();

	return Image::initialize(pG->getGraphics(), wid, hei, nCols, pTM);
}

void Entity::activate()
{
	m_isActive = true;
}

void Entity::update(float frameTime)
{
	m_velocity += m_deltaV;
	m_deltaV.x = m_deltaV.y = 0;
	
	Image::update(frameTime);

	m_isRotatedBox = false;
}

void Entity::processAI(float frameTime, Entity* pE)
{
	;
}

bool Entity::isOutsideRect(RECT* rect)
{
	return
	(
		m_spriteData.x + m_spriteData.width * getScale() < rect->left ||
		m_spriteData.x > rect->right ||
		m_spriteData.y + m_spriteData.height * getScale() < rect->top ||
		m_spriteData.y > rect->bottom
	);
}

bool Entity::collidesWith(Entity* pE, VECTOR2* pV)
{
	if (!m_isActive || !(pE->getActive()))
	{
		return false;
	}
	else if (m_collisionType == NS_Entity::CIRCLE &&
			pE->getCollisionType() == NS_Entity::CIRCLE)
	{
		return collideCircle(pE, pV);
	}
	else if (m_collisionType == NS_Entity::BOX &&
			pE->getCollisionType() == NS_Entity::BOX)
	{
		return collideBox(pE, pV);
	}
	else if (m_collisionType != NS_Entity::CIRCLE &&
			pE->getCollisionType() != NS_Entity::CIRCLE)
	{
		return collideRotatedBox(pE, pV);
	}
	else if (m_collisionType == NS_Entity::CIRCLE)
	{
		bool isCollide = pE->collideRotatedBoxCircle(this, pV);
		*pV *= -1;

		return isCollide;
	}
	else
	{
		return collideRotatedBoxCircle(pE, pV);
	}
}

void Entity::damage(int weapon)
{
	;
}

void Entity::bounce(VECTOR2* pV, Entity* pE)
{
	VECTOR2 Vdiff = pE->getVelocity() - m_velocity;
	VECTOR2 cUV = *pV;             

	Graphics::vector2Normalize(&cUV);
	float cUVdotVdiff = Graphics::vector2Dot(&cUV, &Vdiff);

	float massRatio = 2.0f;
	if (getMass() != 0)
	{
		massRatio *= (pE->getMass() / (getMass() + pE->getMass()));
	}

	if (cUVdotVdiff > 0)
	{
		setX(getX() - cUV.x * massRatio);
		setY(getY() - cUV.y * massRatio);
	}
	else
	{
		m_deltaV += ((massRatio * cUVdotVdiff) * cUV);
		m_deltaV *= m_bounciness;
	}
}

void Entity::gravityForce(Entity* pE, float frameTime)
{
	if (!m_isActive || !(pE->getActive()))
	{
		return;
	}

	m_RR = pow((pE->getCenterX() - getCenterX()), 2) +
		pow((pE->getCenterY() - getCenterY()), 2);
	m_force = m_gravity * pE->getMass() * m_mass / m_RR;

	VECTOR2 gravityV(pE->getCenterX() - getCenterX(),
		pE->getCenterY() - getCenterY());
	Graphics::vector2Normalize(&gravityV);

	gravityV *= m_force * frameTime;
	m_velocity += gravityV;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

bool Entity::collideCircle(Entity* pE, VECTOR2* pV)
{
	m_distSquared = *getCenter() - *pE->getCenter();
	m_distSquared.x = m_distSquared.x * m_distSquared.x;      
	m_distSquared.y = m_distSquared.y * m_distSquared.y;

	m_sumRadiiSquared = (m_radius*getScale()) + (pE->m_radius * pE->getScale());
	m_sumRadiiSquared *= m_sumRadiiSquared;                

	if (m_distSquared.x + m_distSquared.y <= m_sumRadiiSquared)
	{
		*pV = *pE->getCenter() - *getCenter();
		return true;
	}
	else
	{
		return false;
	}
}

bool Entity::collideBox(Entity* pE, VECTOR2* pV)
{
	if (!m_isActive || !pE->getActive())
	{
		return false;
	}
	else if
		(
		(getCenterX() + m_edge.right * getScale() >=
			pE->getCenterX() + pE->getEdge().left * pE->getScale())
		&&
		(getCenterX() + m_edge.left * getScale() <=
			pE->getCenterX() + pE->getEdge().right * pE->getScale())
		&&
		(getCenterY() + m_edge.bottom * getScale() >=
			pE->getCenterY() + pE->getEdge().top * pE->getScale())
		&&
		(getCenterY() + m_edge.top * getScale() <=
			pE->getCenterY() + pE->getEdge().bottom * pE->getScale())
		)
	{
		// set collision vector
		*pV = *pE->getCenter() - *getCenter();
		return true;
	}
	else
	{
		return false;
	}
}

bool Entity::collideRotatedBox(Entity* pE, VECTOR2* pV)
{
	computeRotatedBox();
	pE->computeRotatedBox();

	if (projectionsOverlap(pE) && pE->projectionsOverlap(this))
	{
		*pV = *pE->getCenter() - *getCenter();
		return true;
	}
	else
	{
		return false;
	}
}

bool Entity::projectionsOverlap(Entity* pE)
{
	float projection, min01, max01, min03, max03;

	projection = m_pGraphics->vector2Dot(&m_edge01, pE->getCorner(0));
	min01 = projection;
	max01 = projection;

	for (int c = 1; c<4; c++)
	{
		projection = m_pGraphics->vector2Dot(&m_edge01, pE->getCorner(c));
		if (projection < min01)
		{
			min01 = projection;
		}
		else if (projection > max01)
		{
			max01 = projection;
		}
	}
	if (max01 < m_edge01Min || m_edge01Max < min01)
	{
		return false;
	}

	projection = m_pGraphics->vector2Dot(&m_edge03, pE->getCorner(0));
	min03 = projection;
	max03 = projection;

	for (int c = 1; c<4; c++)
	{
		projection = m_pGraphics->vector2Dot(&m_edge03, pE->getCorner(c));
		if (projection < min03)
		{
			min03 = projection;
		}
		else if (projection > max03)
		{
			max03 = projection;
		}
	}
	if (max03 < m_edge03Min || m_edge03Max < min03)
	{
		return false;
	}

	return true;                            
}

bool Entity::collideRotatedBoxCircle(Entity* pE, VECTOR2* pV)
{
	float min01, min03, max01, max03, center01, center03;

	computeRotatedBox();

	center01 = m_pGraphics->vector2Dot(&m_edge01, pE->getCenter());
	min01 = center01 - pE->getRadius() * pE->getScale();
	max01 = center01 + pE->getRadius() * pE->getScale();
	if (min01 > m_edge01Max || max01 < m_edge01Min)
	{
		return false;
	}

	center03 = m_pGraphics->vector2Dot(&m_edge03, pE->getCenter());
	min03 = center03 - pE->getRadius() * pE->getScale();
	max03 = center03 + pE->getRadius() * pE->getScale();
	if (min03 > m_edge03Max || max03 < m_edge03Min)
	{
		return false;
	}

	if (center01 < m_edge01Min && center03 < m_edge03Min)
	{
		return collideCornerCircle(m_corner[0], pE, pV);
	}
	else if (center01 > m_edge01Max && center03 < m_edge03Min)
	{
		return collideCornerCircle(m_corner[1], pE, pV);
	}
	else if (center01 > m_edge01Max && center03 > m_edge03Max)
	{
		return collideCornerCircle(m_corner[2], pE, pV);
	}
	else if (center01 < m_edge01Min && center03 > m_edge03Max)
	{
		return collideCornerCircle(m_corner[3], pE, pV);
	}
	else
	{
		*pV = *(pE->getCenter()) - *getCenter();
		return true;
	}
}

bool Entity::collideCornerCircle(VECTOR2 corner, Entity* pE, VECTOR2* pV)
{
	m_distSquared = corner - *(pE->getCenter());            
	m_distSquared.x = m_distSquared.x * m_distSquared.x;      
	m_distSquared.y = m_distSquared.y * m_distSquared.y;

	m_sumRadiiSquared = pE->getRadius() * pE->getScale(); 
	m_sumRadiiSquared *= m_sumRadiiSquared;                

	if (m_distSquared.x + m_distSquared.y <= m_sumRadiiSquared)
	{
		*pV = *(pE->getCenter()) - corner;
		return true;
	}
	else
	{
		return false;
	}
}

void Entity::computeRotatedBox()
{
	if (m_isRotatedBox)
	{
		return;
	}

	float projection;

	VECTOR2 rotatedX(cos(m_spriteData.angle), sin(m_spriteData.angle));
	VECTOR2 rotatedY(-sin(m_spriteData.angle), cos(m_spriteData.angle));

	const VECTOR2* center = getCenter();
	m_corner[0] = *center + rotatedX * (static_cast<float>(m_edge.left*getScale())) +
		rotatedY * (static_cast<float>(m_edge.top*getScale()));
	m_corner[1] = *center + rotatedX * (static_cast<float>(m_edge.right*getScale())) +
		rotatedY * (static_cast<float>(m_edge.top*getScale()));
	m_corner[2] = *center + rotatedX * (static_cast<float>(m_edge.right*getScale())) +
		rotatedY * (static_cast<float>(m_edge.bottom*getScale()));
	m_corner[3] = *center + rotatedX * (static_cast<float>(m_edge.left*getScale())) +
		rotatedY * (static_cast<float>(m_edge.bottom*getScale()));

	m_edge01 = VECTOR2(m_corner[1].x - m_corner[0].x, m_corner[1].y - m_corner[0].y);
	m_pGraphics->vector2Normalize(&m_edge01);
	m_edge03 = VECTOR2(m_corner[3].x - m_corner[0].x, m_corner[3].y - m_corner[0].y);
	m_pGraphics->vector2Normalize(&m_edge03);

	projection = m_pGraphics->vector2Dot(&m_edge01, &m_corner[0]);
	m_edge01Min = projection;
	m_edge01Max = projection;

	projection = m_pGraphics->vector2Dot(&m_edge01, &m_corner[1]);
	if (projection < m_edge01Min)
	{
		m_edge01Min = projection;
	}
	else if (projection > m_edge01Max)
	{
		m_edge01Max = projection;
	}

	projection = m_pGraphics->vector2Dot(&m_edge03, &m_corner[0]);
	m_edge03Min = projection;
	m_edge03Max = projection;
	projection = m_pGraphics->vector2Dot(&m_edge03, &m_corner[3]);
	if (projection < m_edge03Min)
	{
		m_edge03Min = projection;
	}
	else if (projection > m_edge03Max)
	{
		m_edge03Max = projection;
	}

	m_isRotatedBox = true;
}