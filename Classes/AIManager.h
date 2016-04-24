#ifndef __AI_MANAGER_H__
#define __AI_MANAGER_H__

#include "Base.h"

class AIManager
{
public:
	AIManager();
	~AIManager();
	//执行AI指令集合
	void exeAIOrders(float time);
};

#endif