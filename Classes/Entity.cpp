#include "cocos2d.h"
#include "Planet.h"
#include "Star.h"
#include "Box2D.h"
#include <list>
#include "BodyUtil.h"

USING_NS_CC;

Entity::Entity(b2World* world, b2Body* body): world(world), body(body){};

Entity* Entity::makePlanet(b2World* world, float density, Vec2 pos, Sprite* sprite)
{
	return new Planet(world, BodyUtil::createCircularBody(world, density, pos, sprite));
}

Entity* Entity::makeStar(b2World* world, float density, Vec2 pos, Sprite* sprite)
{
	return new Star(world, BodyUtil::createCircularBody(world, density, pos, sprite));
}

Sprite* Entity::getSprite()
{
    Sprite* data = (Sprite*)body->GetUserData();
    return data;
}

Sprite* Entity::updateSprite()
{
    Sprite* data = (Sprite*)body->GetUserData();
    Vec2 pos = Vec2(body->GetPosition().x, body->GetPosition().y);
    data->setPosition(pos);
    return data;
}

void Entity::applyForce(Vec2 force)
{
	body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void Entity::applyImpulse(Vec2 impulse)
{
	body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), body->GetPosition(), true);
}

float Entity::getMass()
{
	return body->GetMass();
}

Vec2 Entity::getPos()
{
	b2Vec2 pos = body->GetPosition();
	return Vec2(pos.x, pos.y);
}

Vec2 Entity::getVel()
{
	b2Vec2 vel = body->GetLinearVelocity();
	return Vec2(vel.x, vel.y);
}

bool Entity::castRay(b2RayCastOutput* output, const b2RayCastInput& input)
{
	return body->GetFixtureList()->RayCast(output, input, 0);
}
