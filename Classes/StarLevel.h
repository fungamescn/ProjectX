#ifndef __STAR_LEVEL_H__
#define __STAR_LEVEL_H__

#include "cocos2d.h"

enum STAR_TYPE
{
	STAR_TYPE_KILL_MONSTER = 1,
	STAR_TYPE_USE_TIME,
	STAR_TYPE_NO_DIE,
	STAR_TYPE_KILL_ELEMENT
};

typedef std::map<int, int> STAR_REQ_ITEM;
typedef std::map<int, STAR_REQ_ITEM> STAR_REQ;

class StarLevel
{
public:
	StarLevel(){};
	~StarLevel(){};
	static StarLevel * getInstance();
	void init(int stage);
	void trigger(int type, int key = 0, int value = 0);
	int getLevel(int stage);
	std::vector<int> getCurrnetLevels();
	int getStageId(){ return _stageId; };
	void save();
private:
	static const std::map<int, int> COMPARE;
	static StarLevel * _instance;
	
	STAR_REQ _require;
	STAR_REQ _reached;

	int _stageId = 0;
};
#endif
