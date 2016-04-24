#include "BulletPool.h"
#include "BattleLayer.h"

USING_NS_CC;

BulletPool* BulletPool::_instance = nullptr;

BulletPool::~BulletPool()
{
	this->clearAll();
}

BulletPool* BulletPool::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new(std::nothrow) BulletPool();
	}
	return _instance;
}

Bullet* BulletPool::getBullet()
{
	Bullet* bullet;
	if (_bulletVector.size() <= 0)
	{
		bullet = Bullet::create();
	}
	else
	{
		bullet = _bulletVector.back();
		_bulletVector.popBack();
	}
	auto bulletParent = bullet->getParent();
	if (!bulletParent)
	{
		BattleLayer::getInstance()->addChild(bullet, BULLET_ZORDER);
	}
	else if (bulletParent != BattleLayer::getInstance())
	{
		bullet->removeFromParent();
		BattleLayer::getInstance()->addChild(bullet, BULLET_ZORDER);
	}
	return bullet;
}

void BulletPool::recycleBullet(Bullet* bullet)
{
	bullet->cleanup();
	bullet->init();
	bullet->setVisible(false);
	_bulletVector.pushBack(bullet);
}

cocos2d::Sprite* BulletPool::getSprite()
{
	Sprite* sprite;
	if (_spriteVector.size() <= 0)
	{
		sprite = Sprite::create();
	}
	else
	{
		sprite = _spriteVector.back();
		_spriteVector.popBack();
	}
	auto spriteParent = sprite->getParent();
	if (!spriteParent)
	{
		BattleLayer::getInstance()->addChild(sprite, BULLET_ZORDER);
	}
	else if (spriteParent != BattleLayer::getInstance())
	{
		sprite->removeFromParent();
		BattleLayer::getInstance()->addChild(sprite, BULLET_ZORDER);
	}
	return sprite;
}

void BulletPool::recycleSprite(cocos2d::Sprite* sprite)
{
	sprite->cleanup();
	sprite->init();
	sprite->setVisible(false);
	_spriteVector.pushBack(sprite);
}

Grenade* BulletPool::getGrenade()
{
	Grenade* grenade;
	if (_grenadeVector.size() <= 0)
	{
		grenade = Grenade::create();
	}
	else
	{
		grenade = _grenadeVector.back();
		_grenadeVector.popBack();
	}
	auto grenadeParent = grenade->getParent();
	if (!grenadeParent)
	{
		BattleLayer::getInstance()->addChild(grenade, SKILL_ZORDER);
	}
	else if (grenadeParent != BattleLayer::getInstance())
	{
		grenade->removeFromParent();
		BattleLayer::getInstance()->addChild(grenade, SKILL_ZORDER);
	}
	return grenade;
}

void BulletPool::recycleGrenade(Grenade* grenade)
{
	grenade->cleanup();
	grenade->init();
	grenade->setVisible(false);
	_grenadeVector.pushBack(grenade);
}

Rocket* BulletPool::getRocket()
{
	Rocket* rocket;
	if (_rocketVector.size() <= 0)
	{
		rocket = Rocket::create();
	}
	else
	{
		rocket = _rocketVector.back();
		_rocketVector.popBack();
	}
	auto rocketParent = rocket->getParent();
	if (!rocketParent)
	{
		BattleLayer::getInstance()->addChild(rocket, SKILL_ZORDER);
	}
	else if (rocketParent != BattleLayer::getInstance())
	{
		rocket->removeFromParent();
		BattleLayer::getInstance()->addChild(rocket, SKILL_ZORDER);
	}
	return rocket;
}

void BulletPool::recycleRocket(Rocket* rocket)
{
	rocket->cleanup();
	rocket->init();
	rocket->setVisible(false);
	_rocketVector.pushBack(rocket);
}

Nuclear* BulletPool::getNuclear()
{
	Nuclear* nuclear;
	if (_nuclearVector.size() <= 0)
	{
		nuclear = Nuclear::create();
	}
	else
	{
		nuclear = _nuclearVector.back();
		_nuclearVector.popBack();
	}
	auto nuclearParent = nuclear->getParent();
	if (!nuclearParent)
	{
		BattleLayer::getInstance()->addChild(nuclear, SKILL_ZORDER);
	}
	else if (nuclearParent != BattleLayer::getInstance())
	{
		nuclear->removeFromParent();
		BattleLayer::getInstance()->addChild(nuclear, SKILL_ZORDER);
	}
	return nuclear;
}

void BulletPool::recycleNuclear(Nuclear* nuclear)
{
	nuclear->setVisible(false);
	_nuclearVector.pushBack(nuclear);
}

void BulletPool::clearAll()
{
	_bulletVector.clear();
	_bulletVector.shrinkToFit();

	_spriteVector.clear();
	_spriteVector.shrinkToFit();

	_grenadeVector.clear();
	_grenadeVector.shrinkToFit();

	_rocketVector.clear();
	_rocketVector.shrinkToFit();

	_nuclearVector.clear();
	_nuclearVector.shrinkToFit();
}