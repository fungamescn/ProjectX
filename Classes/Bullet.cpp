#include "Bullet.h"
#include "BattleLayer.h"
#include "Utils.h"
#include "BulletPool.h"
#include "HurtLogic.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Bullet::Bullet() : _attacker(nullptr), _radian(0), _aliveTime(0), _updateCount(0)
{
}

Bullet::~Bullet()
{
	delete _obb2d;
	_crashSet.clear();
}

void Bullet::shoot(Role* attacker, Base* injurer)
{
	_attacker = attacker;
	_attacker->retainBullet();

	_updateCount = 0;
	auto attribute = _attacker->getAttribute();
	auto radian = Utils::vecAngleToX(injurer->getPosition() + injurer->aimOffset() - _attacker->getPosition());
	_aliveTime = attribute->getBulletDur();
	_hitChance = attribute->getMaxHit();
	if (_hitChance <= 0)
	{
		_hitChance = HIT_INFINITY;
	}
	auto bulletEffect = attribute->getBulletEffect();
	auto repeat = attribute->getBulletRepaet();
	auto animation = AnimationCache::getInstance()->getAnimation(bulletEffect);
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
	auto shootPoint = _attacker->getShootPoint() + _attacker->getPosition();
	this->setPosition(shootPoint);
	double accuracy = attribute->getAccuracy();
	accuracy = accuracy * M_PI * rand_0_1() / 180;
	if (rand_0_1() < 0.5f)
	{
		accuracy *= -1;
	}
	_radian = radian + accuracy;
	auto angle = Utils::getRotateAngle(180 * _radian / M_PI);
	this->setRotation(angle);
	this->setVisible(true);
	this->scheduleUpdate();
	auto tailData = attribute->getTailData();
	if (nullptr != tailData)
	{
		if (nullptr == _streak)
		{
			_streak = MotionStreak::create(tailData->fade, 5, tailData->stroke, tailData->color3b, "streak.png");
			_streak->setFastMode(true);
			BattleLayer::getInstance()->addChild(_streak, BULLET_ZORDER);
		}
		else
		{
			_streak->reset();
			_streak->initWithFade(tailData->fade, 5, tailData->stroke, tailData->color3b, "streak.png");
		}
		_streak->setPosition(shootPoint);
	}
	else if(nullptr != _streak)
	{
		_streak->removeFromParent();
		_streak = nullptr;
	}
	if (attribute->getBulletSound() != "")
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((MUSIC_FIRE + attribute->getBulletSound()).c_str());// "music/sound_fire.wav");//(MUSIC_FIRE + attribute->getBulletSound()).c_str());
#if DEBUG_MODE
	if(nullptr == _drawNode)
	{
		_drawNode = DrawNode::create();
		this->addChild(_drawNode);
	}
	else
	{
		_drawNode->clear();
	}
	auto origin = attribute->getBulletOrigin();
	auto size = attribute->getBuletObb();
	auto vertex0 = Vec2(origin.x, origin.y);
	auto vertex1 = Vec2(origin.x + size.width, origin.y);
	auto vertex2 = Vec2(origin.x + size.width, origin.y + size.height);
	auto vertex3 = Vec2(origin.x, origin.y + size.height);
	Vec2 points[] = { vertex0, vertex1, vertex2, vertex3 };
	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));
#endif
}

void Bullet::update(float delta)
{
	if (nullptr == _attacker)
	{
		this->unscheduleUpdate();
		return;
	}
	auto attribute = _attacker->getAttribute();
	auto speed = attribute->getBulletSpeed() * delta;
	auto position = this->getPosition();
	this->setPosition(position.x + speed * cos(_radian), position.y + speed * sin(_radian));
	if (nullptr != _streak)
	{
		_streak->setPosition(this->getPosition());
	}
	if ((_aliveTime -= delta) <= 0)
	{
		this->bomb(true);
		return;
	}
	auto attGroup = attribute->getAttGroup();
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		auto baseGroup = BattleLayer::getInstance()->getBaseVectorByGroupType(type);
		for (auto base : baseGroup)
		{
			if (base->getCurrentHp() <= 0)
			{
				continue;
			}
			if (_crashSet.find(base->getTag()) != _crashSet.end())
			{
				continue;
			}
			if (position.getDistanceSq(base->getShadowPoint()) > CHECK_RATIO_SQ)
			{
				continue;
			}
			auto bulletObb = this->getOBB2d();
			auto roleObb = base->getOBB2d();
			if (bulletObb->isCollidWithOBB(roleObb))
			{
				if (this->bomb())
				{
					return;
				}
				this->hitEffect(base);
				HurtLogic::hurtHp(_attacker, base, attribute->isUseShield());
				if (base->getBaseType() == BASE_TYPE_ELEMENT)
				{
					this->recycle();
					return;
				}
				_crashSet.insert(base->getTag());
				if (_hitChance != HIT_INFINITY && --_hitChance <= 0)
				{
					this->recycle();
					return;
				}
			}
		}
	}
}

