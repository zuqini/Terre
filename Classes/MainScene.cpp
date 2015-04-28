#include "MainScene.h"
#include "Select.h"
#include "LightRay.h"

#define UNSELECT 0
#define SELECT 1

#define WORLD 0
#define OVERLAY 1

#define OBJECTS 0
#define LIGHT 1
#define HUD 2

#define ZOOM_MULTIPLIER 0.0005f

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
    _selected = NONE;
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
    auto objects = Node::create();
    auto lightNode = DrawNode::create();
    auto HUDNode = DrawNode::create();
    world->addChild(objects, 0, OBJECTS);
    world->addChild(lightNode, 10, LIGHT);
    world->addChild(HUDNode, 11, HUD);
    for (std::vector<Entity*>::const_iterator iterator = entities.begin(); iterator != entities.end(); ++iterator) {
    	objects->addChild((*iterator)->getSprite());
    	log("ADDED SPRITE");
    }
    scaleCenter(world, 0.3, origin);

	_starUnselect = MenuItemImage::create(
							   "starNormal.png",
							   "starSelect.png",
							   CC_CALLBACK_1(MainScene::starCallBack, this));

	_starSelect = MenuItemImage::create(
							   "starFinal.png",
							   "starSelect.png",
							   CC_CALLBACK_1(MainScene::starCallBack, this));

	_starToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(MainScene::starCallBack, this), _starUnselect, _starSelect, NULL);
	_starToggle->setPosition(Vec2(origin.x + visibleSize.width - _starToggle->getContentSize().width,
								origin.y + _starToggle->getContentSize().height));

	_planetUnselect = MenuItemImage::create(
							   "planetNormal.png",
							   "planetSelect.png",
							   CC_CALLBACK_1(MainScene::planetCallBack, this));

	_planetSelect = MenuItemImage::create(
							   "planetFinal.png",
							   "planetSelect.png",
							   CC_CALLBACK_1(MainScene::planetCallBack, this));
	_planetToggle = MenuItemToggle::createWithCallback(CC_CALLBACK_1(MainScene::planetCallBack, this), _planetUnselect, _planetSelect, NULL);
	_planetToggle->setPosition(Vec2(origin.x + visibleSize.width - _planetToggle->getContentSize().width,
								origin.y + 150 + _planetToggle->getContentSize().height));

    MenuItemImage* clearButton = MenuItemImage::create(
    							   "clearNormal.png",
    							   "clearSelect.png",
    							   CC_CALLBACK_1(MainScene::clearCallBack, this));
    clearButton->setPosition(Vec2(origin.x + visibleSize.width - clearButton->getContentSize().width,
								origin.y + 300 + clearButton->getContentSize().height));

	// create menu, it's an autorelease object
	auto overLay = Menu::create();
	overLay->addChild(_starToggle);
	overLay->addChild(_planetToggle);
	overLay->addChild(clearButton);
	overLay->setPosition(Vec2::ZERO);
	this->addChild(overLay, 1, OVERLAY);

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

    Vec2 currCenter = node->getScale() * center;
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
		float distDelta = currDist - initDist;
		Vec2 pinchCenter = initT1.getMidpoint(initT2);
		pinchCenter = world->convertToNodeSpace(pinchCenter);
		float zoom = world->getScale() + distDelta * ZOOM_MULTIPLIER;
		zoom = zoom < 0.015 ? 0.015 : zoom;
		zoom = zoom > 0.3 ? 0.3 : zoom;
		scaleCenter(world, zoom, pinchCenter);
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
	if(_selected == STAR || _selected == PLANET)
	{
		auto world = this->getChildByTag(WORLD);
		auto objects = world->getChildByTag(OBJECTS);
		auto drawNode = (DrawNode*)world->getChildByTag(HUD);
		drawNode->clear();

		Vec2 start = world->convertToNodeSpace(touch->getStartLocation());
		Vec2 end = world->convertToNodeSpace(touch->getLocation());
		Vec2 diff = end - start;

		Vec2 loc = start;

		Entity* entity = _selected == STAR ? (Entity*)universe.createStarAt(loc) : (Entity*)universe.createPlanetAt(loc);
		diff = entity->getMass() * diff;
		log("%f", diff.getLength());
		entity->applyImpulse(diff);
		objects->addChild(entity->getSprite());

		log("success");
	}
}

void MainScene::onTouchMoved(Touch* touch, Event* event)
{
	auto world = this->getChildByTag(WORLD);
	if(_selected == NONE)
	{
		//move
		Vec2 diff = touch->getDelta();
		Vec2 currPos = world->getPosition();
		currPos.add(diff);
		world->setPosition(currPos);
	} else {
		auto drawNode = (DrawNode*)world->getChildByTag(HUD);
		drawNode->clear();
		Vec2 start = world->convertToNodeSpace(touch->getStartLocation());
		Vec2 end = world->convertToNodeSpace(touch->getLocation());
		drawNode->drawLine(start, end, Color4F::GREEN);
	}
}

void MainScene::onTouchCancelled(Touch* touch, Event* event)
{
}

void MainScene::update(float dt){
	auto drawNode = (DrawNode*)this->getChildByTag(WORLD)->getChildByTag(LIGHT);
	accumulator += dt;
	while(accumulator > delta)
	{
		universe.step(delta);
		accumulator -= delta;
	}
	universe.updatePos();
	universe.updateLight(drawNode);
}

void MainScene::starCallBack(Ref* pSender)
{
	MenuItemToggle* starToggle = (MenuItemToggle *)(pSender);
	if(starToggle->getSelectedItem() == _starSelect)
	{
		_selected = STAR;
		_planetToggle->setSelectedIndex(UNSELECT);
	} else if(starToggle->getSelectedItem() == _starUnselect)
	{
		_selected = NONE;
	}
}

void MainScene::planetCallBack(Ref* pSender)
{
	MenuItemToggle* planetToggle = (MenuItemToggle *)(pSender);
	if(planetToggle->getSelectedItem() == _planetSelect)
	{
		_selected = PLANET;
		_starToggle->setSelectedIndex(UNSELECT);
	} else if(planetToggle->getSelectedItem() == _planetUnselect)
	{
		_selected = NONE;
	}
}

void MainScene::clearCallBack(Ref* pSender)
{
	MenuItemImage* clearButton = (MenuItemImage*)(pSender);
	universe.nuke();
	this->getChildByTag(WORLD)->getChildByTag(OBJECTS)->removeAllChildrenWithCleanup(true);
}
