#ifndef __STAR_H__
#define __STAR_H__

#include "cocos2d.h"
#include "Entity.h"
#include "Box2D.h"
#include <list>

USING_NS_CC;

class Star: public Entity
{
public:
	Star(b2World* world, b2Body* body);
};

#endif
