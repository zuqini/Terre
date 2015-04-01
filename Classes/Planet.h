#ifndef __PLANET_H__
#define __PLANET_H__

#include "cocos2d.h"
#include "Entity.h"
#include "Box2D.h"
#include <list>

USING_NS_CC;

class Planet: public Entity
{
public:
	Planet(b2World* world, b2Body* body);
};

#endif
