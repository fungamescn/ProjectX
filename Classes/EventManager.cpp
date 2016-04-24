#include "EventManager.h"
#include "BattleLayer.h"
#include "Template.h"
#include "MasterPool.h"
#include "Utils.h"
#include "GuideLayer.h"
#include "NoticeLayer.h"
#include "GuideAlertLayer.h"
#include "PlatFormInterface.h"

USING_NS_CC;

//基本事件类型
const string EventManager::EVENT_TYPE_TEST = "event_type_test";
const string EventManager::EVENT_TYPE_GAME_START = "event_type_game_start";
const string EventManager::EVENT_TYPE_BASE_POSITION_CHANGE = "event_type_base_position_change";
const string EventManager::EVENT_TYPE_BASE_DEAD = "event_type_base_dead";
const string EventManager::EVENT_TYPE_MONSTER_COUNT_CHANGE = "event_type_monster_count_change";
const string EventManager::EVENT_TYPE_PRE_EVENT_COMPLETE = "event_type_pre_event_complete";
const string EventManager::EVENT_TYPE_ITEM_DEAD = "event_type_item_dead";

//条件类型
char * EventManager::CONDITION_TYPE_TEST = "0";
char * EventManager::CONDITION_TYPE_GAME_START = "1";
char * EventManager::CONDITION_TYPE_SOME_MONSTER_DIE = "2";
char * EventManager::CONDITION_TYPE_SOME_MONSTER_LEFT = "3";
char * EventManager::CONDITION_TYPE_ENY_MONSTER_DIE = "4";
char * EventManager::CONDITION_TYPE_ENY_MONSTER_LEFT = "5";
char * EventManager::CONDITION_TYPE_SOME_ELEMENT_DIE = "6";
char * EventManager::CONDITION_TYPE_HERO_ENTER_AREA = "7";
char * EventManager::CONDITION_TYPE_SOME_MONTER_ENTER_AREA = "8";
//char * EventManager::CONDITION_TYPE_ENY_MONTER_ENTER_AREA = "9";
char * EventManager::CONDITION_TYPE_SOME_ELEMENT_LEFT = "10";
char * EventManager::CONDITION_TYPE_SOME_ITEM_DIE = "11";

//行为类型
const string EventManager::ACTION_TYPE_POINTS_CREATE_MONSTER = "1";
const string EventManager::ACTION_TYPE_RANDOM_CREATE_MONSTER = "2";
const string EventManager::ACTION_TYPE_SOME_ELEMENT_DIE = "3";
const string EventManager::ACTION_TYPE_POINTS_CREATE_ELEMENT = "4";
const string EventManager::ACTION_TYPE_RANDOM_CREATE_ELEMENT = "5";
const string EventManager::ACTION_TYPE_RESULT_WIN = "6";
const string EventManager::ACTION_TYPE_RESULT_LOSE = "7";
const string EventManager::ACTION_TYPE_STOP_EVENT = "8";
const string EventManager::ACTION_TYPE_GUIDE = "9";
const string EventManager::ACTION_TYPE_ALERT = "10";
const string EventManager::ACTION_TYPE_REMOVE_PLAYER = "11";
const string EventManager::ACTION_TYPE_GIFT = "12";


EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::readTemplate(int level)
{
	string stageId = Utils::int2String(level);
	META_TABLE metaTable = Template::getInstance()->getMetaTable(TEMPLATE_BATTLE_EVENT);
	for (META_TABLE::iterator it = metaTable.begin(); it != metaTable.end(); ++it)
	{
		META_LINE metaLine = it->second;

		if (stageId != metaLine["stageId"])
		{
			continue;
		}

		string subType = metaLine["type"];
		int size = eventMap[subType].size();

		eventMap[subType].push_back(metaLine["id"]);

		_effectCounts[metaLine["id"]] = Utils::string2Int(metaLine["count"]);
	}
}

void EventManager::areaChecking()
{
	if (_posChangeBases.empty())
	{
		return;
	}

	auto base = _posChangeBases.front();
	
	if (base->getBaseType() == BASE_TYPE_HERO)
	{
		typeHandler(EventManager::CONDITION_TYPE_HERO_ENTER_AREA, base);
	}
	else if (base->getBaseType() == BASE_TYPE_MONSTER && (base->getGroupType()&GROUP_TYPE_JUST))
	{
		typeHandler(EventManager::CONDITION_TYPE_SOME_MONTER_ENTER_AREA, base);
	}

	_posChangeBases.pop_front();
}

void EventManager::initStageData(int level)
{
	readTemplate(level);
}

