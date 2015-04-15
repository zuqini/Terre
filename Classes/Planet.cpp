#include "Planet.h"

Planet::Planet(b2World* world, b2Body* body): Entity(world, body)
{
}

float Planet::getRadius()
{
	return body->GetFixtureList()->GetShape()->m_radius;
}
