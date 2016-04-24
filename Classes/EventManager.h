#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "cocos2d.h"
#include "Base.h"

using namespace std;

class EventManager
{
public:
	static const string EVENT_TYPE_TEST;
	static const string EVENT_TYPE_GAME_START;
	static const string EVENT_TYPE_BASE_POSITION_CHANGE;
	static const string EVENT_TYPE_BASE_DEAD;
	static const string EVENT_TYPE_MONSTER_COUNT_CHANGE;
	static const string EVENT_TYPE_PRE_EVENT_COMPLETE;
	static const string EVENT_TYPE_ITEM_DEAD;

	static char * CONDITION_TYPE_TEST;
	static char * CONDITION_TYPE_GAME_START;
	static char * CONDITION_TYPE_SOME_MONSTER_DIE;
	static char * CONDITION_TYPE_SOME_MONSTER_LEFT;
	static char * CONDITION_TYPE_ENY_MONSTER_DIE;
	static char * CONDITION_TYPE_ENY_MONSTER_LEFT;
	static char * CONDITION_TYPE_SOME_ELEMENT_DIE;
	static char * CONDITION_TYPE_HERO_ENTER_AREA;
	static char * CONDITION_TYPE_SOME_MONTER_ENTER_AREA;
	//static char * CONDITION_TYPE_ENY_MONTER_ENTER_AREA;
	static char * CONDITION_TYPE_SOME_ELEMENT_LEFT;
	static char * CONDITION_TYPE_SOME_ITEM_DIE;
	
	static const string ACTION_TYPE_POINTS_CREATE_MONSTER;
	static const string ACTION_TYPE_RANDOM_CREATE_MONSTER;
	static const string ACTION_TYPE_SOME_ELEMENT_DIE;
	static const string ACTION_TYPE_POINTS_CREATE_ELEMENT;
	static const string ACTION_TYPE_RANDOM_CREATE_ELEMENT;
	static const string ACTION_TYPE_RESULT_WIN;
	static const string ACTION_TYPE_RESULT_LOSE;
	static const string ACTION_TYPE_STOP_EVENT;
	static const string ACTION_TYPE_GUIDE;
	static const string ACTION_TYPE_ALERT;
	static const string ACTION_TYPE_REMOVE_PLAYER;
	static const string ACTION_TYPE_GIFT;

	EventManager();
	~EventManager();
	void trigger(string eventType,...);
	void initStageData(int level);
	void clearLevelData();
	void areaChecking();

	
private:
	void readTemplate(int level);
	void onTestEvent();
	void onGameStartEvent();
	void onBasePositionChangeEvent(Base * base);
	void onBaseDeadEvent(Base * base);
	void onPreEventCompleteEvent(string  nextEventId);
	void onItemDeadEvent(int itemId);
	
	void typeHandler(const char* subType, ...);
	
	void doAction(string tid);
	bool isPreEventComplete(string eventId);
	bool isConditionMet(string tid, va_list &args);

	map<string, int> _effectCounts;//<tid,count>可生效次数
	map<string, vector<string>> eventMap;//<subType,subTypeVec<tid>>
	map<string, int> _triggerCount;//触发次数
	map<string, int> _completeCount;//完成次数
	list<Base*> _posChangeBases;
};
#endif