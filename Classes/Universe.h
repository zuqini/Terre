#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "DynamicLightWorld.h"
#include "Entity.h"
#include <list>

USING_NS_CC;

class Universe : public DynamicLightWorld
{
public:
	Universe();
	void generateEntities(Vec2 origin, Size visibleSize);
	Star* createStarAt(Vec2 location);
	Planet* createPlanetAt(Vec2 location);
	std::vector<Entity*> getEntities();
	void step(float delta);
	void updatePos();
private:
	Entity* addEntity(Entity* entity);
	void generateSystem(Vec2 origin, Size visibleSize, int starOpt);
	void applyGravity();

	std::vector<Entity*> entities;
	b2World* world;
};

#endif
