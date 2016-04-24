#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"
#include "OBB2d.h"
#include "Role.h"
#include <unordered_set>
#include "Config.h"

class Bullet : public cocos2d::Sprite
{
public:
	Bullet();
	~Bullet();

	CREATE_FUNC(Bullet);
	void update(float delta) override;
	void shoot(Role* attacker, Base* injurer);
	OBB2d* getOBB2d();
	void recycle();
private:
	static const int CHECK_RATIO_SQ = 10000;
	float _aliveTime;
	int _hitChance;
	int _updateCount;
	double _radian;
	Role* _attacker = nullptr;
	OBB2d* _obb2d = nullptr;
	cocos2d::MotionStreak* _streak = nullptr;
	std::unordered_set<int> _crashSet;
	static const int HIT_INFINITY = -1;
	bool bomb(bool clear = false);
	void hitEffect(Base* base = nullptr);
	void hitEffectEnd(cocos2d::Sprite* sprite);
#if DEBUG_MODE
	cocos2d::DrawNode* _drawNode = nullptr;
#endif
};

#endif