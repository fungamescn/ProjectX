#include "SkillExecutor.h"
#include "Template.h"
#include "Utils.h"
#include "BattleLayer.h"
#include "BulletPool.h"
#include "HurtLogic.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

SkillExecutor* SkillExecutor::_instance = nullptr;

SkillExecutor* SkillExecutor::getInstance()
{
	if (!_instance)
	{
		_instance = SkillExecutor::create();
	}
	return _instance;
}

bool SkillExecutor::init()
{
	if (!Node::init())
	{
		return false;
	}

	_processFuncs[0] = CC_CALLBACK_3(SkillExecutor::processFunc_0, this);
	_processFuncs[1] = CC_CALLBACK_3(SkillExecutor::processFunc_1, this);
	_beforeFuncs[2] = CC_CALLBACK_3(SkillExecutor::beforeFunc_2, this);
	_processFuncs[2] = CC_CALLBACK_3(SkillExecutor::processFunc_2, this);
	_processFuncs[3] = CC_CALLBACK_3(SkillExecutor::processFunc_3, this);
	_processFuncs[4] = CC_CALLBACK_3(SkillExecutor::processFunc_4, this);
	_beforeFuncs[5] = CC_CALLBACK_3(SkillExecutor::beforeFunc_5, this);
	_processFuncs[5] = CC_CALLBACK_3(SkillExecutor::processFunc_5, this);
	_processFuncs[6] = CC_CALLBACK_3(SkillExecutor::processFunc_6, this);

	return true;
}

void SkillExecutor::executeSkill(Base* conjurer, const int &id, const cocos2d::Value &param)
{
	if (conjurer) conjurer->retainSkill();
	auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, id);
	auto scriptId = atoi(skillMeta["scriptId"].c_str());
	auto delay1 = atof(skillMeta["delay1"].c_str());
	auto delay2 = atof(skillMeta["delay2"].c_str());

	Vector<FiniteTimeAction*> actionArray;
	if (_beforeFuncs[scriptId])
	{
		actionArray.pushBack(CallFunc::create([=]()
		{
			_beforeFuncs[scriptId](conjurer, id, param);
		}));
		if (delay1 > 0)
		{
			actionArray.pushBack(DelayTime::create(delay1));
		}
	}
	if (_processFuncs[scriptId])
	{
		actionArray.pushBack(CallFunc::create([=]()
		{
			_processFuncs[scriptId](conjurer, id, param);
		}));
		if (delay2 > 0)
		{
			actionArray.pushBack(DelayTime::create(delay2));
		}
	}
	if (_afterFuncs[scriptId])
	{
		actionArray.pushBack(CallFunc::create([=]()
		{
			_afterFuncs[scriptId](conjurer, id, param);
		}));
	}
	if (nullptr != conjurer)
	{
		actionArray.pushBack(CallFunc::create([=]()
		{
			conjurer->releaseSkill();
		}));
	}
	this->runAction(Sequence::create(actionArray));
}

void SkillExecutor::clearAll()
{
	while (_gatlinQueue.size() > 0)
	{
		auto base = _gatlinQueue.front();
		_gatlinQueue.pop_front();
		if (nullptr != base)
		{
			base->releaseSkill();
		}
	}
	this->cleanup();
}

void SkillExecutor::processFunc_0(Base* conjurer, const int &id, const Value& value)
{
	auto vm = value.asValueMap();
	auto dest = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	auto baseVector = BattleLayer::getInstance()->getBaseVectorByGroupType(GROUP_TYPE_HERO);
	int index = 0;
	for (auto base : baseVector)
	{
		++index;
		if (base->getCurrentHp() <= 0)
		{
			continue;
		}
		auto role = static_cast<Role*>(base);
		auto shootPoint = role->getShootPoint() + role->getPosition();
		auto baseDest = Utils::getDistancePoint(dest, index)[1];
		auto grenade = BulletPool::getInstance()->getGrenade();
		grenade->setPosition(shootPoint);
		grenade->shoot(id, baseDest);
	}
}

void SkillExecutor::processFunc_1(Base* conjurer, const int &id, const Value& value)
{
	auto vm = value.asValueMap();
	auto dest = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	auto grenade = BulletPool::getInstance()->getGrenade();
	auto role = static_cast<Role*>(conjurer);
	auto shootPoint = role->getShootPoint() + role->getPosition();
	grenade->setPosition(shootPoint);
	grenade->shoot(id, dest);
}

