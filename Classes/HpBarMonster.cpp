#include "HpBarMonster.h"

USING_NS_CC;

HpBarMonster::HpBarMonster() : 
_hideDelay(0), 
_groupType(-1)
{

}

bool HpBarMonster::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	_lowerBar = Sprite::create();
	this->addChild(_lowerBar);
	_upperProgress = ProgressTimer::create(Sprite::create());
	_upperProgress->setType(ProgressTimer::Type::BAR);
	_upperProgress->setMidpoint(Vec2(0, 1));
	_upperProgress->setBarChangeRate(Vec2(1, 0));
	this->addChild(_upperProgress);
	return true;
}

void HpBarMonster::initialize(const int &groupType)
{
	if (_groupType != groupType)
	{
		if (_upperProgress->getPercentage() == 100)
		{
			_upperProgress->setPercentage(0);
		}
		_lowerBar->setSpriteFrame(groupType == GROUP_TYPE_JUST ? "hpbar_frame2.png" : "hpbar_frame.png");
		_upperProgress->getSprite()->setSpriteFrame(groupType == GROUP_TYPE_JUST ? "hpbar_hp2.png" : "hpbar_hp.png");
		auto size = _upperProgress->getSprite()->getContentSize();
		_upperProgress->setPosition(-size.width * 0.5f, -size.height * 0.5f);
		_upperProgress->setPercentage(100);
	}
	_groupType = groupType;
}

void HpBarMonster::updateView(const unsigned int &currentHp, const int &finalHp)
{
	if (currentHp <= 0)
	{
		_hideDelay = 0;
		this->unscheduleUpdate();
		this->setVisible(false);
		return;
	}
	float percent = 100 * float(currentHp) / finalHp;
	if (percent > 100)
	{
		percent = 100;
	}
	_upperProgress->setPercentage(percent);
	this->setVisible(true);
	if (_hideDelay <= 0)
	{
		this->scheduleUpdate();
	}
	_hideDelay = HIDE_DELAY;
}

void HpBarMonster::update(float delta)
{
	if (_hideDelay <= 0)
	{
		this->unscheduleUpdate();
		return;
	}
	_hideDelay -= delta;
	if (_hideDelay <= 0)
	{
		this->unscheduleUpdate();
		this->setVisible(false);
	}
}