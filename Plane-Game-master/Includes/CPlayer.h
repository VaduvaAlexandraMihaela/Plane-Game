//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "CBullet.h"
#include "CLife.h"
#include "CScore.h"
#include <deque>

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_FORWARD	 = 1, 
		DIR_BACKWARD	= 2, 
		DIR_LEFT		= 4, 
		DIR_RIGHT	   = 8, 
	
	};

	
	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CPlayer(const BackBuffer *pBackBuffer, int depl);
	virtual ~CPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt, ULONG width, ULONG height);
	void					Draw();
	void					Move(ULONG ulDirection);
	void					Shoot(const BackBuffer *pBackBuffer,int i);

	Vec2&					Position();
	Vec2&					Velocity();
	Vec2&					Rotate();
	void					Explode();
	unsigned int			NoOfBullets() { return bullets.size(); }
	CBullet*				getBullet(int i) { return bullets[i]; }
	void					deleteBullet(int i);
	bool					AdvanceExplosion();


	void					RotateLeft();
	void					RotateRight();
	std::string owner;
	DIRECTION mFacingDirection;
	const BackBuffer *mBackBuffer;

	int Height() { return m_pSprite->height(); }
	int Width() { return m_pSprite->width(); }
	void incrementScore(int i) { scorey += i; score->setScore(scorey); }
	void setScore(int i) { scorey = i; score->setScore(scorey); }
	int  getScore() { return scorey; }
	int GetLives();
	void MoveFaster();
	void Invincible();
	void newLife(const BackBuffer *pBackBuffer);
	void Reset();
	//void SetLives(int newLives);
	int mLives;
	void deleteLife();
	bool isAlive() { return alive; }
	void revive() { alive = true; }
	unsigned int NoOfLives() { return lives.size(); }
	void setLives(const BackBuffer *pBackBuffer, int i);

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite*					m_pSprite;
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;
	std::deque<CBullet *>	bullets;
	std::deque<CLife *>	    lives;
	CScore *			    score;
	int						scorey;
	bool					alive;
	int						movementCoef;
	bool					isInvincible;
	bool					m_bExplosion;
	AnimatedSprite*			m_pExplosionSprite;
	int						m_iExplosionFrame;


};

#endif // _CPLAYER_H_