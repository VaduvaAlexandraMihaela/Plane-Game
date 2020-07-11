#include "CLife.h"


CLife::CLife(const BackBuffer *pBackBuffer, double x)
{
	m_pSprite = new Sprite("data/poti.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	static int cnt = 0;
	if (cnt < 3)
		m_pSprite->mPosition.x = x + 10;
	else
		m_pSprite->mPosition.x = x + 480;
	m_pSprite->mPosition.y = m_pSprite->height() / 2;
	cnt++;
}


CLife::~CLife()
{
	delete m_pSprite;
}
