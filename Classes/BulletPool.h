#ifndef __BULLETPOOL_H__
#define __BULLETPOOL_H__

#include "cocos2d.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Rocket.h"
#include "Nuclear.h"

class BulletPool
{
public:
	BulletPool() = default;
	~BulletPool();

	static BulletPool* getInstance();
	Bullet* getBullet();
	void recycleBullet(Bullet* bullet);
	cocos2d::Sprite* getSprite();
	void recycleSprite(cocos2d::Sprite* sprite);
	Grenade* getGrenade();
	void recycleGrenade(Grenade* grenade);
	Rocket* getRocket();
	void recycleRocket(Rocket* rocket);
	Nuclear* getNuclear();
	void recycleNuclear(Nuclear* nuclear);

	void clearAll();
private:
	static BulletPool* _instance;
	cocos2d::Vector<Bullet*> _bulletVector;
	cocos2d::Vector<cocos2d::Sprite*> _spriteVector;
	cocos2d::Vector<Grenade*> _grenadeVector;
	cocos2d::Vector<Rocket*> _rocketVector;
	cocos2d::Vector<Nuclear*> _nuclearVector;
};

#endif