#ifndef __NUCLEAR_H__
#define __NUCLEAR_H__

#include "cocos2d.h"

class Nuclear : public cocos2d::Sprite
{
public:
	Nuclear() = default;
	~Nuclear() = default;
	CREATE_FUNC(Nuclear);
	virtual bool init() override;
	void shoot(const int &skillId, const cocos2d::Vec2 &dest);
	virtual void update(float delta) override;
private:
	static const int FALL_SPEED = 10;
	static const float SCALE_SPEED;

	int _skillId;
	cocos2d::Vec2 _destVec;
	cocos2d::Sprite* _rocket;
	cocos2d::Sprite* _shadow;
	void bomb();
	void hitEffect();
	void hitEffectEnd(cocos2d::Sprite* sprite);
	void recycle();
};

#endif