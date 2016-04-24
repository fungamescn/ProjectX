#ifndef __ROCKET_H__
#define __ROCKET_H__

#include "cocos2d.h"

class Rocket : public cocos2d::Sprite
{
public:
	static const int FALL_SPEED = 8;
	Rocket() = default;
	~Rocket() = default;
	CREATE_FUNC(Rocket);
	void shoot(const int &skillId, const cocos2d::Vec2 &dest);
	virtual void update(float delta) override;
private:
	int _skillId;
	cocos2d::Vec2 _destVec;
	void bomb();
	void hitEffect();
	void hitEffectEnd(cocos2d::Sprite* sprite);
};

#endif