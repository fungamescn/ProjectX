#ifndef __HPBARHERO_H__
#define __HPBARHERO_H__

#include "HpBarBase.h"

class HpBarHero : public HpBarBase
{
public:
	HpBarHero() = default;
	~HpBarHero() = default;
	CREATE_FUNC(HpBarHero);
	virtual bool init() override;
	virtual void initialize(const int &groupType /* = GROUP_TYPE_HERO */) override;
	virtual void updateView(const unsigned int &currentHp, const int &finalHp) override;
private:
	cocos2d::Sprite* _lowerCircle = nullptr;
	cocos2d::ProgressTimer* _leftProgress = nullptr;
	cocos2d::ProgressTimer* _rightProgress = nullptr;
};

#endif