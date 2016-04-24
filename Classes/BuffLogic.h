#ifndef __BUFFLOGIC_H__
#define __BUFFLOGIC_H__

#include "cocos2d.h"
#include "Base.h"

struct Buff 
{
	int id;
	bool aging;
	int propType;
	float timeVal;
	int addType;
	float addNum;
};

enum
{
	ADD_TYPE_DIRECT,
	ADD_TYPE_PERCENT
};

class BuffLogic
{
public:
	static BuffLogic* getInstance();
	BuffLogic();
	~BuffLogic();
	void scheduleBuffs(bool start);
	void addBuffHero(const unsigned int &id);
	void clearAll();
	//first->direct, second->percent
	std::pair<float, float> getBuffByPair(const int &baseType, const int &propType);
private:
	static BuffLogic* _instance;
	cocos2d::Scheduler* _scheduler;
	std::vector<Buff*> _heroBuffs;
	std::vector<Buff*> _monsterBuffs;
	void update(float delta);
};

#endif