#ifndef __GRENADE_H__
#define __GRENADE_H__

#include "cocos2d.h"

class Grenade : public cocos2d::Sprite
{
public:
	Grenade() = default;
	virtual ~Grenade() = default;
	CREATE_FUNC(Grenade);
	void update(float delta) override;
	void shoot(const int &skillId, const cocos2d::Vec2 &dest);
private:
	int _skillId;
	float _radian;
	cocos2d::Vec2 _dest;
	float _bulletSpeed;

	float _speedZ;
	float _speedAcc;
	int _scaleGap;
	static const float GRAVITY;
	static const float BULLET_SPEED;
	static const int SCALE_GAP = 4;
	static const int MAX_DISTANCE_SQ = 90000;
	void bombOnce();
	void hitEffect();
	void hitEffectEnd(cocos2d::Sprite* sprite);
	void bomb(float dt);
};

#endif