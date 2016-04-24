#ifndef __HERO_H__
#define __HERO_H__
#include "Role.h"

class Hero : public Role
{
public:
	Hero(const int &roleId);
	static Hero* create(const int &roleId);
	virtual bool init();
	virtual int getBaseType() override{ return BASE_TYPE_HERO; };
	int getRestrict(int matertype);
	virtual void dead() override;
	int getFindPathListNum(){ return findPathList.size(); };
	virtual void setGroupType(const int &grouptype) override;
	virtual void setCurrentHp(const float &hp) override;
	void reviveHandler(Ref* pSender = nullptr);
	void showReviveMenu();
	void upMoneySprite(int num);
	void setMoneySpriteVisible();
	void refreshProp(int roleId);
	void leave();
public:
	cocos2d::Sprite * moneySprite;

protected:
	virtual int calRoleState() override;
	virtual void setAIAttTarget() override;//设置英雄的目标
	virtual Base * getTargetAI() override;//无任何状态下获取目标
	virtual int getToPositiontDir(const cocos2d::Vec2& dest, bool isAttack = false) override;
	virtual void initHpBar() override;
private:
	cocos2d::Menu* _reviveMenu = nullptr;
};
#endif