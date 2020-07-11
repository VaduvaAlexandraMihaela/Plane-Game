//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer, int depl):mBackBuffer(pBackBuffer), mFacingDirection(DIRECTION::DIR_FORWARD)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff,0x00, 0xff));
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;

	//creating lives
	for (int i = 0; i < 3; i++)
	{
		lives.push_front(new CLife(pBackBuffer, i * 50 + 20 + depl));
	}

	score = new CScore(pBackBuffer, depl);
	scorey = 0;

	alive = true;
	//schimbare viteza nave
	movementCoef = 3;
	isInvincible = false;

}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;

	while (!bullets.empty())
	{
		delete bullets.front();
		bullets.erase(bullets.begin());

	}

	while (!lives.empty())
	{
		delete lives.front();
		lives.erase(lives.begin());
	}
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	//m_pExplosionSprite->SetFrame(0);
	//PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
	if (!isInvincible)
		deleteLife();
	Velocity() = Vec2(0, 0);
}

void CPlayer::deleteLife()
{
	if (lives.size())
	{
		delete lives[0];
		lives.erase(lives.begin());
	}
	else
	{
		alive = false;
	}
}

void CPlayer::Update(float dt, ULONG width, ULONG height)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		if (!bullets[i]->Update(dt, height))
		{
			std::deque<CBullet *>::iterator it = bullets.begin();
			advance(it, i);
			delete (*it);

			bullets.erase(bullets.begin() + i);
			i--;
		}
	}

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Shoot(const BackBuffer *pBackBuffer, int i)
{
	ULONG Direction;
	if (!m_bExplosion)
	{
		bullets.push_front(new CBullet(pBackBuffer, "data/BulletImgAndMask.bmp"));
		bullets.front()->Position() = Vec2(Position().x, Position().y);
		/*if(Direction==FORWARD)
			bullets.front()->Velocity() = Vec2(0, -500);
		if (Direction == BACKWARD)
			bullets.front()->Velocity() = Vec2(0, 500);
		if (Direction == LEFT)
			bullets.front()->Velocity() = Vec2(500, 0);
		if (Direction == RIGHT)*/
		if(i==0)
			bullets.front()->Velocity() = Vec2(0, -500);
		else
			if(i==1)
				bullets.front()->Velocity() = Vec2(-500, 0);
			else
				if(i==2)
					bullets.front()->Velocity() = Vec2(0, 500);
				else
					if(i==3)
					bullets.front()->Velocity() = Vec2(500, 0);


	}
}

void CPlayer::Draw()
{
	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();

		if (alive)
		{
			for (unsigned int i = 0; i < bullets.size(); i++)
				bullets[i]->Draw();

			for (unsigned int i = 0; i < lives.size(); i++)
				lives[i]->Draw();

			score->Draw();
		}

}

void CPlayer::Reset()
{
	movementCoef = 1;
	isInvincible = false;
}

void CPlayer::newLife(const BackBuffer *pBackBuffer)
{
	setLives(pBackBuffer, 3);
}

void CPlayer::Invincible()
{
	isInvincible = true;
}

void CPlayer::MoveFaster()
{
	movementCoef = 100;
}

void CPlayer::Move(ULONG ulDirection)
{
	//stanga
	if (m_pSprite->mPosition.x < m_pSprite->width() - m_pSprite->mPosition.x) {

		m_pSprite->mPosition.x = m_pSprite->width() - m_pSprite->mPosition.x;
		m_pSprite->mVelocity.x = 0;

	}
	 //dreapta
	if (m_pSprite->mPosition.x > GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() /2 - 20) {
		m_pSprite->mPosition.x = GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() /2 - 20;
		m_pSprite->mVelocity.x = 0;
	}
	//sus
	if (m_pSprite->mPosition.y < m_pSprite->height() - m_pSprite->mPosition.y) {
		m_pSprite->mPosition.y = m_pSprite->height() - m_pSprite->mPosition.y;
		m_pSprite->mVelocity.y = 0;
	}
	//jos
	if (m_pSprite->mPosition.y > GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height() ) {
		m_pSprite->mPosition.y = GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height() ;
		m_pSprite->mVelocity.y = 0;
	}

	
	if( ulDirection & CPlayer::DIR_LEFT )
		m_pSprite->mVelocity.x -= 7.1;

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x += 7.1;

	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y -= 7.1;

	if( ulDirection & CPlayer::DIR_BACKWARD )
		m_pSprite->mVelocity.y += 7.1;
}

Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;

}


Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}


void CPlayer::RotateLeft()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/planeimgandmaskleft.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_LEFT;
		owner = "left";
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/planeimgandmaskright.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_RIGHT;
		owner = "right";
		break;
	case DIRECTION::DIR_LEFT:
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		owner = "down";
		m_pSprite = new Sprite("data/planeimgandmaskdown.bmp", RGB(0xff, 0x00, 0xff));
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		owner = "forward";
		mFacingDirection = DIRECTION::DIR_FORWARD;
		break;
	}
	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}

void CPlayer::RotateRight()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/planeimgandmaskright.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_RIGHT;
		owner = "right";
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/planeimgandmaskleft.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_LEFT;
		owner = "left";
		break;
	case DIRECTION::DIR_LEFT:
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_FORWARD;
		owner = "forward";
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/planeimgandmaskdown.bmp", RGB(0xff, 0x00, 0xff));
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		owner = "down";
		break;
	}

	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}


void CPlayer::deleteBullet(int i)
{
	delete bullets[i];
	bullets.erase(bullets.begin() + i);
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			//m_pSprite->mVelocity = Vec2(0,0);
			//m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}


void CPlayer::setLives(const BackBuffer *pBackBuffer, int i)
{

	if (i > lives.size())
	{
		for (int liv = lives.size(); liv < i; liv++)
		{
			lives.push_front(new CLife(pBackBuffer, liv * 40 + 20));
		}
	}
	else if (i < lives.size())
	{
		for (int liv = i; liv < lives.size(); liv++)
		{
			deleteLife();
		}
	}

}

int CPlayer::GetLives()
{
	return mLives;
}
