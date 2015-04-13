#include "Planet.h"

Planet::Planet(b2World* world, b2Body* body): Entity(world, body)
{
}

float Planet::getRadius()
{
	Sprite* sprite = (Sprite*)body->GetUserData();
	return sprite->getBoundingBox().size.width * sprite->getScale();
}
