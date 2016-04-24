#include "Hero.h"
#include "BattleLayer.h"
#include "Utils.h"
#include "HpBarHero.h"
#include "Element.h"
#include "StageLayer.h"
#include "GameData.h"
#include "PlatFormInterface.h"

USING_NS_CC;

Hero::Hero(const int &roleId) : Role(roleId)
{
	
}

Hero* Hero::create(const int &roleId)
{
	Hero *hero = new(std::nothrow) Hero(roleId);
	CCASSERT(hero, StringUtils::format("create hero fialed %d", roleId).c_str());
	hero->setGroupType(GROUP_TYPE_HERO);
	hero->init();
	hero->autorelease();
	return hero;
}

bool Hero::init()
{
	//类相关性变量初始化
	this->totalDirs = 16;
	this->roleType = "hero";
	if (!Role::init())
	{
		return false;
	}
	auto attribute = this->getAttribute();
	auto offsetY = attribute->getOffsetY();
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("button_revive.png");
	auto closeItem = MenuItemImage::create();
	closeItem->setNormalSpriteFrame(frame);
	closeItem->setSelectedSpriteFrame(frame);
	closeItem->setCallback(CC_CALLBACK_1(Hero::reviveHandler, this));
	_reviveMenu = Menu::create(closeItem, NULL);
	_reviveMenu->setPosition(Vec2(0, offsetY+50));
	_reviveMenu->setVisible(false);
	this->addChild(_reviveMenu);
	moneySprite = Sprite::createWithSpriteFrameName("item_money.png");
	moneySprite->setPosition(Vec2(-20, offsetY + 80));
	auto label1 = Label::createWithSystemFont("+1", "Arial", 30);
	moneySprite->addChild(label1, 1,  10);
	label1->setPosition(Vec2(85, 30));
	this->addChild(moneySprite);
	moneySprite->setVisible(false);
	return true;
}

void Hero::refreshProp(int roleId)
{
	int oldCHp = getAttribute()->getCurrentHp();
	Role::refreshProp(roleId);
	if (oldCHp <= 0)  BattleLayer::getInstance()->callBackPlayer(this);
}

void Hero::upMoneySprite(int num)
{
	auto attribute = this->getAttribute();
	auto offsetY = attribute->getOffsetY();
	moneySprite->setPosition(Vec2(-20, offsetY + 80));
	auto label1 = static_cast<Label *>(moneySprite->getChildByTag(10));
	label1->setString("+"+Utils::int2String(num));
	moneySprite->setVisible(true);
	auto endCall = CallFunc::create(CC_CALLBACK_0(Hero::setMoneySpriteVisible, this));
	moneySprite->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(-20, offsetY + 150)), endCall, NULL));
}

void Hero::setMoneySpriteVisible()
{
	moneySprite->setVisible(false);
}

void Hero::setAIAttTarget()
{
	Base * forceRole = BattleLayer::getInstance()->getForceRole();
	if (forceRole == NULL || forceRole->getCurrentHp() <= 0)
	{
		//无锁定目标，更新锁定目标
		Base * nextRole = getTargetAI();
		this->setOldRole(nextRole);
	}
	else{
		//集火目标 
		if (forceRole->getShadowPoint().getDistanceSq(this->getShadowPoint())
			<= this->getAttribute()->getShotDistance()* this->getAttribute()->getShotDistance())
		{
			//射程内更新锁定目标
			this->findPathList.clear();
			//清楚移动路径
			this->setOldRole(forceRole);
		}
		else if (this->findPathList.size() <= 0)
		{
			Point pt = forceRole->getPosition();
			if (forceRole->getBaseType() == BASE_TYPE_ELEMENT)
			{
				std::vector<Vec2> movePoint = static_cast<Element *>(forceRole)->movePoint;
				if (movePoint.size() > 0)
				{
					pt = BattleLayer::getInstance()->getNearVec2(forceRole->getShadowPoint(), this->getPosition(), movePoint);
				}
			}
			this->doFindPath(pt);
		}
	}
}


