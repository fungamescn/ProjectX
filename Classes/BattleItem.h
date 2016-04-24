#ifndef __BATTLE_ITEM_H__
#define __BATTLE_ITEM_H__

#include "cocos2d.h"
#include "Config.h"

USING_NS_CC;
using namespace std;

enum BATTLEITEM_EFFECT_TYPE
{
	BATTLEITEM_EFFECT_PLAN_AWARD = 1,
	BATTLEITEM_EFFECT_ADD_BUFF,
	BATTLEITEM_EFFECT_ADD_HP,
	BATTLEITEM_EFFECT_ADD_SHIELD,
	BATTLEITEM_EFFECT_GIFT,
	BATTLEITEM_EFFECT_ADD_HERO
};

class BattleItem:public Sprite
{
public:
	BattleItem(){};
	~BattleItem(){};
	CREATE_FUNC(BattleItem);
	bool init();
	void setDataById(int tid);
	int getTemplateId();
	void update(float delta) override;
	void dead();
	void doEffect();
	static bool canDrop(int tid);
private:
	int _tid;
	string res;
	int radius;
	int effect;
	int params1;
	int params2;
	float durTime;

	Sprite* _tipPart;
	Sprite* _shadow;
	Sprite* _body;
	int _flashNum = 0;

	void planAward();
	void eachAddHp();
	void eachAddShield();
	void showGift();
	void addHero();

#if DEBUG_MODE
	DrawNode* _drawNode;
#endif
};



#endif