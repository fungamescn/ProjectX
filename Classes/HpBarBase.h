#ifndef __HPBARBASE_H__
#define __HPBARBASE_H__

#include "cocos2d.h"
#include "Config.h"

class HpBarBase : public cocos2d::Sprite
{
public:
	HpBarBase() = default;
	virtual ~HpBarBase() = default;
	virtual void initialize(const int &groupType = GROUP_TYPE_HERO) = 0;
	virtual void updateView(const unsigned int &currentHp, const int &finalHp) = 0;
};

#endif