void SkillExecutor::beforeFunc_2(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setVisible(true);
	sprite->initWithSpriteFrameName("skill_planeShadow.png");
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	auto battleX = BattleLayer::getInstance()->getPositionX();
	auto startX = origin.x - sprite->getContentSize().width - battleX;
	sprite->setPosition(startX, target.y);

	auto endX = origin.x + visibleSize.width + sprite->getContentSize().width - battleX;
	auto moveTo = MoveTo::create(2.0f, Vec2(endX, target.y));
	auto endCall = CallFunc::create([=](){ BulletPool::getInstance()->recycleSprite(sprite); });
	sprite->runAction(Sequence::create(moveTo, endCall, NULL));
}

void SkillExecutor::processFunc_2(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	auto rocket = BulletPool::getInstance()->getRocket();
	rocket->shoot(id, target);
}

void SkillExecutor::processFunc_3(Base* conjurer, const int &id, const Value& value)
{
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	auto nuclear = BulletPool::getInstance()->getNuclear();
	nuclear->shoot(id, target);
}

void SkillExecutor::processFunc_4(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	META_LINE keys = { { "level", "1" }, { "skillId", Utils::int2String(id) } };
	auto skillMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_SKILL_LEVEL, keys);
	auto gatlin = BattleLayer::getInstance()->createOneMonster(1007, GROUP_TYPE_JUST, target, false);
	gatlin->setAttack(atoi(skillMeta["damage"].c_str()));
	gatlin->retainSkill();
	if (_gatlinQueue.size() >= 3)
	{
		for (auto iter = _gatlinQueue.begin(); iter != _gatlinQueue.end(); ++iter)
		{
			if ((*iter)->getCurrentHp() <= 0)
			{
				(*iter)->releaseSkill();
				_gatlinQueue.erase(iter);
				break;
			}
		}
	}
	_gatlinQueue.push_back(gatlin);
	while (_gatlinQueue.size() > GATLIN_MAX)
	{
		auto base = _gatlinQueue.front();
		_gatlinQueue.pop_front();
		if (nullptr == base)
		{
			continue;
		}
		if (base != gatlin && base->getCurrentHp() > 0)
		{
			base->setCurrentHp(0);
		}
		base->releaseSkill();
	}
}

void SkillExecutor::beforeFunc_5(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setVisible(true);
	sprite->initWithSpriteFrameName("skill_warningFrame.png");
	sprite->setScale(0.63f);
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	sprite->setPosition(target);

	auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, id);
	auto scriptId = atoi(skillMeta["scriptId"].c_str());
	auto delay1 = atof(skillMeta["delay1"].c_str());
	auto endCall = CallFunc::create([=](){
		sprite->setScale(1);
		BulletPool::getInstance()->recycleSprite(sprite);
	});
	sprite->runAction(Sequence::create(DelayTime::create(delay1), endCall, NULL));
}

void SkillExecutor::processFunc_5(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	auto vm = value.asValueMap();
	auto target = Vec2(vm["x"].asFloat(), vm["y"].asFloat());
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(MUSIC_EXPLOSION);
	auto animation = AnimationCache::getInstance()->getAnimation("skill_explosion");
	Action* action = Animate::create(animation);
	auto sprite = BulletPool::getInstance()->getSprite();
	sprite->setScale(1.6f);
	sprite->setVisible(true);
	sprite->setAnchorPoint(Vec2(0.5f, 0.457f));
	sprite->setPosition(target);
	auto endCall = CallFunc::create([=](){
		sprite->setScale(1);
		sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		BulletPool::getInstance()->recycleSprite(sprite);
	});
	action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
	sprite->runAction(action);

	META_LINE keys = { { "level", "1" }, { "skillId", Utils::int2String(id) } };
	auto skillMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_SKILL_LEVEL, keys);
	int attGroup = atoi(skillMeta["attGroup"].c_str());
	auto radius = atoi(skillMeta["radius"].c_str());
	auto radiusSq = radius * radius;
	for (auto type : GROUP_TYPE)
	{
		if ((attGroup & type) == 0)
		{
			continue;
		}
		for (auto base : BattleLayer::getInstance()->getBaseVectorByGroupType(type))
		{
			if (base->getCurrentHp() <= 0 || target.getDistanceSq(base->getShadowPoint()) > radiusSq)
			{
				continue;
			}
			HurtLogic::skillHurtHp(skillMeta, base);
		}
	}
}

void SkillExecutor::processFunc_6(Base* conjurer, const int &id, const cocos2d::Value& value)
{
	conjurer->setCurrentHp(0);
}