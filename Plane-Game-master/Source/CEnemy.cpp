#include "CEnemy.h"
#include "CGameApp.h"
#include <list>

extern CGameApp g_App;

CEnemy::CEnemy(const BackBuffer *pBackBuffer, int NrOfEnemy)
{
	if(NrOfEnemy==1)
		m_pSprite = new Sprite("data/enemyimgandmask.bmp", RGB(0xff, 0x00, 0xff));
	else
		m_pSprite = new Sprite("data/enemyimgandmask2.bmp", RGB(0xff, 0x00, 0xff));

	m_pSprite->setBackBuffer(pBackBuffer);
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

}

CEnemy::~CEnemy()
{
	delete m_pSprite;

}

void CEnemy::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

}

void CEnemy::Draw()
{
	if (fireCooldown > 1) {
		fireCooldown--;
	}
		m_pSprite->draw();
}

void CEnemy::Move(ULONG ulDirection)
{if (ulDirection & CEnemy::DIR_LEFT)
		m_pSprite->mVelocity.x -= 1.1;


	//stanga
	if (m_pSprite->mPosition.x < m_pSprite->width() - m_pSprite->mPosition.x) {

		//m_pSprite->mPosition.x = m_pSprite->width() - m_pSprite->mPosition.x;
		ulDirection |= CEnemy::DIR_FORWARD;
		m_pSprite->mVelocity.x = 0;
		if (ulDirection & CEnemy::DIR_FORWARD)
			m_pSprite->mVelocity.y -= 1.1;
	}
	//sus+stanga
	if ((m_pSprite->mPosition.y < m_pSprite->height() - m_pSprite->mPosition.y ) && (m_pSprite->mPosition.x < m_pSprite->width() - m_pSprite->mPosition.x)) {
		m_pSprite->mPosition.y = m_pSprite->height() - m_pSprite->mPosition.y;
		//m_pSprite->mPosition.y = m_pSprite->height() - m_pSprite->mPosition.y;
		ulDirection |= CEnemy::DIR_RIGHT;
		m_pSprite->mVelocity.y = 0;
		if (ulDirection & CEnemy::DIR_RIGHT)
			m_pSprite->mVelocity.x += 400.2;
		
		
	}
	//dreapta
	if (m_pSprite->mPosition.x > GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() / 2 - 20) {
		///m_pSprite->mPosition.x = GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() / 2 - 20;

		m_pSprite->mVelocity.x = 0;
	}
	//sus
	

	
	//jos
	if (m_pSprite->mPosition.y > GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height()) {
		//m_pSprite->mPosition.y = GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height();
		m_pSprite->mVelocity.y = 0;
	}

	
	

	

	/*if (ulDirection & CEnemy::DIR_BACKWARD)
		m_pSprite->mVelocity.y += 1.1;

	*/
}

void CEnemy::Move2(ULONG ulDirection)
{
	if (ulDirection & CEnemy::DIR_LEFT)
		m_pSprite->mVelocity.x -= 1.1;


	//stanga
	if (m_pSprite->mPosition.x < m_pSprite->width() - m_pSprite->mPosition.x) {

		//m_pSprite->mPosition.x = m_pSprite->width() - m_pSprite->mPosition.x;
		ulDirection |= CEnemy::DIR_BACKWARD;
		m_pSprite->mVelocity.x = 0;
		if (ulDirection & CEnemy::DIR_BACKWARD)
			m_pSprite->mVelocity.y += 1.1;
	}
	//jos+stanga
	if ((m_pSprite->mPosition.y > GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height()) && (m_pSprite->mPosition.x < m_pSprite->width() - m_pSprite->mPosition.x)) {
		m_pSprite->mPosition.y = GetSystemMetrics(SM_CYSCREEN) - m_pSprite->height();
		m_pSprite->mVelocity.y = 0;
		ulDirection |= CEnemy::DIR_RIGHT;
		m_pSprite->mVelocity.y = 0;
		if (ulDirection & CEnemy::DIR_RIGHT)
			m_pSprite->mVelocity.x += 400.2;

	}
	//dreapta
	if (m_pSprite->mPosition.x > GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() / 2 - 20) {
		///m_pSprite->mPosition.x = GetSystemMetrics(SM_CXSCREEN) - m_pSprite->width() / 2 - 20;

		m_pSprite->mVelocity.x = 0;
	}
	//sus


}

void CEnemy::fireEnemy(int x, int y) {
	for (Sprite* it : bullets) {
		it->draw();
		it->mPosition.y += y;
		it->mVelocity.y = x;
		it->mPosition.x -= x;
		it->mVelocity.x = y;
	}
}


void CEnemy::enemyShoot(int x)
{
	if (fireCooldown < 70) {

		bullet = new Sprite("data/BulletImgAndMask2.bmp", RGB(0xff, 0x00, 0xff));
		bullet->setBackBuffer(g_App.m_pBBuffer);

		if (x == 1) {
			bullet->mPosition.y = m_pSprite->mPosition.y - m_pSprite->height() / 2;
			bullet->mPosition.x = m_pSprite->mPosition.x - 4;
		}

		if (x == -1)
		{
			bullet->mPosition.y = m_pSprite->mPosition.y + m_pSprite->height() / 2;
			bullet->mPosition.x = m_pSprite->mPosition.x + 4;
		}


		bullets.push_back(bullet);
		fireCooldown = 100;


	}

}

Vec2& CEnemy::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CEnemy::Velocity()
{
	return m_pSprite->mVelocity;
}

