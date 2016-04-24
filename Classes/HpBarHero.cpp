#include "HpBarHero.h"

USING_NS_CC;

bool HpBarHero::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	_lowerCircle = Sprite::createWithSpriteFrameName("heroHp_green.png");
	this->addChild(_lowerCircle);

	auto upperCircle = Sprite::createWithSpriteFrameName("heroHp_red.png");
	_leftProgress = ProgressTimer::create(upperCircle);
	_leftProgress->setType(ProgressTimer::Type::RADIAL);
	this->addChild(_leftProgress);
	
	_rightProgress = ProgressTimer::create(upperCircle);
	_rightProgress->setType(ProgressTimer::Type::RADIAL);
	_rightProgress->setReverseProgress(true);
	this->addChild(_rightProgress);
	
	return true;
}

void HpBarHero::initialize(const int &groupType)
{
}

void HpBarHero::updateView(const unsigned int &currentHp, const int &finalHp)
{
	int percent = round(float(currentHp) / finalHp * 100);
	auto halfPercent = (100 - percent) >> 1;
	_leftProgress->setPercentage(halfPercent);
	_rightProgress->setPercentage(halfPercent);
}