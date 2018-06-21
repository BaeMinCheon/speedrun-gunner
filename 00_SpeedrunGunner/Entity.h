#pragma once

#ifndef _ENTITY_H
#define _ENTITY_H
#define WIN32_LEAN_AND_MEAN

#include "Image.h"
#include "Input.h"
#include "Game.h"

namespace NS_Entity
{
	enum COLLISION_TYPE
	{
		NONE,
		CIRCLE,
		BOX,
		ROTATED_BOX
	};

	const float GRAVITY = 6.67428e-11f;
}

class Entity : public Image
{
public:
	Entity();
	virtual ~Entity();

	virtual bool initialize(Game*, int, int, int, TextureManager*);
	virtual void activate();
	virtual void update(float);
	virtual void processAI(float, Entity*);
	virtual bool isOutsideRect(RECT*);
	virtual bool collidesWith(Entity*, VECTOR2*);
	virtual void damage(int);

	void bounce(VECTOR2*, Entity*);
	void gravityForce(Entity*, float);

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	virtual const VECTOR2* getCenter()
															{
																m_center = VECTOR2(getCenterX(), getCenterY());
																return &m_center;
															}
	virtual float getRadius() const							{ return m_radius; }
	virtual const RECT& getEdge() const						{ return m_edge; }
	virtual const VECTOR2* getCorner(UINT c) const
															{
																if (c >= 4)
																	c = 0;
																return &m_corner[c];
															}
	virtual const VECTOR2 getVelocity() const				{ return m_velocity; }
	virtual bool  getActive()         const					{ return m_isActive; }
	virtual float getMass()           const					{ return m_mass; }
	virtual float getGravity()        const					{ return m_gravity; }
	virtual float getHealth()         const					{ return m_health; }
	virtual NS_Entity::COLLISION_TYPE getCollisionType()	{ return m_collisionType; }
	virtual float getBounciness() const						{ return m_bounciness; }

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== =====

	virtual void  setVelocity(VECTOR2 v)					{ m_velocity = v; }
	virtual void  setDeltaV(VECTOR2 dv)						{ m_deltaV = dv; }
	virtual void  setActive(bool a)							{ m_isActive = a; }
	virtual void  setHealth(float h)						{ m_health = h; }
	virtual void  setMass(float m)							{ m_mass = m; }
	virtual void  setGravity(float g)						{ m_gravity = g; }
	virtual void  setCollisionRadius(float r)				{ m_radius = r; }
	virtual void  setCollisionType(NS_Entity::COLLISION_TYPE t)
															{ m_collisionType = t; }
	virtual void  setBounciness(float b)					{ m_bounciness = b; }
	virtual void  setEdge(RECT r)							{ m_edge = r; }

protected:
	virtual bool collideCircle(Entity*, VECTOR2*);
	virtual bool collideBox(Entity*, VECTOR2*);
	virtual bool collideRotatedBox(Entity*, VECTOR2*);
	virtual bool collideRotatedBoxCircle(Entity*, VECTOR2*);
	void computeRotatedBox();
	bool projectionsOverlap(Entity*);
	bool collideCornerCircle(VECTOR2, Entity*, VECTOR2*);

protected:
	Input*						m_pInput;
	NS_Entity::COLLISION_TYPE	m_collisionType;
	VECTOR2						m_center;
	VECTOR2						m_distSquared;
	float						m_radius;
	float						m_sumRadiiSquared;
	RECT						m_edge;
	VECTOR2						m_corner[4];
	VECTOR2						m_edge01;
	VECTOR2						m_edge03;
	float						m_edge01Min;
	float						m_edge01Max;
	float						m_edge03Min;
	float						m_edge03Max;
	VECTOR2						m_velocity;
	VECTOR2						m_deltaV;
	float						m_mass;
	float						m_health;
	float						m_RR;
	float						m_force;
	float						m_gravity;
	float						m_bounciness;
	HRESULT						m_HR;
	bool						m_isActive;
	bool						m_isRotatedBox;
};

#endif