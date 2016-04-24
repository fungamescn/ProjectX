#include "AIManager.h"
#include "BattleLayer.h"

AIManager::AIManager(){}

AIManager::~AIManager(){}

void AIManager::exeAIOrders(float time)
{
	auto battleLayer = BattleLayer::getInstance();
	if ( !battleLayer ) return;
	int typeArr[] = { GROUP_TYPE_HERO, GROUP_TYPE_EVIL, GROUP_TYPE_JUST };
	for (auto groupType : typeArr)
	{
		std::vector<Base *> baseVec = battleLayer->getBaseVectorByGroupType(groupType);
		for (std::vector<Base *>::iterator iter = baseVec.begin(); iter != baseVec.end(); ++iter)
		{
			Base * it = *iter;
			if (it->getBaseType() == BASE_TYPE_HERO || it->getBaseType() == BASE_TYPE_MONSTER)
				//CCLOG("CurrentHp：%f", it->getCurrentHp());
				it->doAIAction(time);

		}
	}
}