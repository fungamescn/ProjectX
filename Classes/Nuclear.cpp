#include "Nuclear.h"
#include "BattleLayer.h"
#include "BulletPool.h"
#include "HurtLogic.h"
#include "Utils.h"
#include "StageLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

const float Nuclear::SCALE_SPEED = 0.03f;

bool Nuclear::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	_shadow = Sprite::createWithSpriteFrameName("skill_warheadShadow.png");
	this->addChild(_shadow);

	_rocket = Sprite::createWithSpriteFrameName("skill_warhead.png");
	_rocket->setAnchorPoint(Vec2(0.5f, 0));
	this->addChild(_rocket);

	return true;
}

void Nuclear::shoot(const int &skillId, const cocos2d::Vec2 &dest)
{
	_skillId = skillId;
	_destVec = dest;
	this->setPosition(dest);
	this->setVisible(true);

	_shadow->setScale(1);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	_rocket->setPositionY(visibleSize.height);

	this->scheduleUpdate();
}

void Nuclear::update(float delta)
{
	auto timeScale = Director::getInstance()->getScheduler()->getTimeScale();
	auto positionY = _rocket->getPositionY() - FALL_SPEED * timeScale;
	if (positionY <= 0)
	{
		this->bomb();
		return;
	}
	_rocket->setPositionY(positionY);
	_shadow->setScale(_shadow->getScale() + SCALE_SPEED * timeScale);
}

void Nuclear::bomb()
{
	this->hitEffect();
	META_LINE keys = { { "level", "1" }, { "skillId", Utils::int2String(_skillId) } };
	auto skillMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_SKILL_LEVEL, keys);
	int attGroup = atoi(skillMeta["attGroup"].c_str());
	auto origin = BattleLayer::getInstance()->convertToNodeSpace(Vec2::ZERO);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto rect = Rect(origin.x, origin.y, visibleSize.width, visibleSize.height);
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		for (auto base : BattleLayer::getInstance()->getBaseVectorByGroupType(type))
		{
			if (base->getCurrentHp() <= 0)
			{
				continue;
			}
			if (!rect.containsPoint(base->getShadowPoint()))
			{
				continue;
			}
			HurtLogic::skillHurtHp(skillMeta, base);
		}
	}
	this->recycle();
}

void Nuclear::hitEffect()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(MUSIC_EXPLOSION);
	auto animation = AnimationCache::getInstance()->getAnimation("skill_nuclear");
	Action* action = Animate::create(animation);
	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setVisible(true);
	sprite->setScale(2.0f);
	sprite->setAnchorPoint(Vec2(0.5f, 0.217f));
	sprite->setPosition(this->getPosition());
	auto endCall = CallFunc::create(CC_CALLBACK_0(Nuclear::hitEffectEnd, this, sprite));
	action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
	sprite->runAction(action);
	BattleLayer::getInstance()->startShake();
	StageLayer::getInstance()->startWhiteSmoke();
}

void Nuclear::hitEffectEnd(cocos2d::Sprite* sprite)
{
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	sprite->setScale(1.0f);
	BulletPool::getInstance()->recycleSprite(sprite);
}

void Nuclear::recycle()
{
	this->unscheduleUpdate();
	BulletPool::getInstance()->recycleNuclear(this);
}