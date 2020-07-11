#include "CMovingObject.h"


CMovingObject::CMovingObject()
{
}


CMovingObject::~CMovingObject()
{
	//delete m_pSprite;
}

void CMovingObject::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


}

void CMovingObject::Draw()
{
	m_pSprite->draw();
}
