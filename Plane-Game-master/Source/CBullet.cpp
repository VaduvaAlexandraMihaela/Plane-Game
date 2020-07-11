#include "CBullet.h"


CBullet::CBullet(const BackBuffer *pBackBuffer, char *szImageFile)
{
	m_pSprite = new Sprite(szImageFile, RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	//PlaySound("data/gunshot.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_pSprite->mVelocity = Vec2(0, 200);
}


CBullet::~CBullet()
{
	delete m_pSprite;
}

bool CBullet::Update(float dt, UINT height)
{
	m_pSprite->update(dt);
	if (Position().y >= 0 && Position().y < height)
	{
		return true;
	}
	return false;
}
