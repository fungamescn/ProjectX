#ifndef __HPBARMONSTER_H__
#define __HPBARMONSTER_H__

#include "HpBarBase.h"
#include "cocos2d.h"

class HpBarMonster : public HpBarBase
{
public:
	HpBarMonster();
	~HpBarMonster() = default;

	CREATE_FUNC(HpBarMonster);
	virtual bool init() override;
	virtual void update(float delta) override;
	virtual void initialize(const int &groupType /* = GROUP_TYPE_HERO */);
	virtual void updateView(const unsigned int &currentHp, const int &finalHp) override;
private:
	cocos2d::Sprite* _lowerBar = nullptr;
	cocos2d::ProgressTimer* _upperProgress = nullptr;
	static const int HIDE_DELAY = 3;
	float _hideDelay;
	int _groupType;
};

#endif