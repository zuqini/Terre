#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "Select.h"
#include "Universe.h"

class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void starCallBack(cocos2d::Ref* pSender);
    void planetCallBack(cocos2d::Ref* pSender);
    void addEntitySubs(Entity* entity);
    void update(float) override;

    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

    virtual bool onTouchesBegan(std::vector<cocos2d::Touch*>, cocos2d::Event*);
    virtual void onTouchesEnded(std::vector<cocos2d::Touch*>, cocos2d::Event*);
    virtual void onTouchesMoved(std::vector<cocos2d::Touch*>, cocos2d::Event*);
    virtual void onTouchesCancelled(std::vector<cocos2d::Touch*>, cocos2d::Event*);

    CREATE_FUNC(MainScene);
private:
    Select _selected;

    MenuItemToggle* _starToggle;
    MenuItemImage* _starUnselect;
    MenuItemImage* _starSelect;

    MenuItemToggle* _planetToggle;
    MenuItemImage* _planetUnselect;
    MenuItemImage* _planetSelect;

    void scaleCenter(cocos2d::Node*, float, cocos2d::Vec2);

    Universe universe;

    double accumulator;
    const double delta = 1./60;
};

#endif // __MAIN_SCENE_H__
