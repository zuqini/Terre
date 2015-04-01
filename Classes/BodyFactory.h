#ifndef __BODYFACTORY_H__
#define __BODYFACTORY_H__

#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;

//creates and defines b2bodies
namespace BodyFactory
{
    b2Body* createCircularBody(b2World* world, float density, Vec2 pos, Sprite* sprite);
}

#endif