Base * Hero::getTargetAI()
{
	Base * oldRole = this->getOldRole();
	if (oldRole != NULL &&  oldRole->getCurrentHp() > 0)
	{
		//有锁定目标
		if (oldRole->getShadowPoint().getDistanceSq(this->getShadowPoint())
			<= this->getAttribute()->getShotDistance()* this->getAttribute()->getShotDistance()){
			return oldRole;
		}
	}
	Base * enemyBase = getRecentEnemyByGroupType(GROUP_TYPE_EVIL, this->getAttribute()->getShotDistance(), false);
	return enemyBase;
}

int Hero::calRoleState()
{
	//英雄特殊处理当强制目标不是锁定目标的情况下，执行寻路至强制目标处攻击
	Base * forceRole = BattleLayer::getInstance()->getForceRole();
	if (forceRole != NULL || (forceRole && forceRole->getCurrentHp() > 0))
	{
		if (forceRole->getShadowPoint().getDistanceSq(this->getShadowPoint())
			> this->getAttribute()->getShotDistance()* this->getAttribute()->getShotDistance()){
			return ROLE_STATE_MOVE;
		}
	}
	Base * monster = getOldRole();
	bool hasPath = false;
	if (this->findPathList.size() > 0)
	{
		hasPath = true;
	}
	if (monster == NULL)
	{
		if (hasPath) return ROLE_STATE_MOVE;
		else return ROLE_STATE_STAND;
	}
	else{
		if (hasPath) return ROLE_STATE_MOVE_ATTACK;
		else return ROLE_STATE_ATTAK;
	}
}

int Hero::getToPositiontDir(const cocos2d::Vec2& dest, bool isAttack /* = false */)
{
	auto minVec = dest - this->getPosition();
	if (isAttack)
	{
		auto size = this->getAttribute()->getRoleObb();
		minVec.y -= size.height / 2;
	}
	return Utils::getDirHex(minVec);
}

int Hero::getRestrict(int matertype)
{
	if (matertype == MATER_TYPE_LIVE)
	{
		return getAttribute()->getRestrictLive();
	}
	else if (matertype == MATER_TYPE_BUILD)
	{
		return getAttribute()->getRestrictBuild();
	}
	else if (matertype == MATER_TYPE_MACHINE)
	{
		return getAttribute()->getRestrictMachine();
	}
	return 0;
}

void Hero::initHpBar()
{
	hpBar = HpBarHero::create();
	this->addChild(hpBar, -1);
}

void Hero::dead()
{
	Role::dead();
//	_reviveMenu->setVisible(true);
}

void Hero::reviveHandler(Ref* pSender /* = nullptr */)
{
	if (INVALID_FLAG == BattleLayer::getInstance()->minusSkillItem(SKILL_ITEM_HEAL, 1))
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_16);
		return;
	}
	_reviveMenu->setVisible(false);
	if (this->getCurrentHp() > 0)
	{
		this->setCurrentHp(this->getFinalHp());
	}
	else
	{
		this->setCurrentHp(this->getFinalHp());
		this->scheduleUpdate();
		BattleLayer::getInstance()->callBackPlayer(this);
	}
	StageLayer::getInstance()->updateSkillItem();
}

void Hero::setGroupType(const int &grouptype)
{
	group_type = grouptype;
}

void Hero::setCurrentHp(const float &hp)
{
	Role::setCurrentHp(hp);
	StageLayer::getInstance()->updateHeal(this);
}

void Hero::showReviveMenu()
{
	_reviveMenu->setVisible(true);
}

void Hero::leave()
{
	std::vector<Base *> &baseVec = BattleLayer::getInstance()->getBaseVectorByGroupType(this->getGroupType());
	for (std::vector<Base*>::iterator it = baseVec.begin(); it != baseVec.end(); ++it)
	{
		if (*it == this)
		{
			baseVec.erase(it);
			break;
		}
	}
}