OBB2d* Bullet::getOBB2d()
{
	if (!_obb2d)
	{
		_obb2d = new OBB2d();
	}
	auto attribute = _attacker->getAttribute();
	auto origin = attribute->getBulletOrigin();
	auto size = attribute->getBuletObb();

	auto vertex0 = BattleLayer::getInstance()->convertToNodeSpace(this->convertToWorldSpace(Vec2(origin.x, origin.y)));
	auto vertex1 = BattleLayer::getInstance()->convertToNodeSpace(this->convertToWorldSpace(Vec2(origin.x + size.width, origin.y)));
	auto vertex2 = BattleLayer::getInstance()->convertToNodeSpace(this->convertToWorldSpace(Vec2(origin.x + size.width, origin.y + size.height)));
	auto vertex3 = BattleLayer::getInstance()->convertToNodeSpace(this->convertToWorldSpace(Vec2(origin.x, origin.y + size.height)));

	_obb2d->setVertex(0, vertex0);
	_obb2d->setVertex(1, vertex1);
	_obb2d->setVertex(2, vertex2);
	_obb2d->setVertex(3, vertex3);

	return _obb2d;
}

void Bullet::recycle()
{
	this->unscheduleUpdate();
	if (this->isVisible())
	{
		_attacker->releaseBullet();
	}
	_attacker = nullptr;
	_crashSet.clear();
	BulletPool::getInstance()->recycleBullet(this);
}

bool Bullet::bomb(bool clear /* = false */)
{
	if (nullptr == _attacker)
	{
		return false;
	}
	auto attribute = _attacker->getAttribute();
	auto radius = attribute->getBombRadius();
	if (radius <= 0)
	{
		if(clear) this->recycle();
		return false;
	}
	this->hitEffect();
	auto origin = attribute->getBulletOrigin();
	auto size = attribute->getBuletObb();
	auto position = BattleLayer::getInstance()->convertToNodeSpace(this->convertToWorldSpace(Vec2(origin.x + size.width / 2, origin.y + size.height / 2)));
	auto radiusSq = radius * radius;
	auto attGroup = attribute->getAttGroup();
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		auto baseGroup = BattleLayer::getInstance()->getBaseVectorByGroupType(type);
		for (auto base : baseGroup)
		{
	 		if (base->getCurrentHp() <= 0)
			{
				continue;
			}
			auto shadow = base->getShadowPoint();
			if (position.getDistanceSq(shadow) > radiusSq)
			{
				continue;
			}
			auto attribute = _attacker->getAttribute();
			HurtLogic::hurtHp(_attacker, base, attribute->isUseShield());
		}
	}
	this->recycle();
	return true;
}

void Bullet::hitEffect(Base* base /* = nullptr */)
{
	if (nullptr == _attacker)
	{
		return;
	}
	int effect = 2;
	if (nullptr != base)
	{
		auto materialType = base->getMaterialType();
		effect = materialType == MATER_TYPE_HERO || materialType == MATER_TYPE_LIVE ? 0 : 1;
	}
	auto attribute = _attacker->getAttribute();
	auto effectName = attribute->getHitEffect(effect);
	auto animation = AnimationCache::getInstance()->getAnimation(effectName);
	auto action = Animate::create(animation);
	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setVisible(true);
	sprite->setPosition(this->getPosition());
	auto endCall = CallFunc::create(CC_CALLBACK_0(Bullet::hitEffectEnd, this, sprite));
	sprite->runAction(Sequence::create(action, endCall, NULL));
}

void Bullet::hitEffectEnd(cocos2d::Sprite* sprite)
{
	BulletPool::getInstance()->recycleSprite(sprite);
}