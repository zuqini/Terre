#include "DynamicLightWorld.h"
#include "Box2D.h"
#include "Planet.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "LightRay.h"

#define MAX_FRAC 5
#define FRAC_UNIT 30

USING_NS_CC;

DynamicLightWorld::DynamicLightWorld()
{
}

Entity* DynamicLightWorld::addSource(Entity* entity)
{
	sources.push_back(entity);
	return entity;
}

Planet* DynamicLightWorld::addPlanet(Planet* entity)
{
	planets.push_back(entity);
	return entity;
}

std::vector<Entity*> DynamicLightWorld::getSources()
{
	return sources;
}

std::vector<Planet*> DynamicLightWorld::getPlanets()
{
	return planets;
}

b2RayCastInput DynamicLightWorld::makeInput(Vec2 p1, Vec2 p2)
{
	Vec2 ray = p2 - p1;
	ray.normalize();
	ray.scale(FRAC_UNIT);
	p2 = p1 + ray;

	b2RayCastInput input;
	input.p1 = b2Vec2(p1.x, p1.y);
	input.p2 = b2Vec2(p2.x, p2.y);
	input.maxFraction = MAX_FRAC;

	return input;
}

void DynamicLightWorld::castToPlanet(Entity* source, Planet* dest, std::vector<b2RayCastInput>& inputs)
{
	float radius = dest->getRadius();
	Vec2 ray = dest->getPos() - source->getPos();
	Vec2 perp = ray.getPerp();
	perp.normalize();
	perp *= radius;

	Vec2 rPerp = ray.getRPerp();
	rPerp.normalize();
	rPerp *= radius;

	Vec2 p2 = dest->getPos() + perp;
	Vec2 rp2 = dest->getPos() + rPerp;

	inputs.push_back(makeInput(source->getPos(), p2));
	inputs.push_back(makeInput(source->getPos(), rp2));
}

std::vector<struct LightRay> DynamicLightWorld::getRays()
{
	std::vector<b2RayCastInput> inputs;
	std::vector<LightRay> rays;
	//Get all input rays
	auto numOfSources = sources.size();
	auto numOfPlanets = planets.size();
	for(auto i = 0; i < numOfSources; i++)
	{
		for(auto j = 0; j < numOfPlanets; j++)
		{
			castToPlanet(sources[i], planets[j], inputs);
		}
	}

	//Update rays
	auto numOfInputs = inputs.size();
	for(auto i = 0; i < numOfInputs; i++)
	{
		for(auto j = 0; j < numOfPlanets; j++)
		{
			b2RayCastOutput output;
			LightRay ray;
			ray.p1 = Vec2(inputs[i].p1.x, inputs[i].p1.y);
			ray.p2 = Vec2(inputs[i].p2.x, inputs[i].p2.y);
			if (planets[j]->castRay(&output, inputs[i]))
			{
				ray.frac = output.fraction;
				ray.normal = Vec2(output.normal.x, output.normal.y);
			}
			else
			{
				ray.frac = inputs[i].maxFraction;
				ray.normal = Vec2::ZERO;
			}
			rays.push_back(ray);
		}
	}
	return rays;
}
