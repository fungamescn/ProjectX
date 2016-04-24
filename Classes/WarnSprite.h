#ifndef __WARN_SPRITE_H__
#define __WARN_SPRITE_H__

#include "cocos2d.h"

using namespace std;

enum WARN_TYPE
{
	WARN_DEAD = 1,
	WARN_QUEST = 2,
	WARN_ENEMY = 3,
	WARN_TOUCH = 4
};

class WarnSprite : public cocos2d::Sprite
{
public:
	WarnSprite();
	~WarnSprite();
	virtual bool init();
	static WarnSprite * createWarnSprite(int type);
	void initSpriteChild(int type);
private:
	cocos2d::Sprite * _directSprite;
	string _animationStr;
protected:
	void playAnimation();
};

#endif