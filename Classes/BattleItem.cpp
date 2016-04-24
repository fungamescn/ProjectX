#include "BattleItem.h"
#include "Template.h"
#include "BattleLayer.h"
#include "Utils.h"
#include "RandomLib.h"
#include "BuffLogic.h"
#include "GameData.h"
#include "PlatFormInterface.h"

bool BattleItem::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	this->scheduleUpdate();

	_shadow = Sprite::create();
	this->addChild(_shadow);
	_shadow->initWithSpriteFrameName("monster_1001_3.png");

	_body = Sprite::create();
	this->addChild(_body);

	return true;
}

void BattleItem::update(float delta)
{
	if (!this->getParent())
	{
		return;
	}

	else if (durTime <= 0)
	{
		if (durTime > -1)
		{
			this->dead();
			return;
		}
	}
	else if (durTime < 2)
	{
		int flashRate = BATTLE_ITEM_FLASH_RATE / CCDirector::getInstance()->getScheduler()->getTimeScale();
		if (_flashNum % flashRate == 0)
		{
			this->setVisible(!this->isVisible());
		}
		_flashNum++;
	}

	durTime -= delta*CCDirector::getInstance()->getScheduler()->getTimeScale();

	Base * hero = nullptr;
	auto heroVec = BattleLayer::getInstance()->heroVec;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		if (heroVec[i]->getCurrentHp() <= 0)
		{
			continue;
		}
		else if (hero == nullptr)
		{
			hero = heroVec[i];
		}
		
		auto distance = Utils::getDistance(this->getPosition(), heroVec[i]->getPosition());
		if (distance <= radius)
		{
			this->doEffect();
			this->dead();
			return;
		}
	}

	//自动吸金
	if (effect == BATTLEITEM_EFFECT_PLAN_AWARD && GameData::getInstance()->getGameDataForKey(GD_SUCTION_GOLD) == "1")
	{
		if (hero == nullptr) return;
		auto dx = hero->getShadowPoint().x - this->getPosition().x;
		auto dy = hero->getShadowPoint().y - this->getPosition().y;

		auto dxx = dx * dx;
		auto dyy = dy * dy;
		
		auto speed = BATTLE_ITEM_MOVE_SPEED * CCDirector::getInstance()->getScheduler()->getTimeScale();
		if (dxx + dyy < speed * speed)
		{
			this->setPosition(hero->getShadowPoint());
			return;
		}

		auto mx = sqrt(dx * dx * 10 * 10 / ( dx * dx + dy * dy));
		auto my = sqrt(dy * dy * 10 * 10 / ( dx * dx + dy * dy));

		this->setPositionX(this->getPositionX() + (dx > 0 ? mx : -mx));
		this->setPositionY(this->getPositionY() + (dy > 0 ? my : -my));
		
	}
}

void BattleItem::setDataById(int tid)
{
	_tid = tid;
	META_LINE meta = Template::getInstance()->getMetaById(TEMPLATE_BATTLE_ITEM, tid);

	radius = Utils::string2Int(meta["radius"]);
	effect = Utils::string2Int(meta["effect"]);
	params1 = Utils::string2Int(meta["params1"]);
	params2 = Utils::string2Int(meta["params2"]);
	durTime = Utils::string2Float(meta["time"]);

	
	_body->initWithSpriteFrameName(meta["res"]);
	_body->setPositionY(Utils::string2Float(meta["shadowY"]));
	auto action = MoveBy::create(0.3, Vec2(0, 10));
	_body->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));

	if(meta["arrowRes"]!="")
	{
		_tipPart = Sprite::create();
		this->addChild(_tipPart);

		_tipPart->initWithSpriteFrameName(meta["arrowRes"]);
		_tipPart->setPositionY(Utils::string2Float(meta["arrowPosY"]));
		auto action = MoveBy::create(GUIDE_ARROW_TIME, Vec2(0, GUIDE_ARROW_DISTANCE));
		_tipPart->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}

#if DEBUG_MODE
	_drawNode = DrawNode::create();
	_drawNode->clear();
	_drawNode->drawCircle(Point::ZERO, radius, 0, 360, false, Color4F(0, 0, 1, 1));
