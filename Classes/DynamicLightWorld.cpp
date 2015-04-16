#include <algorithm>    // std::sort
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "DynamicLightWorld.h"
#include "Box2D.h"
#include "Planet.h"
#include "LightRay.h"

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

b2RayCastInput DynamicLightWorld::makeInput(Vec2 p1, Vec2 p2, float fracUnit, float maxFrac)
{
	Vec2 ray = p2 - p1;
	ray.normalize();
	ray.scale(fracUnit);
	p2 = p1 + ray;

	b2RayCastInput input;
	input.p1 = b2Vec2(p1.x, p1.y);
	input.p2 = b2Vec2(p2.x, p2.y);
	input.maxFraction = maxFrac;

	return input;
}

void DynamicLightWorld::castToPlanet(Entity* source, Planet* dest, std::vector<b2RayCastInput>& inputs)
{
	float radius = dest->getRadius();
	Vec2 ray = dest->getPos() - source->getPos();
	Vec2 perp = ray.getPerp();
	perp.normalize();
	Vec2 perpOffset = perp;
	perp.scale(radius);

	Vec2 rPerp = ray.getRPerp();
	rPerp.normalize();
	Vec2 rPerpOffset = rPerp;
	rPerp.scale(radius);

	Vec2 p2L = dest->getPos() + perp + perpOffset;
	Vec2 p2 = dest->getPos() + perp;
	Vec2 p2R = dest->getPos() + perp - perpOffset;

	Vec2 rp2L = dest->getPos() + rPerp + rPerpOffset;
	Vec2 rp2 = dest->getPos() + rPerp;
	Vec2 rp2R = dest->getPos() + rPerp - rPerpOffset;

	float unitFrac = 50;
	float maxFrac = 50;
	inputs.push_back(makeInput(source->getPos(), p2L, unitFrac, maxFrac));
	inputs.push_back(makeInput(source->getPos(), p2, unitFrac, maxFrac));
	inputs.push_back(makeInput(source->getPos(), p2R, unitFrac, maxFrac));
	inputs.push_back(makeInput(source->getPos(), rp2L, unitFrac, maxFrac));
	inputs.push_back(makeInput(source->getPos(), rp2, unitFrac, maxFrac));
	inputs.push_back(makeInput(source->getPos(), rp2R, unitFrac, maxFrac));
}

bool sortByRadian (struct LightRay i, struct LightRay j)
{
	Vec2 a = i.p2-i.p1;
	Vec2 b = j.p2-j.p1;
	float angleA = a.getAngle();
	float angleB = b.getAngle();
	if(i.p2.y >= i.p1.y && i.p2.x >= i.p1.x && j.p2.y >= j.p1.y && j.p2.x >= j.p1.x)
		return (angleA<angleB);
	else if(i.p2.y >= i.p1.y && i.p2.x >= i.p1.x && (j.p2.y <= j.p1.y || j.p2.x < j.p1.x))
		return true;
	else if(i.p2.y >= i.p1.y && i.p2.x < i.p1.x && j.p2.y >= j.p1.y && j.p2.x < j.p1.x)
		return (angleA<angleB);
	else if(i.p2.y >= i.p1.y && i.p2.x < i.p1.x && j.p2.y < j.p1.y)
		return true;
	else if(i.p2.y < i.p1.y && i.p2.x <= i.p1.x && j.p2.y < j.p1.y && j.p2.x <= j.p1.x)
		return (angleA<angleB);
	else if(i.p2.y < i.p1.y && i.p2.x <= i.p1.x && j.p2.y < j.p1.y && j.p2.x > j.p1.x)
		return true;
	else if(i.p2.y < i.p1.y && i.p2.x > i.p1.x && j.p2.y < j.p1.y && j.p2.x > j.p1.x)
		return (angleA<angleB);
	return false;
}

void DynamicLightWorld::cast360(Entity* source, std::vector<b2RayCastInput>& inputs)
{
	float unitFrac = 50;
	float maxFrac = 50;
	for(auto radian = 0.; radian < 2. * M_PI; radian += (2. * M_PI / 200))
	{
		Vec2 p2 = Vec2::forAngle(radian) + source->getPos();
		inputs.push_back(makeInput(source->getPos(), p2, unitFrac, maxFrac));
	}
}

std::vector<struct LightRay> DynamicLightWorld::getRays()
{
	std::vector<b2RayCastInput> inputs;
	std::vector<LightRay> rays;

	//Get input rays
	auto numOfSources = sources.size();
	auto numOfPlanets = planets.size();
	for(auto i = 0; i < numOfSources; i++)
	{
		cast360(sources[i], inputs);
		for(auto j = 0; j < numOfPlanets; j++)
		{
			castToPlanet(sources[i], planets[j], inputs);
		}
	}

	//Update rays
	auto numOfInputs = inputs.size();
	for(auto i = 0; i < numOfInputs; i++)
	{
		LightRay ray;
		ray.p1 = Vec2(inputs[i].p1.x, inputs[i].p1.y);
		ray.p2 = Vec2(inputs[i].p2.x, inputs[i].p2.y);
		ray.maxFrac = inputs[i].maxFraction;
		for(auto j = 0; j < numOfPlanets; j++)
		{
			b2RayCastOutput output;
			if (planets[j]->castRay(&output, inputs[i]))
			{
				ray.frac = output.fraction;
				ray.normal = Vec2(output.normal.x, output.normal.y);
				break;
			}
			else if (j == numOfPlanets - 1)
			{
				ray.frac = inputs[i].maxFraction;
				ray.normal = Vec2::ZERO;
			}
		}
		rays.push_back(ray);
	}
	std::sort (rays.begin(), rays.end(), sortByRadian);
	return rays;
}
