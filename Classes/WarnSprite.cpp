#include "WarnSprite.h"

USING_NS_CC;

WarnSprite::WarnSprite(){}

WarnSprite::~WarnSprite(){}

bool WarnSprite::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

WarnSprite * WarnSprite::createWarnSprite(int type)
{
	WarnSprite * warnSprite = new WarnSprite();
	warnSprite->init();
	warnSprite->initSpriteChild(type);
	return warnSprite;
}

void WarnSprite::initSpriteChild(int type)
{
	switch (type)
	{
		case WARN_DEAD:
			_animationStr = "revive_warn";
			break;
		case WARN_QUEST:
			_animationStr = "quest_warn";
			break;
		case WARN_ENEMY:
			_animationStr = "enemy_warn";
			break;
		case WARN_TOUCH:
			_animationStr = "movePoint";// "touch_warn";
			break;
	}
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("warnSprite.plist");
	//AnimationCache::getInstance()->addAnimationsWithFile("warnSpriteAnimation.plist");
	playAnimation();
}

void WarnSprite::playAnimation()
{
	auto animation = AnimationCache::getInstance()->getAnimation(_animationStr);
	Action* action = Animate::create(animation);
	this->runAction(RepeatForever::create(static_cast<ActionInterval*>(action)));
}
