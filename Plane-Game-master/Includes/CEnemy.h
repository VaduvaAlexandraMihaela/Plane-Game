
#ifndef _CENEMY_H_
#define _CENEMY_H_


#include "Main.h"
#include "Sprite.h"
#include <list>

class CEnemy
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION
	{
		DIR_FORWARD = 1,
		DIR_BACKWARD = 2,
		DIR_LEFT = 4,
		DIR_RIGHT = 8,

	};


	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CEnemy(const BackBuffer *pBackBuffer, int x);
	virtual ~CEnemy();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt);
	int Height() { return m_pSprite->height(); }
	int Width() { return m_pSprite->width(); }
	void					Draw();
	void					Move(ULONG ulDirection);
	void					Move2(ULONG ulDirection);
	Vec2&					Position();
	Vec2&					Velocity();
	int                     fireCooldown = 20;
	void					fireEnemy(int x, int y);
	void					enemyShoot(int x);
	std::list<Sprite*>	    bullets;
	Sprite*					bullet;

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pSprite;
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;

};

#endif