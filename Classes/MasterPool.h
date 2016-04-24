#ifndef __MASTERPOOL_H__
#define __MASTERPOOL_H__
//回收对象池

#include "cocos2d.h"
#include "Role.h"
#include "Element.h"
#include <list>
#include <queue>

struct MonsterCreateData
{
	int tid;
	int gType;
	float pointX;
	float pointY;
	bool isTiledPos;
};

class MasterPool
{
public:
	static MasterPool* getInstance();
	
	void clean();
	void releaseDeadMonsterPtrArray();
	void releaseDeadElementPtrArray();
	void addMonsterToCreateQuque(int tid, int groupType, cocos2d::Point &point, bool isTiledPos);
	Role * getMonster();
	MonsterCreateData getNextCreateMonster();
	int getCreateMonsterLen();

public:
	std::list<Role *> deadMonsterArray;//场上死了的怪物集合（只有monster，不包括hero）
	std::vector<Role *> deadMonsterPtrArray;//被移除的Monster统一存放于此，避免出现野指针
	std::vector<Element *> deadElementPtrArray;//被移除的Element统一存放于此，避免出现野指针

private:
	static MasterPool* _instance;
	std::queue<MonsterCreateData> createMonsterQueue;
	int count = 0;

private:
	MasterPool() = default;
	~MasterPool();
	MasterPool(const MasterPool&);
	MasterPool& operator = (const MasterPool&);
};

#endif