void EventManager::clearLevelData()
{
	_effectCounts.clear();
	map<string, int>(_effectCounts).swap(_effectCounts);

	eventMap.clear();
	map<string, vector<string>>(eventMap).swap(eventMap);

	_triggerCount.clear();
	map<string, int>(_triggerCount).swap(_triggerCount);

	_completeCount.clear();
	map<string, int>(_completeCount).swap(_completeCount);
	
	_posChangeBases.clear();
	list<Base*>(_posChangeBases).swap(_posChangeBases);

}

bool isInRectArea(Base* base ,string areaStr)
{
	Point tileCoor = BattleLayer::getInstance()->tileCoordForPosition(base->getPosition());
	vector<string> pointStrVec = Utils::str_split(areaStr, "|");
	vector<float> p0 = Utils::str_split_f(pointStrVec[0], ",");
	vector<float> p1 = Utils::str_split_f(pointStrVec[1], ",");
	if (tileCoor.x >= p0[0] && tileCoor.x < p1[0] && tileCoor.y >= p0[1] && tileCoor.y < p1[1])
	{
		return true;
	}
	return false;
}

//前置事件是否完成
bool EventManager::isPreEventComplete(string eventId)
{
	META_LINE eventMeta = Template::getInstance()->getMetaById(TEMPLATE_BATTLE_EVENT, Utils::string2Int(eventId));

	if (eventMeta["preEvent"] == "0")
	{
		return true;
	}

	auto preEventIds = Utils::str_split(eventMeta["preEvent"], ",");
	for (size_t i = 0; i < preEventIds.size(); i++)
	{
		if (_completeCount[preEventIds[i]] <= 0)
		{
			return false;
		}
	}
	return true;
}