//	Vec2 p0 = Vec2(0, 0);
//	Vec2 p1 = Vec2(0, 50);
//	Vec2 p2 = Vec2(50, 50);
//	Vec2 p3 = Vec2(50, 0);
//	Vec2 points[] = { p0, p1, p2, p3 };
//	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));

	this->addChild(_drawNode);
#endif

}

int BattleItem::getTemplateId()
{ 
	return _tid; 
}

void BattleItem::dead()
{
	this->unscheduleUpdate();
	BattleLayer::getInstance()->removeBattleItem(this);
}



void BattleItem::doEffect()
{
	switch (effect)
	{
	case BATTLEITEM_EFFECT_PLAN_AWARD:
		planAward();
		break;
	case BATTLEITEM_EFFECT_ADD_BUFF:
		BuffLogic::getInstance()->addBuffHero(params1);
		break;
	case BATTLEITEM_EFFECT_ADD_HP:
		eachAddHp();
		break;
	case BATTLEITEM_EFFECT_ADD_SHIELD:
		eachAddShield();
		break;
	case BATTLEITEM_EFFECT_GIFT:
		showGift();
		break;
	case BATTLEITEM_EFFECT_ADD_HERO:
		addHero();
		break;
	default:
		break;
	}
}

void BattleItem::planAward()
{
	RandomLib::awardByPlanId(params1);
}

void BattleItem::eachAddHp()
{
	auto heroVec = BattleLayer::getInstance()->heroVec;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		if (heroVec[i] && heroVec[i]->getCurrentHp() > 0)
		{
			heroVec[i]->setCurrentHp(heroVec[i]->getCurrentHp() + params1);
		}
	}
}

void BattleItem::eachAddShield()
{
	auto heroVec = BattleLayer::getInstance()->heroVec;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		if (heroVec[i] && heroVec[i]->getCurrentHp() > 0)
		{
			heroVec[i]->setShield(params1);
		}
	}
}

void BattleItem::showGift()
{
	PlatFormInterface::getInstance()->showPayLayout( params1 - 1 );
}

void BattleItem::addHero()
{
	auto player = Hero::create(params1);
	auto hero = BattleLayer::getInstance()->heroVec[0];
	auto pos = hero->getPosition() + Vec2(0, 150);
	player->setRolePosition(pos);
	player->setDirection(1);
	player->stand();
	BattleLayer::getInstance()->addBase(player);
}

//天赋
bool propertyLimit(int propertyId)
{
	auto propertyStr = GameData::getInstance()->getGameDataForKey(propertyId, GD_PROPERTY);
	auto propertyLvl = propertyStr.empty() ? 0 : Utils::string2Int(propertyStr);
	if (propertyLvl >= PROPERTY_MAX_LVL)
	{
		return false;
	}
	return true;
}

//解锁
bool teamUnlockLimit(int index)
{
	auto unLockStr = GameData::getInstance()->getGameDataForKey(index, GD_TEAM_LOCK);
	return unLockStr.empty();
}

//等级
bool levelLimit(int heroId)
{
	auto levelStr = GameData::getInstance()->getGameDataForKey(heroId, GD_TEAM_LEVEL);
	auto levelValue = levelStr.empty() ? 0 : Utils::string2Int(levelStr);
	if (levelValue >= MAX_LEVEL)
	{
		return false;
	}
	return true;
}

bool isGuideCompleted(int guideId)
{
	return GameData::getInstance()->getGameDataForKey(guideId, GD_GUIDE) == "1";
}

bool BattleItem::canDrop(int tid)
{
	META_LINE meta = Template::getInstance()->getMetaById(TEMPLATE_BATTLE_ITEM, tid);
	if (meta["dropless"] == "")
	{
		return true;
	}
	auto dropless = Utils::string2Int(meta["dropless"]);
	switch (dropless)
	{
	case 1:
	case 2:
	case 3:
	case 4:
		return propertyLimit(dropless);
	case 5:
	case 6:
	case 7:
	case 8:
		return propertyLimit(dropless - 4);
	case 9:
	case 10:
	case 11:
	case 12:
		return propertyLimit(1000 + dropless - 8);
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
		return !isGuideCompleted(dropless + 77);
	default:
		break;
	}
	return true;
}

