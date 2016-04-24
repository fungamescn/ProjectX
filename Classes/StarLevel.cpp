#include "StarLevel.h"
#include "Template.h"
#include "Utils.h"
#include "GameData.h"
#include "PlatFormInterface.h"

StarLevel * StarLevel::_instance = nullptr;

const std::map<int, int> StarLevel::COMPARE =
{
	{ STAR_TYPE_KILL_MONSTER ,1},
	{ STAR_TYPE_USE_TIME, -1 },
	{ STAR_TYPE_NO_DIE, -1 },
	{ STAR_TYPE_KILL_ELEMENT, 1 }
};

StarLevel * StarLevel::getInstance()
{
	if (!_instance)
	{
		_instance = new StarLevel();
	}
	return _instance;
}

void StarLevel::init(int stage)
{
	_stageId = stage;

	_require.clear();
	_reached.clear();

	auto meta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, stage);
	for (size_t i = 1; i <= 3; i++)
	{
		auto index = Utils::int2String(i);
		auto starType = Utils::string2Int(meta["starEvent" + index]);
		auto var1 = meta["var" + index + "1"];
		auto var2 = meta["var" + index + "2"];

		switch (starType)
		{
		case STAR_TYPE_KILL_MONSTER:
		{
			auto mstIds = Utils::str_split_f(var1, "|");
			auto mstNums = Utils::str_split_f(var2, "|");
			if (mstIds.size() != mstNums.size())
			{
				CCLOG("template error:stage[%d]", stage);
				return;
			}
			for (size_t j = 0; j < mstIds.size(); j++)
			{
				int mstId = mstIds[j];
				int mstNum = mstNums[j];
				_require[starType][mstId] = mstNum;
			}
		}
			break;
		case STAR_TYPE_USE_TIME:
		{
			auto sec = Utils::string2Int(var1);
			_require[starType][0] = sec;
		}
			break;
		case STAR_TYPE_NO_DIE:
			_require[starType][0] = 0;
			break;
		case STAR_TYPE_KILL_ELEMENT:
		{
			auto elmIds = Utils::str_split_f(var1, "|");
			auto elmNums = Utils::str_split_f(var2, "|");
			if (elmIds.size() != elmNums.size())
			{
				CCLOG("template error:stage[%d]", stage);
				return;
			}
			for (size_t j = 0; j < elmIds.size(); j++)
			{
				int elmId = elmIds[j];
				int elmNum = elmNums[j];
				_require[starType][elmId] = elmNum;
			}
		}
			break;
		default:
			break;
		}
		
	}

}

void StarLevel::trigger(int type, int key, int value)
{
	if (_require[type].empty())
	{
		return;
	}

	if (_require[type][key] == 0 && COMPARE.at(type) > 0)
	{
		return;
	}

	if (value)
	{
		_reached[type][key] = value;
	}
	else
	{
		_reached[type][key]++;
	}
}

std::vector<int> StarLevel::getCurrnetLevels()
{
	std::vector<int> levels;
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);

	for (size_t i = 1; i <= 3; i++)
	{
		auto index = Utils::int2String(i);
		auto starType = Utils::string2Int(meta["starEvent" + index]);

		if (_require[starType].empty())
		{
			levels.push_back(1);
			continue;
		}

//		if (_reached[starType].empty())
//		{
//			_reached[starType][0] = 0;
//		}

		for (STAR_REQ_ITEM::iterator it = _require[starType].begin(); it != _require[starType].end(); ++it)
		{
			auto key = it->first;
			auto value = it->second;

			auto value1 = _reached[starType][key];
			auto value2 = _require[starType][key];

			if ((value1 - value2) * COMPARE.at(starType) < 0)
			{
				levels.push_back(0);
				break;
			}
		}

		if (levels.size() < i)
		{
			levels.push_back(1);
		}
	}
	return levels;
}

int StarLevel::getLevel(int stage)
{
	std::string starLevel = GameData::getInstance()->getGameDataForKey(stage, GD_STAGE_STAR);
	return Utils::string2Int(starLevel);
}

//0-3分别是0-3星
void StarLevel::save()
{
	//通关奖励
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
	auto itemId = Utils::string2Int(meta["award"]);
	auto itemNum = Utils::string2Int(meta["awardNum"]);
	GameData::getInstance()->addItemById(itemId, itemNum);

	auto starLevelStr = GameData::getInstance()->getGameDataForKey(_stageId, GD_STAGE_STAR);
	if (starLevelStr == "3")
	{
		return;
	}
	auto starLevel = Utils::string2Int(starLevelStr);
	auto curLevels = getCurrnetLevels();
	auto curLevel = curLevels[0] + curLevels[1] + curLevels[2];

	if (curLevel <= starLevel)
	{
		return;
	}

	//星级奖励
	for (size_t i = starLevel + 1; i <= curLevel; i++)
	{
		auto index = Utils::int2String(i);
		auto itemId = Utils::string2Int(meta["starAward" + index]);
		auto itemNum = Utils::string2Int(meta["num" + index]);
		GameData::getInstance()->addItemById(itemId, itemNum);
	}


	GameData::getInstance()->setGameDataForKey(_stageId, Utils::int2String(curLevel), GD_STAGE_STAR);
	PlatFormInterface::getInstance()->record("关卡", "星级", _stageId, curLevel);
}