//条件判断
bool EventManager::isConditionMet(string tid, va_list &args)
{
	int battleEventId = Utils::string2Int(tid);
	META_LINE eventMeta = Template::getInstance()->getMetaById(TEMPLATE_BATTLE_EVENT, battleEventId);

	if (eventMeta["type"] == EventManager::CONDITION_TYPE_TEST || 
		eventMeta["type"] == EventManager::CONDITION_TYPE_GAME_START)
	{
		return true;
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_MONSTER_DIE)
	{
		Base* base = va_arg(args, Base*);
		if (base->getTemplateId() != Utils::string2Int(eventMeta["condPrams1"]) )
		{
			return false;
		}
		_triggerCount[eventMeta["id"]] ++;
		return _triggerCount[eventMeta["id"]] >= Utils::string2Int(eventMeta["condPrams2"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_MONSTER_LEFT)
	{
		int num = BattleLayer::getInstance()->getMonstersByTid(eventMeta["condPrams1"]).size();
		return num <= Utils::string2Int(eventMeta["condPrams2"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_ENY_MONSTER_DIE)
	{
		_triggerCount[eventMeta["id"]] ++;
		return _triggerCount[eventMeta["id"]] >= Utils::string2Int(eventMeta["condPrams1"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_ENY_MONSTER_LEFT)
	{
		int num = BattleLayer::getInstance()->getBasesList().size() - BattleLayer::getInstance()->heroVec.size();
		return num <= Utils::string2Int(eventMeta["condPrams1"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_ELEMENT_DIE)
	{
		Base* base = va_arg(args, Base*);
		if (base->getTemplateId() != Utils::string2Int(eventMeta["condPrams1"]))
		{
			return false;
		}
		_triggerCount[eventMeta["id"]] ++;
		return _triggerCount[eventMeta["id"]] >= Utils::string2Int(eventMeta["condPrams2"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_HERO_ENTER_AREA)
	{
		auto team = BattleLayer::getInstance()->getBaseVectorByGroupType(GROUP_TYPE_HERO);
		for (size_t i = 0; i < team.size(); i++)
		{
			if (isInRectArea(team[i], eventMeta["condPrams1"]))
			{
				return true;
			}
		}
		return false;
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_MONTER_ENTER_AREA)
	{
		vector<Base*> monsters = BattleLayer::getInstance()->getMonstersByTid(eventMeta["condPrams1"]);
		if (monsters.size() == 0)
		{
			return false;
		}

		return isInRectArea(monsters[0], eventMeta["condPrams2"]);
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_ELEMENT_LEFT)
	{
		Element* base = static_cast<Element*>(va_arg(args, Base*));
		if (base->getGroupId() != eventMeta["condPrams1"])
		{
			return false;
		}

		int num = BattleLayer::getInstance()->getElementsByGroupId(eventMeta["condPrams1"]).size();
		
		if (num <= Utils::string2Int(eventMeta["condPrams2"]))
		{
			return true;
		}
		else
		{
			if (eventMeta["conditionTips"] != "")
			{
				auto numStr = Utils::int2String(num);
				auto noticeContent = Utils::str_replace(eventMeta["conditionTips"], "#0", numStr);
				NoticeLayer::getInstance()->show(noticeContent);
			}
			return false;
		}
	}
	else if (eventMeta["type"] == EventManager::CONDITION_TYPE_SOME_ITEM_DIE)
	{
		int itemId = va_arg(args, int);
		return itemId == Utils::string2Int(eventMeta["condPrams1"]);
	}

	return false;
}

vector<Point> getPointsByStr(string posStr)
{
	vector<Point> points;

	vector<string> posStrVec = Utils::str_split(posStr, "|");

	for (size_t i = 0; i < posStrVec.size(); i++)
	{
		vector<float> posVec = Utils::str_split_f(posStrVec[i], ",");
		points.push_back(Point(posVec[0], posVec[1]));
	}
	return points;
}

//行为动作
void EventManager::doAction(string tid)
{
	//StageLayer::getInstance()->showDebugLog("doAction id = " + tid);
	META_LINE eventMeta = Template::getInstance()->getMetaById(TEMPLATE_BATTLE_EVENT, Utils::string2Int(tid));

	if (eventMeta["actionType"] == "0")
	{
		//do nothing
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_POINTS_CREATE_MONSTER)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);
		int count = Utils::string2Int(eventMeta["actionParams2"]);
		vector<Point> points = getPointsByStr(eventMeta["actionParams3"]);
		int groupType = Utils::string2Int(eventMeta["actionParams4"]);
		BattleLayer::getInstance()->createMonsters(tid, count, groupType, points);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_RANDOM_CREATE_MONSTER)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);
		int count = Utils::string2Int(eventMeta["actionParams2"]);
		int randomId = Utils::string2Int(eventMeta["actionParams3"]);
		int groupType = Utils::string2Int(eventMeta["actionParams4"]);
		BattleLayer::getInstance()->createMonsters(tid, count, groupType, randomId);

	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_SOME_ELEMENT_DIE)
	{
		const vector<Base*> &elements = BattleLayer::getInstance()->getElementsByGroupId(eventMeta["actionParams1"]);
		for (size_t i = 0; i < elements.size(); i++)
		{
			elements[i]->dead();
		}
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_POINTS_CREATE_ELEMENT)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);
		int count = Utils::string2Int(eventMeta["actionParams2"]);
		vector<Point> points = getPointsByStr(eventMeta["actionParams3"]);
		int groupType = Utils::string2Int(eventMeta["actionParams4"]);
		BattleLayer::getInstance()->createElements(tid, count, groupType, points);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_RANDOM_CREATE_ELEMENT)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);
		int count = Utils::string2Int(eventMeta["actionParams2"]);
		int randomId = Utils::string2Int(eventMeta["actionParams3"]);
		int groupType = Utils::string2Int(eventMeta["actionParams4"]);
		BattleLayer::getInstance()->createElements(tid, count, groupType, randomId);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_RESULT_WIN)
	{
		BattleLayer::getInstance()->gameOver(1);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_RESULT_LOSE)
	{
		BattleLayer::getInstance()->gameOver(0);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_STOP_EVENT)
	{
		vector<string> eventIdVec = Utils::str_split(eventMeta["actionParams1"], ",");
		for (size_t i = 0; i < eventIdVec.size(); i++)
		{
			_effectCounts[eventIdVec[i]] = 0;
		}
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_GUIDE)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);
		GuideLayer::getInstance()->showById(tid);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_ALERT)
	{
		string title = eventMeta["actionParams1"];
		string content = eventMeta["actionParams2"];
		GuideAlertLayer::getInstance()->show(title, content);
	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_REMOVE_PLAYER)
	{
		int tid = Utils::string2Int(eventMeta["actionParams1"]);

		vector<Base *> &baseVec = BattleLayer::getInstance()->heroVec;
		for (std::vector<Base*>::iterator it = baseVec.begin(); it != baseVec.end();)
		{
			if ((*it)->getTemplateId() == tid)
			{
				BattleLayer::getInstance()->removeBase(*it);
				it = baseVec.erase(it);
			}
			else
			{
				it++;
			}
		}

	}
	else if (eventMeta["actionType"] == EventManager::ACTION_TYPE_GIFT)
	{
		int giftId = Utils::string2Int(eventMeta["actionParams1"]);
		PlatFormInterface::getInstance()->showPayLayout(giftId);
	}

	_effectCounts[tid]--;
	_completeCount[tid]++;

	if (eventMeta["actionTips"] != "")
	{
		NoticeLayer::getInstance()->show(eventMeta["actionTips"]);
	}

	string nextEventIdStr = eventMeta["nextEvent"];

	if (nextEventIdStr == "0")
	{
		return;
	}

	auto nextEventIds = Utils::str_split(nextEventIdStr, ",");
	for (size_t i = 0; i < nextEventIds.size(); i++)
	{
		this->trigger(EventManager::EVENT_TYPE_PRE_EVENT_COMPLETE, Utils::string2Int(nextEventIds[i]));
	}
}

