#ifndef __DYNAMICLIGHTWORLD_H__
#define __DYNAMICLIGHTWORLD_H__

#include "cocos2d.h"
#include "Entity.h"
#include "Planet.h"
#include "Box2D.h"
#include <list>
#include "LightRay.h"

USING_NS_CC;

class DynamicLightWorld
{
public:
	DynamicLightWorld();
	Entity* addSource(Entity* entity);
	Planet* addPlanet(Planet* entity);
	std::vector<struct LightRay> getRays();
	b2RayCastInput makeInput(Vec2 p1, Vec2 p2, float fracUnit, float maxFrac);
	void cast360(Entity* source, std::vector<b2RayCastInput>& inputs);
	void castToPlanet(Entity* source, Planet* dest, std::vector<b2RayCastInput>&);
	std::vector<Entity*> getSources();
	std::vector<Planet*> getPlanets();
private:
	std::vector<Entity*> sources;
	std::vector<Planet*> planets;
};

#endif
