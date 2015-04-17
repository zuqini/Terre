#include "MainScene.h"
#include "LightRay.h"

#define WORLD 0
#define DRAW 1

#define ZOOM_MULTIPLIER 0.005f

USING_NS_CC;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto touchListener = EventListenerTouchOneByOne::create();
    auto multiTouchListener = EventListenerTouchAllAtOnce::create();

	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(MainScene::onTouchCancelled, this);

	multiTouchListener->onTouchesBegan = CC_CALLBACK_2(MainScene::onTouchesBegan, this);
	multiTouchListener->onTouchesEnded = CC_CALLBACK_2(MainScene::onTouchesEnded, this);
	multiTouchListener->onTouchesMoved = CC_CALLBACK_2(MainScene::onTouchesMoved, this);
	multiTouchListener->onTouchesCancelled = CC_CALLBACK_2(MainScene::onTouchesCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(multiTouchListener, this);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    universe.generateEntities(origin, visibleSize);
    std::vector<Entity*> entities = universe.getEntities();

    auto world = Node::create();
    world->setPosition(origin);
    auto drawNode = DrawNode::create();
    world->addChild(drawNode, 10, DRAW);
    for (std::vector<Entity*>::const_iterator iterator = entities.begin(); iterator != entities.end(); ++iterator) {
    	world->addChild((*iterator)->getUpdateSprite());
    }
    this->addChild(world, 0 , WORLD);
    this->scheduleUpdate();
    return true;
}

void MainScene::scaleCenter(Node* node, float scale, Vec2 center)
{
    // Get the new center point.
    Vec2 initCenter = node->getScale() * center;
    // Set the scale.
    node->setScale(scale);

    Vec2 currCenter = scale * center;
    Vec2 centerPointDelta = initCenter - currCenter;
    node->setPosition(node->getPosition() + centerPointDelta);
}

bool MainScene::onTouchesBegan(std::vector<Touch*> touches, Event* event)
{
}

void MainScene::onTouchesEnded(std::vector<Touch*> touches, Event* event)
{
}

void MainScene::onTouchesMoved(std::vector<Touch*> touches, Event* event)
{
	if(touches.size() == 2)
	{
		auto world = this->getChildByTag(WORLD);
		//zoom
		Vec2 initT1 = touches[0]->getPreviousLocation();
		Vec2 initT2 = touches[1]->getPreviousLocation();
		float initDist = initT1.distance(initT2);
		Vec2 currT1 = touches[0]->getLocation();
		Vec2 currT2 = touches[1]->getLocation();
		float currDist = currT1.distance(currT2);

		float distDelta = initDist - currDist;
		Vec2 pinchCenter = initT1.getMidpoint(initT2);

		scaleCenter(world, world->getScale() - distDelta * ZOOM_MULTIPLIER, pinchCenter);
	}
}

void MainScene::onTouchesCancelled(std::vector<Touch*> touches, Event* event)
{
}

bool MainScene::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void MainScene::onTouchEnded(Touch* touch, Event* event)
{
}

void MainScene::onTouchMoved(Touch* touch, Event* event)
{
	auto world = this->getChildByTag(WORLD);
	//move
	Vec2 diff = touch->getDelta();
	Vec2 currPos = world->getPosition();
	currPos.add(diff);
	world->setPosition(currPos);
}

void MainScene::onTouchCancelled(Touch* touch, Event* event)
{
}

/**
 * @TODO
 * this method could(should) be improved.
 */
void MainScene::intensifyLight(struct LightRay ray1, struct LightRay ray2)
{
	auto drawNode = (DrawNode*)this->getChildByTag(WORLD)->getChildByTag(DRAW);

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
	drawNode->drawPolygon(verts, 3, Color4F(1, 1, 0, 1), 0, Color4F::BLACK);

	Vec2 p3;
	Vec2 p4;
	auto factor = 0;
	while(length1 <= ray1.frac || length2 <= ray2.frac)
	{
		auto length  = pow(1.02, factor);
		length1 = length1 + length < ray1.frac ? length1 + length : ray1.frac;
		length2 = length2 + length < ray2.frac ? length2 + length : ray2.frac;

		p3 = ray1.p1 + (ray1.p2 - ray1.p1) * length1;
		p4 = ray2.p1 + (ray2.p2 - ray2.p1) * length2;
		auto strength = pow(0.97, factor++);
		if(strength < 0.005) {
			break;
		}
		Vec2 verts1[4] = {
			p1, p3, p4, p2
		};
		drawNode->drawPolygon(verts1, 4, Color4F(1, 1, 0, strength), 0, Color4F(0,0,0,0));
		p1 = Vec2(p3);
		p2 = Vec2(p4);

		if(length1 == ray1.frac || length2 == ray2.frac)
			break;
	}
}

void MainScene::update(float dt){
	accumulator += dt;
	while(accumulator > delta)
	{
		universe.step(delta);
		accumulator -= delta;
	}
	universe.updatePos();
	std::vector<struct LightRay> rays = universe.getRays();
	auto length = rays.size();

	auto drawNode = (DrawNode*)this->getChildByTag(WORLD)->getChildByTag(DRAW);
	drawNode->clear();
	//drawNode->drawLine(rays[0].p1, rays[0].p2 + (rays[0].p2 - rays[0].p1) * rays[0].frac, Color4F::GREEN);
	for(auto i = 1; i < length; i++)
	{
		intensifyLight(rays[i-1], rays[i]);
		//drawNode->drawLine(rays[i].p1, rays[i].p2 + (rays[i].p2 - rays[i].p1) * rays[i].frac, Color4F::GREEN);
	}
	intensifyLight(rays[0], rays[length - 1]);
}

void MainScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
