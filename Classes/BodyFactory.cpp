#include "cocos2d.h"
#include "BodyFactory.h"
#include "Box2D.h"

USING_NS_CC;

namespace BodyFactory
{
	b2Body* createCircularBody(b2World* world, float density, Vec2 pos, Sprite* sprite) {
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(pos.x, pos.y);
		bodyDef.userData = sprite;
		b2Body* body = world->CreateBody(&bodyDef);

		// Create circle shape
		b2CircleShape circle;
		circle.m_radius = sprite->getContentSize().width/2;

		// Create shape definition and add to body
		b2FixtureDef shapeDef;
		shapeDef.shape = &circle;
		shapeDef.density = density;
		shapeDef.friction = 100.f;
		shapeDef.restitution = 0.f;
		body->CreateFixture(&shapeDef);
		return body;
	}
}
