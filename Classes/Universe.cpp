#include "Universe.h"
#include "Box2D.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

#define G_CONSTANT 0.001

USING_NS_CC;

Universe::Universe() : DynamicLightWorld()
{
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	world = new b2World(gravity);
}

Star* Universe::createStarAt(Vec2 loc)
{
	srand (time(0));
	Sprite* starSprite;
	auto starOpt = rand() % 3 + 0;
	switch(starOpt)
	{
	case 0:
		starSprite = Sprite::create("Star00.png");
		starSprite->setColor(Color3B(1,1,0));
		break;
	case 1:
		starSprite = Sprite::create("Star01.png");
		starSprite->setColor(Color3B(0,0,1));
		break;
	case 2:
		starSprite = Sprite::create("Star02.png");
		starSprite->setColor(Color3B(1,0,0));
		break;
	}
	Star* star = (Star*)addEntity(Entity::makeStar(world, 100000, loc, starSprite));
	addStar(star);
	return star;
}

Planet* Universe::createPlanetAt(Vec2 loc)
{
	srand (time(0));
	Sprite* planetSprite;
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
	Planet* currPlanet = (Planet*)addEntity(Entity::makePlanet(world, rand() % 1000 + 500, loc, planetSprite));
	addPlanet(currPlanet);
	return currPlanet;
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

void Universe::generateSystem(Vec2 origin, Size visibleSize, int starOpt)
{
	/*
	 * TODO
	 * need better way to generateSystems
	 * need to update for new gravity constant
	 */

	Star* star = createStarAt(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	Vec2 currPos = star->getPos();
	currPos.add(Vec2(800,0));
	Sprite* planetSprite;
	int planetCount = rand() % 2 + 4;
	for(auto i = 0; i < planetCount; ++i)
	{
		currPos.add(Vec2((i+1)*500,0));
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
		Planet* currPlanet = (Planet*)addEntity(Entity::makePlanet(world, rand() % 1000 + 500, currPos, planetSprite));
		addPlanet(currPlanet);
		currPlanet->applyImpulse(Vec2(0,currPlanet->getMass() * sqrt(G_CONSTANT * star->getMass() / star->getPos().getDistance(currPlanet->getPos()))));
	}
}

void Universe::generateEntities(Vec2 origin, Size visibleSize)
{
	//createStarAt(origin);
	generateSystem(origin, visibleSize, 2);	//Apply more times at different locations to generate more systems
	//generateSystem(origin + Vec2(10000,10000), visibleSize, 0);	//Apply more times at different locations to generate more systems
	//generateSystem(origin + Vec2(20000,0), visibleSize, 1);	//Apply more times at different locations to generate more systems
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

void Universe::updatePos()
{
	int length = entities.size();
	for(auto i = 0; i < length; ++i) {
		entities[i]->updateSprite();
	}
}

std::vector<Entity*> Universe::getEntities()
{
	return entities;
}

void Universe::nuke()
{
	DynamicLightWorld::nuke();
	//The universe entities vector should be a comprehensive list of all entities.
	for(std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
		delete *it;
		*it = NULL;
	}
	entities.clear();
}
