
#ifndef _CBULLET_H_
#define _CBULLET_H_

#include "Main.h"
#include "Sprite.h"
#include "CMovingObject.h"

class CBullet : public CMovingObject
{
public:
	// Constructor and destructor
	CBullet(const BackBuffer *pBackBuffer, char *szImageFile);
	~CBullet();

	// Public functions

	bool Update(float dt, UINT height);

};



#endif