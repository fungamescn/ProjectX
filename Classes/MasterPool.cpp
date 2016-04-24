#include "MasterPool.h"
#include "BattleLayer.h"

USING_NS_CC;

MasterPool* MasterPool::_instance = nullptr;

MasterPool::~MasterPool()
{
	this->clean();
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}

MasterPool* MasterPool::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new(std::nothrow) MasterPool();
	}
	return _instance;
}

void MasterPool::clean()
{
	while (!createMonsterQueue.empty())
	{
		createMonsterQueue.pop();
	}

	while (!deadMonsterArray.empty())
	{
		deadMonsterArray.pop_back();
	}

	while (!deadMonsterPtrArray.empty())
	{
		deadMonsterPtrArray.back()->release();
		deadMonsterPtrArray.pop_back();
	}
	while (!deadElementPtrArray.empty())
	{
		deadElementPtrArray.back()->release();
		deadElementPtrArray.pop_back();
	}
}

void MasterPool::addMonsterToCreateQuque(int tid, int groupType, cocos2d::Point &point, bool isTiledPos)
{
	MonsterCreateData mdata;
	mdata.tid = tid;
	mdata.gType = groupType;
	mdata.pointX = point.x;
	mdata.pointY = point.y;
	mdata.isTiledPos = isTiledPos;
	createMonsterQueue.push(mdata);
}

int MasterPool::getCreateMonsterLen()
{
	return createMonsterQueue.size();
}

MonsterCreateData MasterPool::getNextCreateMonster()
{
	MonsterCreateData data;
	if (createMonsterQueue.size() > 0)
	{
		data = createMonsterQueue.front();
		createMonsterQueue.pop();
	}
	return data;
}

Role * MasterPool::getMonster()
{
	if (deadMonsterPtrArray.size() > 0)
	{
		for (std::vector<Role *>::iterator iter = deadMonsterPtrArray.begin(); iter != deadMonsterPtrArray.end(); ++iter)
		{
			Role *role = *iter;
			if (role->canRecovey())
			{
				deadMonsterPtrArray.erase(iter);
				BattleLayer::getInstance()->addChild(role);
				role->release();
				count++;
				CCLOG("deadMonsterPtrArray:ceate%d", count);
				return role;
			}
		}
	}
	if (deadMonsterArray.size() > MIN_DEAD_BODY_NUM)
	{
		for (std::list<Role *>::iterator iter = deadMonsterArray.begin(); iter != deadMonsterArray.end(); ++iter)
		{
			Role *role = *iter;
			if (role->canRecovey())
			{
				deadMonsterArray.erase(iter);
				count++;
				CCLOG("deadMonsterArray:ceate%d", count);
				return role;
			}
		}
	}
	return NULL;
}

void MasterPool::releaseDeadMonsterPtrArray()
{
	if (deadMonsterPtrArray.size() > MAX_DEAD_MONSTER_PTR_NUM)
	{
		for (std::vector<Role *>::iterator iter = deadMonsterPtrArray.begin(); iter != deadMonsterPtrArray.end(); iter++)
		{
			Role *role = *iter;
			if (role->canRecovey())
			{
				deadMonsterPtrArray.erase(iter);
				CCLOG("release deadMonsterPtrArray......%d", role->getTemplateId());
				role->release();
				iter--;
			}
		}
	}
}

void MasterPool::releaseDeadElementPtrArray()
{
	if (deadElementPtrArray.size() > 0)
	{
		for (std::vector<Element *>::iterator iter = deadElementPtrArray.begin(); iter != deadElementPtrArray.end(); iter++)
		{
			Element * ele = *iter;
			if (ele->canRecovey())
			{
				deadElementPtrArray.erase(iter);
				CCLOG("release deadElementPtrArray......%d", ele->getTemplateId());
				ele->release();
				iter--;
			}
		}
	}
}