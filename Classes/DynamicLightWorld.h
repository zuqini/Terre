#ifndef __DYNAMICLIGHTWORLD_H__
#define __DYNAMICLIGHTWORLD_H__

#include "cocos2d.h"
#include "Entity.h"
#include "Star.h"
#include "Planet.h"
#include "Box2D.h"
#include <list>
#include "LightRay.h"

USING_NS_CC;

class DynamicLightWorld
{
public:
	DynamicLightWorld();
	std::vector<struct LightRay> getRaysforSource(Entity* Source);
	std::vector<Star*> getStars();
	void updateLight(DrawNode* drawNode);
protected:
	Star* addStar(Star* entity);
	Planet* addPlanet(Planet* entity);
private:
	b2RayCastInput makeInput(Vec2 p1, Vec2 p2);
	void cast360(Entity* source, std::vector<b2RayCastInput>& inputs);
	void castToPlanet(Entity* source, Planet* dest, std::vector<b2RayCastInput>&);
	void drawLight(DrawNode* drawNode, struct LightRay ray1, struct LightRay ray2, Star* star);

	std::vector<Star*> stars;
	std::vector<Planet*> planets;

	const int max_frac = 500;
	const int unit_frac = 50;
};

#endif
