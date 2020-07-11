#ifndef __CLIFE_H__
#define __CLIFE_H__

#include "Sprite.h"

class CLife
{
	Sprite *m_pSprite;
public:
	CLife(const BackBuffer *pBackBuffer, double x);
	virtual ~CLife(void);

	void Draw() { m_pSprite->draw(); }
	Vec2& Position() { return m_pSprite->mPosition; }

	int Height() { return m_pSprite->height(); }
	int Width() { return m_pSprite->width(); }
};

#endif