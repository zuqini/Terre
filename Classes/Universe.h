#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "Entity.h"
#include <list>

USING_NS_CC;

class Universe
{
public:
	Universe();
	Entity* addEntity(Entity* entity);
	void generateSystem(Vec2 origin, Size visibleSize);
	void generateEntities(Vec2 origin, Size visibleSize);
	void render();
	void step(float delta);
	std::vector<Entity*> getEntities();
private:
	void applyGravity();
	std::vector<Entity*> entities;
	b2World* world;
};

#endif
