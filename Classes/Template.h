#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include "cocos2d.h"

typedef std::unordered_map<std::string, std::string> META_LINE;
typedef std::unordered_map<int, META_LINE> META_TABLE;

enum TEMPLATE_INDEX
{
	TEMPLATE_AI = 1,
	TEMPLATE_BATTLE_EVENT,
	TEMPLATE_BATTLE_ITEM,
	TEMPLATE_BUFF,
	TEMPLATE_BULLET,
	TEMPLATE_ELEMENT_MODEL,
	TEMPLATE_GUIDE,
	TEMPLATE_HERO,
	TEMPLATE_ITEM,
	TEMPLATE_LEVEL,
	TEMPLATE_MONSTER,
	TEMPLATE_PLAN,
	TEMPLATE_PLAN_AWARD,
	TEMPLATE_PROPERTY,
	TEMPLATE_RANDOM_POINTS,
	TEMPLATE_SCENE_ELEMENT,
	TEMPLATE_SHOP,
	TEMPLATE_SKILL,
	TEMPLATE_SKILL_LEVEL,
	TEMPLATE_STAGE,
	TEMPLATE_STAGE_TILES
};

class Template
{
public:
	Template();
	~Template();
	static Template* getInstance();
	void loadMeta();
	const META_LINE& getMetaById(const int &index, const int &id);
	const META_LINE& getMetaByKeys(const int &index, const META_LINE &keys);
	std::vector<META_LINE> getMetaVectorByKeys(const int &index, const META_LINE &keys);
	const META_TABLE& getMetaTable(const int &index);
private:
	static const std::unordered_map<int, std::string> FILE_DICT;
	static Template* _template;
	std::unordered_map<int, META_TABLE> _tableDict;
};

#endif