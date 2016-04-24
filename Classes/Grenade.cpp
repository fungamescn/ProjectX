#include "Grenade.h"
#include "BattleLayer.h"
#include "HurtLogic.h"
#include "BulletPool.h"
#include "Utils.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

const float Grenade::GRAVITY = 0.001f;
const float Grenade::BULLET_SPEED = 6.0f;

void Grenade::shoot(const int &skillId, const cocos2d::Vec2 &dest)
{
	_dest = dest;
	_skillId = skillId;

	auto position = this->getPosition();
	_radian = Utils::vecAngleToX(_dest - position);
	float ratio = _dest.getDistanceSq(position) / MAX_DISTANCE_SQ;
	if (ratio > 1)
	{
		ratio = 1;
	}
	_bulletSpeed = BULLET_SPEED * ratio;

	_speedZ = 0.0f;
	_speedAcc = 0.006f;
	_scaleGap = 0;
	this->setScale(1);

	auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, skillId);
	int repeat = atoi(skillMeta["repeat"].c_str());
	auto animation = AnimationCache::getInstance()->getAnimation("skill_grenade");
	Action* action = Animate::create(animation);
	if (repeat <= 0)
	{
		action = RepeatForever::create(static_cast<ActionInterval*>(action));
	}
	else
	{
		action = Repeat::create(static_cast<FiniteTimeAction*>(action), repeat);
	}
	this->runAction(action);
	this->setVisible(true);
	this->scheduleUpdate();
}

void Grenade::update(float delta)
{
	auto timeScale = Director::getInstance()->getScheduler()->getTimeScale();
	auto speed = _bulletSpeed * timeScale;
	this->setPositionX(this->getPositionX() + speed * cos(_radian));
	this->setPositionY(this->getPositionY() + speed * sin(_radian));
	if (++_scaleGap == SCALE_GAP)
	{
		_scaleGap = 0;
		auto currentZ = this->getScale();
		_speedZ += _speedAcc * timeScale;
		currentZ += _speedZ * timeScale;
		_speedAcc -= GRAVITY * timeScale;
		if (currentZ <= 1)
		{
			this->bombOnce();
			return;
		}
		this->setScale(currentZ);
	}
}

void Grenade::bombOnce()
{
	this->unscheduleUpdate();
	auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, _skillId);
	int delay = atoi(skillMeta["delay2"].c_str());
	scheduleOnce(schedule_selector(Grenade::bomb), delay);
}

void Grenade::hitEffect()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(MUSIC_EXPLOSION);
	auto animation = AnimationCache::getInstance()->getAnimation("skill_explosion");
	Action* action = Animate::create(animation);
	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setVisible(true);
	sprite->setAnchorPoint(Vec2(0.5f, 0.457f));
	sprite->setPosition(this->getPosition());
	auto endCall = CallFunc::create(CC_CALLBACK_0(Grenade::hitEffectEnd, this, sprite));
	action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
	sprite->runAction(action);
}

void Grenade::hitEffectEnd(cocos2d::Sprite* sprite)
{
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	BulletPool::getInstance()->recycleSprite(sprite);
}

void Grenade::bomb(float dt)
{
	this->hitEffect();
	META_LINE keys = { { "level", "1" }, { "skillId", Utils::int2String(_skillId) } };
	auto skillMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_SKILL_LEVEL, keys);
	int attGroup = atoi(skillMeta["attGroup"].c_str());
	auto radius = atoi(skillMeta["radius"].c_str());
	auto radiusSq = radius * radius;
	auto origin = this->getPosition();
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		auto baseGroup = BattleLayer::getInstance()->getBaseVectorByGroupType(type);
		for (auto base : baseGroup)
		{
			if (base->getCurrentHp() <= 0 || origin.getDistanceSq(base->getShadowPoint()) > radiusSq)
			{
				continue;
			}
			HurtLogic::skillHurtHp(skillMeta, base);
		}
	}
	BulletPool::getInstance()->recycleGrenade(this);
}