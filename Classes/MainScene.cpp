#include "MainScene.h"

USING_NS_CC;

double MainScene::lastTickTime = 0;
double MainScene::accumulator = 0;
const double delta = 1./60;
double getCurrentTimeInSeconds()
{
  static struct timeval currentTime;
  gettimeofday(&currentTime, nullptr);
  return (currentTime.tv_sec) + (currentTime.tv_usec / 1000000.0);
}

void MainScene::resetTickTime()
{
	lastTickTime = 0;
}

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
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    universe.generateEntities(origin, visibleSize);
    std::vector<Entity*> entities = universe.getEntities();
    for (std::vector<Entity*>::const_iterator iterator = entities.begin(); iterator != entities.end(); ++iterator) {
    	this->addChild((*iterator)->getUpdateSprite(), 1);
    }
    this->scheduleUpdate();
    return true;
}

void MainScene::update(float dt){
	double currTickTime = getCurrentTimeInSeconds();
	if(lastTickTime == 0)	lastTickTime = currTickTime;
	double frameTime = currTickTime - lastTickTime;
	lastTickTime = currTickTime;
	accumulator += frameTime;

	while(accumulator > delta)
	{
		log("%f, %f", accumulator, delta);
		universe.step(delta);
		accumulator -= delta;
	}
	universe.render();
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
