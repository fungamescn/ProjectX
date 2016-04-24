#include "Template.h"
#include "json/rapidjson.h"
#include "json/document.h"

using namespace rapidjson;

Template* Template::_template = nullptr;
const std::unordered_map<int, std::string> Template::FILE_DICT = {
		{ TEMPLATE_AI, "json/Ai.json" },
		{ TEMPLATE_BATTLE_EVENT, "json/BattleEvent.json" },
		{ TEMPLATE_BATTLE_ITEM , "json/BattleItem.json"},
		{ TEMPLATE_BULLET, "json/Bullet.json" },
		{ TEMPLATE_BUFF , "json/Buff.json" },
		{ TEMPLATE_ELEMENT_MODEL, "json/ElementModel.json" },
		{ TEMPLATE_GUIDE ,"json/Guide.json"},
		{ TEMPLATE_HERO, "json/Hero.json" },
		{ TEMPLATE_ITEM, "json/Item.json" },
		{ TEMPLATE_LEVEL, "json/Level.json" },
		{ TEMPLATE_MONSTER, "json/Monster.json" },
		{ TEMPLATE_PLAN, "json/Plan.json" },
		{ TEMPLATE_PLAN_AWARD, "json/PlanAward.json" },
		{ TEMPLATE_PROPERTY, "json/Property.json" },
		{ TEMPLATE_RANDOM_POINTS, "json/RandomPoints.json" },
		{ TEMPLATE_SCENE_ELEMENT, "json/SceneElement.json" },
		{ TEMPLATE_SHOP, "json/Shop.json" },
		{ TEMPLATE_SKILL, "json/Skill.json" },
		{ TEMPLATE_SKILL_LEVEL, "json/SkillLevel.json" },
		{ TEMPLATE_STAGE, "json/Stage.json" },
		{ TEMPLATE_STAGE_TILES, "json/StageTiles.json" }
};

Template::Template()
{
}

Template::~Template()
{
	_tableDict.clear();
}

Template* Template::getInstance()
{
	if (!_template)
	{
		_template = new Template();
	}
	return _template;
}

bool parse(const std::string path,Document &doc)
{
	ssize_t size = 0;
	unsigned char* titlech = cocos2d::FileUtils::getInstance()->getFileData(path, "r", &size);
	std::string load_str = std::string((const char*)titlech, size);
	doc.Parse<0>(load_str.c_str());
	if (doc.HasParseError())
	{
		//CCLOG("GetParseError%s\n", doc.GetParseError());
		return false;
	}
	return true;
}

void Template::loadMeta()
{
	for (auto fileIter = FILE_DICT.cbegin(); fileIter != FILE_DICT.cend(); ++ fileIter)
	{
		auto fileIndex = fileIter->first;
		auto filePath = fileIter->second;
		Document eachTable;
		if (!parse(filePath, eachTable))
		{
			continue;
		}
		META_TABLE metaTable;
		for (int i = 0; i < eachTable.Capacity(); ++i)
		{
			META_LINE metaLine;
			Value& eachLine = eachTable[(SizeType)i];
			for (Document::MemberIterator iter = eachLine.MemberBegin(); iter != eachLine.MemberEnd(); ++iter)
			{
				auto key = iter->name.GetString();
				auto value = iter->value.GetString();
				metaLine[key] = value;
			}
			if (metaLine.empty())
			{
				continue;
			}
			int id = atoi(eachLine["id"].GetString());
			metaTable[id] = metaLine;
		}
		_tableDict[fileIndex] = metaTable;
	}
}

const META_LINE& Template::getMetaById(const int &index, const int &id)
{
	return _tableDict[index][id];
}

const META_LINE& Template::getMetaByKeys(const int &index, const META_LINE &keys)
{
	int foundId = 0;
	for (const auto &iter : _tableDict[index])
	{
		bool foundMeta = true;
		auto metaLine = iter.second;
		for (const auto &keyIter : keys)
		{
			auto lineIter = metaLine.find(keyIter.first);
			if (lineIter == metaLine.end() || lineIter->second != keyIter.second)
			{
				foundMeta = false;
				break;
			}
		}
		if (foundMeta)
		{
			foundId = atoi(metaLine["id"].c_str());
			break;
		}
	}
	return _tableDict[index][foundId];
}

std::vector<META_LINE> Template::getMetaVectorByKeys(const int &index, const META_LINE &keys)
{
	int foundId = 0;
	std::vector<META_LINE> vec;
	for (const auto &iter : _tableDict[index])
	{
		bool foundMeta = true;
		auto metaLine = iter.second;
		for (const auto &keyIter : keys)
		{
			auto lineIter = metaLine.find(keyIter.first);
			if (lineIter == metaLine.end() || lineIter->second != keyIter.second)
			{
				foundMeta = false;
				break;
			}
		}
		if (foundMeta)
		{
			foundId = atoi(metaLine["id"].c_str());
			vec.push_back(_tableDict[index][foundId]);
		}
	}
	return vec;
}

const META_TABLE& Template::getMetaTable(const int &index)
{
	return _tableDict[index];
}

