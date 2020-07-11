
#ifndef _CSCORE_H_
#define _CSCORE_H_

#include "Sprite.h"

#include <deque>
#include <string>

class CScore
{
	int val;
	int transl; 
	std::deque<Sprite *> digits;
	const BackBuffer *pBackBuffer;

public:
	CScore(const BackBuffer *Buffer, int tr);
	virtual ~CScore();
	void setScore(int i) { val = i; }

	void Draw();
	void initScore();


};

#endif
