
#include "CScore.h"


CScore::CScore(const BackBuffer *Buffer, int tr)
{
	val = 0;
	pBackBuffer = Buffer;
	transl = tr;
}


CScore::~CScore()
{
	while (!digits.empty())
	{
		delete digits.front();
		digits.erase(digits.begin());
	}
}

void CScore::Draw()
{

	initScore();
	int i = 0;
	for (Sprite *sp : digits)
	{
			//if (i == 0)
				sp->mPosition = Vec2(350 + i * 23 + transl, 23);
			//else
				//sp->mPosition = Vec2(350 + i * 23 + transl + 400, 23);
			sp->draw();
		i++;
	}

}

void CScore::initScore()
{
	for (Sprite *sp : digits)
	{
		delete sp;
	}
	digits.clear();

	if (val == 0)
	{
		Sprite *s = new Sprite("data/0.bmp", RGB(0xff, 0x00, 0xff));
		s->setBackBuffer(pBackBuffer);
		s->mPosition = Vec2(530, 50);

		digits.push_front(s);
	}
	else
	{
		
		int copy = val;
		unsigned int i = 0;

		while (copy)
		{
			std::string str = "data/";
			str += std::to_string(copy % 10);
			str += ".bmp";
			copy /= 10;

			const char *result = str.c_str();
			Sprite *s = new Sprite(result, RGB(0xff, 0x00, 0xff));
			s->setBackBuffer(pBackBuffer);

			digits.push_front(s);
			i++;
			
		}
	}
}
