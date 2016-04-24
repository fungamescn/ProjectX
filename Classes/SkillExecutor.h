#ifndef __SKILLEXECUTOR_H__
#define __SKILLEXECUTOR_H__

#include "cocos2d.h"
#include "Base.h"

class SkillExecutor : public cocos2d::Node
{
public:
	SkillExecutor() = default;
	~SkillExecutor() = default;
	CREATE_FUNC(SkillExecutor);
	virtual bool init() override;
	static SkillExecutor* getInstance();
	void executeSkill(Base* conjurer, const int &id, const cocos2d::Value &param);
	void clearAll();
private:
	static SkillExecutor* _instance;

	std::function<void(Base* conjurer, const int &id, const cocos2d::Value& value)> _beforeFuncs[10];
	std::function<void(Base* conjurer, const int &id, const cocos2d::Value& value)> _processFuncs[10];
	std::function<void(Base* conjurer, const int &id, const cocos2d::Value& value)> _afterFuncs[10];
	//英雄手雷
	void processFunc_0(Base* conjurer, const int &id, const cocos2d::Value& value);
	//怪物手雷
	void processFunc_1(Base* conjurer, const int &id, const cocos2d::Value& value);
	//空袭
	void beforeFunc_2(Base* conjurer, const int &id, const cocos2d::Value& value);
	void processFunc_2(Base* conjurer, const int &id, const cocos2d::Value& value);
	//核弹
	void processFunc_3(Base* conjurer, const int &id, const cocos2d::Value& value);
	//机枪塔
	static const int GATLIN_MAX = 3;
	std::deque<Base*> _gatlinQueue;
	void processFunc_4(Base* conjurer, const int &id, const cocos2d::Value& value);
	//坦克
	void beforeFunc_5(Base* conjurer, const int &id, const cocos2d::Value& value);
	void processFunc_5(Base* conjurer, const int &id, const cocos2d::Value& value);
	//自爆
	void processFunc_6(Base* conjurer, const int &id, const cocos2d::Value& value);
};

#endif