//事件类型处理
void EventManager::typeHandler(const char* subType,...)
{
	vector<string> subTypeVec = eventMap[subType];

	for (size_t i = 0; i < subTypeVec.size(); i++)
	{
		string eventId = subTypeVec.at(i);
		

		//是否有生效次数
		if (_effectCounts[eventId] == 0)
		{
			continue;
		}


		//前置条件是否满足
		if ( !isPreEventComplete(eventId) )
		{
			continue;
		}


		va_list args;
		va_start(args, subType);
		if (!EventManager::isConditionMet(eventId, args))
		{
			va_end(args);
			continue;
		}
		va_end(args);
		
		EventManager::doAction(eventId);
	}
}

void EventManager::trigger(string eventType, ...)
{
	va_list args;
	va_start(args, eventType);
	if (eventType == EventManager::EVENT_TYPE_TEST)
	{
		EventManager::onTestEvent();
	}
	else if (eventType == EventManager::EVENT_TYPE_GAME_START)
	{
		EventManager::onGameStartEvent();
	}
	else if (eventType == EventManager::EVENT_TYPE_BASE_POSITION_CHANGE)
	{
		Base* base = va_arg(args, Base*);
		EventManager::onBasePositionChangeEvent(base);
	}
	else if (eventType == EventManager::EVENT_TYPE_BASE_DEAD)
	{
		Base* base = va_arg(args, Base*);
		EventManager::onBaseDeadEvent(base);
	}
	else if (eventType == EventManager::EVENT_TYPE_PRE_EVENT_COMPLETE)
	{ 
		const int nextEventId = va_arg(args, int);
		//StageLayer::getInstance()->showDebugLog("trigger :" + nextEventId);
		EventManager::onPreEventCompleteEvent(Utils::int2String(nextEventId));
	}
	else if (eventType == EventManager::EVENT_TYPE_ITEM_DEAD)
	{
		int itemId = va_arg(args, int);
		EventManager::onItemDeadEvent(itemId);
	}
	va_end(args);
}

//test function
void EventManager::onTestEvent()
{
	//typeHandler(EventManager::CONDITION_TYPE_TEST, 11, "23", BattleLayer::getInstance()->getBasesList());
}

void EventManager::onGameStartEvent()
{
	typeHandler(EventManager::CONDITION_TYPE_GAME_START);
}

void EventManager::onBasePositionChangeEvent(Base * base)
{
	for (list<Base*>::iterator it = _posChangeBases.begin(); it != _posChangeBases.end(); ++it)
	{
		if ( *it == base)
		{
			return;
		}
	}

	_posChangeBases.push_back(base);
}

void EventManager::onBaseDeadEvent(Base * base)
{
	if (base->getBaseType() == BASE_TYPE_MONSTER)
	{
		typeHandler(EventManager::CONDITION_TYPE_SOME_MONSTER_DIE, base);
		typeHandler(EventManager::CONDITION_TYPE_SOME_MONSTER_LEFT, base);
		typeHandler(EventManager::CONDITION_TYPE_ENY_MONSTER_DIE, base);
		typeHandler(EventManager::CONDITION_TYPE_ENY_MONSTER_LEFT, base);
		MasterPool::getInstance()->releaseDeadMonsterPtrArray();
	}
	else if (base->getBaseType() == BASE_TYPE_ELEMENT)
	{
		typeHandler(EventManager::CONDITION_TYPE_SOME_ELEMENT_DIE, base);
		typeHandler(EventManager::CONDITION_TYPE_SOME_ELEMENT_LEFT, base);
		MasterPool::getInstance()->releaseDeadElementPtrArray();
	}

}

void EventManager::onPreEventCompleteEvent(string  nextEventId)
{
	//是否有生效次数
	if (_effectCounts[nextEventId] == 0)
	{
		return;
	}

	//前置条件是否满足
	if (!isPreEventComplete(nextEventId))
	{
		return;
	}

	va_list args;
	if (!EventManager::isConditionMet(nextEventId, args))
	{
		return;
	}

	EventManager::doAction(nextEventId);
}

void EventManager::onItemDeadEvent(int itemId)
{
	typeHandler(EventManager::CONDITION_TYPE_SOME_ITEM_DIE, itemId);
}