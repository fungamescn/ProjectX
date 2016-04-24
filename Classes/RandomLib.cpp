#include "RandomLib.h"
#include "Template.h"
#include "Base.h"
#include "Utils.h"
#include "cocos2d.h"
#include "BattleLayer.h"
#include "GameData.h"
#include "StageLayer.h"
USING_NS_CC;

int RandomLib::getAwardId(int planId)
{
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_PLAN, planId);
	if (meta["weight"] != "")
	{
		auto weights = Utils::str_split_f(meta["weight"], "|");
		auto awards = Utils::str_split(meta["award"], "|");
		if (weights.size() != awards.size())
		{
			CCLOG("plan template error");
			return 0;
		}

		int total = 0;
		for (size_t i = 0; i < weights.size(); i++)
		{
			total += weights[i];
		}
		int value = random(0, total);
		
		total = 0;
		for (size_t j = 0; j < weights.size(); j++)
		{
			total += weights[j];
			if (value < total)
			{
				return Utils::string2Int(awards[j]);
			}
		}
	}
	else
	{
		float probablility = Utils::string2Float(meta["probablility"]);
		if (CCRANDOM_0_1() <= probablility)
		{
			return Utils::string2Int(meta["award"]);
		}
	}
	return 0;
}

void RandomLib::award(int awardId, Base* base)
{
	if (awardId == 0)
	{
		return;
	}
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_PLAN_AWARD, awardId);
	if (meta["battleItem"] != "")
	{
		BattleLayer::getInstance()->createBattleItem(Utils::string2Int(meta["battleItem"]), base->getShadowPoint(), false);
	}
	
	if (meta["gold"] != "")
	{
//		addGold
		GameData::getInstance()->addMoney(Utils::string2Int(meta["gold"]));
		BattleLayer::getInstance()->upMoneySprite(Utils::string2Int(meta["gold"]));
	}

	if (meta["diamond"] != "")
	{
//		addDiamond 
//		to be open later
	}

	if (meta["itemId"] != ""&&meta["itemNum"] != "")
	{
		//		addItem
		auto itemId = Utils::string2Int(meta["itemId"]);
		auto itemNum = Utils::string2Int(meta["itemNum"]);
//		GameData::getInstance()->addItemById(itemId, itemNum);
		BattleLayer::getInstance()->addSkillItemById(itemId, itemNum);
		StageLayer::getInstance()->updateSkillItem();

	}

}

void RandomLib::awardByPlanId(int planId, Base* base)
{
	auto awardId = RandomLib::getAwardId(planId);
	RandomLib::award(awardId, base);
}