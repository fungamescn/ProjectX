#include "BuffLogic.h"
#include "Template.h"
#include "BattleLayer.h"
#include "StageLayer.h"

USING_NS_CC;

BuffLogic* BuffLogic::_instance = nullptr;

BuffLogic* BuffLogic::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = new (std::nothrow) BuffLogic();
	}
	return _instance;
}

BuffLogic::BuffLogic() : _scheduler(nullptr)
{
	_scheduler = Director::getInstance()->getScheduler();
	_scheduler->retain();
}

void BuffLogic::scheduleBuffs(bool start)
{
	auto delay = float(BATTLE_ITEM_FLASH_RATE) / 60;
	_scheduler->schedule(CC_CALLBACK_1(BuffLogic::update, this), this, delay, !start, "buffLogic");
}

BuffLogic::~BuffLogic()
{
	_scheduler->unschedule("buffLogic", this);
	_scheduler->release();
}

void BuffLogic::addBuffHero(const unsigned int &id)
{
	auto buffMeta = Template::getInstance()->getMetaById(TEMPLATE_BUFF, id);
	if (buffMeta.empty())
	{
		CCLOG("no such buffMeta of %d", id);
		return;
	}
	auto buff = new Buff();
	buff->id = id;
	buff->propType = atoi(buffMeta["propType"].c_str());
	buff->timeVal = atof(buffMeta["buffTime"].c_str());
	buff->addType = atoi(buffMeta["addType"].c_str());
	buff->addNum = atoi(buffMeta["addNum"].c_str());
	buff->aging = buff->timeVal > 0;
	_heroBuffs.push_back(buff);
	StageLayer::getInstance()->addBuffSprite(id);
}

void BuffLogic::update(float delta)
{
	auto iter = _heroBuffs.begin();
	while (iter != _heroBuffs.end())
	{
		auto buff = *iter;
		if (buff->aging)
		{
			buff->timeVal -= delta * Director::getInstance()->getScheduler()->getTimeScale();
			if (buff->timeVal <= 0)
			{
				_heroBuffs.erase(iter);
				StageLayer::getInstance()->removeBuffSprite(buff->id);
				delete buff;
				continue;
			}
			if (buff->timeVal <= 2)
			{
				auto buffNode = StageLayer::getInstance()->getBuffSprite(buff->id);
				if (nullptr != buffNode)
				{
					buffNode->setVisible(!buffNode->isVisible());
				}
			}
		}
		++iter;
	}
}

std::pair<float, float> BuffLogic::getBuffByPair(const int &baseType, const int &propType)
{
	auto buffVector = baseType == BASE_TYPE_HERO ? _heroBuffs : _monsterBuffs;
	std::pair<float, float> buffPair = { 0, 0 };
	for (auto buff : buffVector)
	{
		if (buff->propType != propType)
		{
			continue;
		}
		if (buff->addType == ADD_TYPE_DIRECT)
		{
			buffPair.first += buff->addNum;
		}
		else if (buff->addType == ADD_TYPE_PERCENT)
		{
			buffPair.second += buff->addNum;
		}
	}
	return buffPair;
}

void BuffLogic::clearAll()
{
	_heroBuffs.clear();
	_heroBuffs.shrink_to_fit();
}