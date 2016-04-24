#ifndef __AI_BASE_H__
#define __AI_BASE_H__

#include "cocos2d.h"
#include "Base.h"

struct AISKILLDATA
{
	int skillId;
	int frameDelay;
	float distance;
};

class RoleAI
{
protected:
	int getAIID(){ return _aiid; };
	std::string getAIFollow(){ return _aiFollow; };
	std::string getAISelect(){ return _aiSelect; };
	void setAIID(int aiid){ _aiid = aiid; };
	void setFollowAI(std::string aifollow){ _aiFollow = aifollow; };
	void setSelectAI(std::string aiselect){ _aiSelect = aiselect; };

	virtual void createAI(int aiid) = 0;
	virtual void setAIAttTarget() = 0;
	virtual Base * getTargetAI() = 0;//获取AI攻击目标
	virtual Base * getFollowAI() = 0;//获取AI跟随目标
	virtual Base * getSkillTargetAI() = 0;

protected:
	std::list<cocos2d::Point> aiFindpath;
	std::list<AISKILLDATA> aiSkillDataList;
	float nextSkillFrame;

private:
	int _aiid = 0;
	std::string _aiFollow;
	std::string _aiSelect;
};

#endif