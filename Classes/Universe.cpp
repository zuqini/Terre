#include "Universe.h"
#include "Box2D.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

#define G_CONSTANT 0.0001

USING_NS_CC;

Universe::Universe()
{
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	world = new b2World(gravity);
}

Entity* Universe::addEntity(Entity* entity)
{
	entities.push_back(entity);
	return entity;
}

void Universe::step(float delta)
{
	applyGravity();
	world->Step(delta, 8, 3);
}

void Universe:: generateSystem(Vec2 origin, Size visibleSize)
{
	/**
	 * @TODO
	 * there's a better way to pseudorandom generate in C++11
	 */

	srand (time(0));
	Sprite* starSprite;
	switch(rand() % 2)
	{
	case 0:
		starSprite = Sprite::create("Star00.png");
		break;
	case 1:
		starSprite = Sprite::create("Star01.png");
		break;
	}
	Entity* star = addEntity(Entity::makeStar(world, 100000, Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y), starSprite));
	Vec2 currPos = star->getPos();
	currPos.add(Vec2(300,0));
	Sprite* planetSprite;
	int planetCount = rand() % 10 + 4;
	for(auto i = 0; i < planetCount; ++i)
	{
		currPos.add(Vec2(i*200,0));
		switch(rand() % 4)
		{
		case 0:
			planetSprite = Sprite::create("Planet00.png");
			break;
		case 1:
			planetSprite = Sprite::create("Planet01.png");
			break;
		case 2:
			planetSprite = Sprite::create("Planet02.png");
			break;
		case 3:
			planetSprite = Sprite::create("Planet03.png");
			break;
		}
		Entity* currPlanet = addEntity(Entity::makePlanet(world, rand() % 1000 + 500, currPos, planetSprite));
		currPlanet->applyImpulse(Vec2(0,currPlanet->getMass() * sqrt(G_CONSTANT * star->getMass() / star->getPos().getDistance(currPlanet->getPos()))));
	}
}

void Universe::generateEntities(Vec2 origin, Size visibleSize)
{
	generateSystem(origin, visibleSize);	//Apply more times at different locations to generate more systems
}

void Universe::applyGravity()
{

	auto length = entities.size();
	auto r2 = 0.;
	auto sForce = 0.;
	for (auto row = 0; row < length; ++row) {
		for (auto col = row + 1; col < length; ++col) {
			r2 = entities[row]->getPos().distanceSquared(entities[col]->getPos());	//holy cow Vec2 is op, they do everything for us :D
			sForce = G_CONSTANT * entities[row]->getMass() * entities[col]->getMass() / r2;
			Vec2 accRow(entities[row]->getPos(), entities[col]->getPos());
			accRow.normalize();
			accRow.scale(sForce);
			Vec2 accCol = accRow;
			accCol.negate();
			entities[row]->applyForce(accRow);
			entities[col]->applyForce(accCol);
		}
	}
}

void Universe::render()
{
	for(auto i = 0; i < entities.size(); ++i) {
		entities[i]->getUpdateSprite();
	}
}

std::vector<Entity*> Universe::getEntities()
{
	return entities;
}
