#include "Rocket.h"
#include "BattleLayer.h"
#include "BulletPool.h"
#include "HurtLogic.h"
#include "Utils.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

void Rocket::shoot(const int &skillId, const cocos2d::Vec2 &dest)
{
	_skillId = skillId;
	_destVec = dest;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Vec2(dest.x, dest.y + visibleSize.height);
	this->setPosition(origin);
	this->setVisible(true);
	auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, skillId);
	int repeat = atoi(skillMeta["repeat"].c_str());
	auto animation = AnimationCache::getInstance()->getAnimation("skill_missile");
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
	this->scheduleUpdate();
}

void Rocket::update(float delta)
{
	auto timeScale = Director::getInstance()->getScheduler()->getTimeScale();
	auto positionY = this->getPositionY() - FALL_SPEED * timeScale;
	if (positionY <= _destVec.y)
	{
		this->bomb();
		return;
	}
	this->setPositionY(positionY);
}

void Rocket::bomb()
{
	this->hitEffect();
	META_LINE keys = { { "level", "1" }, { "skillId", Utils::int2String(_skillId) } };
	auto skillMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_SKILL_LEVEL, keys);
	int attGroup = atoi(skillMeta["attGroup"].c_str());
	auto radius = atoi(skillMeta["radius"].c_str());
	auto radiusSq = radius * radius;
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		for (auto base : BattleLayer::getInstance()->getBaseVectorByGroupType(type))
		{
			if (base->getCurrentHp() <= 0 || _destVec.getDistanceSq(base->getShadowPoint()) > radiusSq)
			{
				continue;
			}
			HurtLogic::skillHurtHp(skillMeta, base);
		}
	}
	BulletPool::getInstance()->recycleRocket(this);
}

void Rocket::hitEffect()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(MUSIC_EXPLOSION);
	auto animation = AnimationCache::getInstance()->getAnimation("skill_airStrike");
	Action* action = Animate::create(animation);
	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setScale(1.1f);
	sprite->setVisible(true);
	sprite->setAnchorPoint(Vec2(0.5f, 0.406f));
	sprite->setPosition(this->getPosition());
	auto endCall = CallFunc::create(CC_CALLBACK_0(Rocket::hitEffectEnd, this, sprite));
	action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
	sprite->runAction(action);
	BattleLayer::getInstance()->startShake();
}

void Rocket::hitEffectEnd(cocos2d::Sprite* sprite)
{
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	sprite->setScale(1);
	BulletPool::getInstance()->recycleSprite(sprite);
}