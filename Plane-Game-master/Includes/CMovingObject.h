#ifndef _CMOVINGOBJECT_H_
#define _CMOVINGOBJECT_H_

#include "Sprite.h"

class CMovingObject
{
public:


protected:
	Sprite *m_pSprite;
	float m_fTimer;
	

	// constructor and destructor
	CMovingObject();
	virtual ~CMovingObject();

public:

	Vec2&					Position() { return m_pSprite->mPosition; }
	Vec2&					Velocity() { return m_pSprite->mVelocity; }

	int Height() { return m_pSprite->height(); }
	int Width() { return m_pSprite->width(); }

	void Update(float dt);
	void Draw();
	
};

#endif