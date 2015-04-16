#ifndef __RAY_H__
#define __RAY_H__

#include "cocos2d.h"
#include <list>

struct LightRay
{
	Vec2 p1;
	Vec2 p2;
	Vec2 normal;
	float frac;
	float maxFrac;
};

#endif
