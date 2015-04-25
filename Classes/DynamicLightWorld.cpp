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

Star* DynamicLightWorld::addStar(Star* entity)
{
	stars.push_back(entity);
	return entity;
}

Planet* DynamicLightWorld::addPlanet(Planet* entity)
{
	planets.push_back(entity);
	return entity;
}

std::vector<Star*> DynamicLightWorld::getStars()
{
	return stars;
}

b2RayCastInput DynamicLightWorld::makeInput(Vec2 p1, Vec2 p2)
{
	Vec2 ray = p2 - p1;
	ray.normalize();
	ray.scale(DynamicLightWorld::unit_frac);
	p2 = p1 + ray;

	b2RayCastInput input;
	input.p1 = b2Vec2(p1.x, p1.y);
	input.p2 = b2Vec2(p2.x, p2.y);
	input.maxFraction = DynamicLightWorld::max_frac;

	return input;
}

void DynamicLightWorld::castToPlanet(Entity* source, Planet* dest, std::vector<b2RayCastInput>& inputs)
{
	float radius = dest->getRadius();
	Vec2 ray = dest->getPos() - source->getPos();
	Vec2 perp = ray.getPerp();
	perp.normalize();
	Vec2 perpOffset = perp*2;
	perp.scale(radius);

	Vec2 rPerp = ray.getRPerp();
	rPerp.normalize();
	Vec2 rPerpOffset = rPerp*2;
	rPerp.scale(radius);

	Vec2 p2L = dest->getPos() + perp + perpOffset;
	Vec2 p2 = dest->getPos() + perp;
	Vec2 p2R = dest->getPos() + perp - perpOffset;

	Vec2 rp2L = dest->getPos() + rPerp + rPerpOffset;
	Vec2 rp2 = dest->getPos() + rPerp;
	Vec2 rp2R = dest->getPos() + rPerp - rPerpOffset;

	inputs.push_back(makeInput(source->getPos(), p2L));
	inputs.push_back(makeInput(source->getPos(), p2));
	inputs.push_back(makeInput(source->getPos(), p2R));
	inputs.push_back(makeInput(source->getPos(), rp2L));
	inputs.push_back(makeInput(source->getPos(), rp2));
	inputs.push_back(makeInput(source->getPos(), rp2R));
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
	for(auto radian = 0.; radian < 2. * M_PI; radian += (2. * M_PI / 10))
	{
		Vec2 p2 = Vec2::forAngle(radian) + source->getPos();
		inputs.push_back(makeInput(source->getPos(), p2));
	}
}

std::vector<struct LightRay> DynamicLightWorld::getRaysforSource(Entity* source)
{
	std::vector<b2RayCastInput> inputPlanet;
	std::vector<b2RayCastInput> input360;
	std::vector<LightRay> rays;

	auto numOfPlanets = planets.size();
	cast360(source, input360);
	for(auto i = 0; i < numOfPlanets; i++)
	{
		castToPlanet(source, planets[i], inputPlanet);
	}

	//Update rays for Planets
	auto numOfInputPlanet = inputPlanet.size();
	for(auto i = 0; i < numOfInputPlanet; i++)
	{
		LightRay ray;
		ray.p1 = Vec2(inputPlanet[i].p1.x, inputPlanet[i].p1.y);
		ray.p2 = Vec2(inputPlanet[i].p2.x, inputPlanet[i].p2.y);
		ray.frac = inputPlanet[i].maxFraction;
		ray.normal = Vec2::ZERO;
		for(auto j = 0; j < numOfPlanets; j++)
		{
			b2RayCastOutput output;
			if (planets[j]->castRay(&output, inputPlanet[i]))
			{
				if(output.fraction < ray.frac)
				{
					ray.frac = output.fraction;
					ray.normal = Vec2(output.normal.x, output.normal.y);
				}
			}
		}
		rays.push_back(ray);
	}

	//Update rays for 360
	auto numOfInput360 = input360.size();
	for(auto i = 0; i < numOfInput360; i++)
	{
		for(auto j = 0; j < numOfPlanets; j++)
		{
			b2RayCastOutput output;
			if (planets[j]->castRay(&output, input360[i]))
			{
				break;
			}
			else if (j == numOfPlanets - 1)
			{
				LightRay ray;
				ray.p1 = Vec2(input360[i].p1.x, input360[i].p1.y);
				ray.p2 = Vec2(input360[i].p2.x, input360[i].p2.y);
				ray.frac = input360[i].maxFraction;
				ray.normal = Vec2::ZERO;
				rays.push_back(ray);
			}
		}
	}
	std::sort (rays.begin(), rays.end(), sortByRadian);
	return rays;
}

void DynamicLightWorld::drawLight(DrawNode* drawNode, struct LightRay ray1, struct LightRay ray2, Star* star)
{
	Color3B color = star->getSprite()->getColor();

	auto length1 = ray1.frac < 1 ? ray1.frac : 1;
	auto length2 = ray2.frac < 1 ? ray2.frac : 1;

	Vec2 source = ray1.p1;
	Vec2 p1 = ray1.p1 + (ray1.p2 - ray1.p1) * length1;
	Vec2 p2 = ray2.p1 + (ray2.p2 - ray2.p1) * length2;
	float fracUnit1 = ray1.p2.getDistance(ray1.p1);
	float fracUnit2 = ray2.p2.getDistance(ray2.p1);

	Vec2 verts[3] = {
			source, p1, p2
	};
	drawNode->drawPolygon(verts, 3, Color4F(color.r, color.g, color.b, 1), 0, Color4F(0, 0, 0, 0));

	Vec2 p3;
	Vec2 p4;
	auto lengthFactor = 0;
	auto strengthFactor = 0;
	while(length1 <= ray1.frac || length2 <= ray2.frac)
	{
		auto length  = pow(1.05, lengthFactor++);
		length1 = length1 + length < ray1.frac ? length1 + length : ray1.frac;
		length2 = length2 + length < ray2.frac ? length2 + length : ray2.frac;

		p3 = ray1.p1 + (ray1.p2 - ray1.p1) * length1;
		p4 = ray2.p1 + (ray2.p2 - ray2.p1) * length2;
		auto strength = pow(0.94, strengthFactor++);
		if(strength < 0.001) {
			break;
		}
		Vec2 verts[4] = {
			p1, p3, p4, p2
		};
		drawNode->drawPolygon(verts, 4, Color4F(color.r, color.g, color.b, strength), 0, Color4F(0,0,0,0));
		p1 = Vec2(p3);
		p2 = Vec2(p4);

		if(length1 == ray1.frac || length2 == ray2.frac)
			break;
	}
}

void DynamicLightWorld::updateLight(DrawNode* drawNode) {
	drawNode->clear();
	std::vector<Star*> stars = getStars();
	auto numOfStars = stars.size();
	for(auto i = 0; i < numOfStars; i++)
	{
		std::vector<struct LightRay> rays = getRaysforSource(stars[i]);
		auto numOfRays = rays.size();
		//drawNode->drawLine(rays[0].p1, rays[0].p2 + (rays[0].p2 - rays[0].p1) * rays[0].frac, Color4F::GREEN);
		for(auto j = 1; j < numOfRays; j++)
		{
			drawLight(drawNode, rays[j-1], rays[j], stars[i]);
			//drawNode->drawLine(rays[i].p1, rays[i].p2 + (rays[i].p2 - rays[i].p1) * rays[i].frac, Color4F::GREEN);
		}
		drawLight(drawNode, rays[0], rays[numOfRays - 1], stars[i]);
	}
}
