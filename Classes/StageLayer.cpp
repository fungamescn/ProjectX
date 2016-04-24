#include "StageLayer.h"
#include "cocostudio/CocoStudio.h"
#include "BattleLayer.h"
#include "SkillExecutor.h"
#include "Astar.h"
#include "PopupLayer.h"
#include "Template.h"
#include "ui/UILoadingBar.h"
#include "ui/UIImageView.h"
#include "GameData.h"
#include "Utils.h"
#include "PlatFormInterface.h"

USING_NS_CC;
using namespace ui;

const float StageLayer::MOVE_DELAY = 0.02f;

const std::string StageLayer::HEAL = "Battle_heal";
const std::string StageLayer::ROCKET = "Battle_airStrike";
const std::string StageLayer::GATLIN = "Battle_gatlin";
const std::string StageLayer::NUCLEAR = "Battle_nuclear";

const std::unordered_map<std::string, int> StageLayer::BUTTON_MAP = {
		{ HEAL, SKILL_ID_HEAL }, 
		{ ROCKET, SKILL_ID_ROCKET }, 
		{ GATLIN, SKILL_ID_GATLIN }, 
		{ NUCLEAR, SKILL_ID_NUCLEAR }
};
const std::unordered_map<int, int> StageLayer::ITEM_MAP = {
		{ SKILL_ID_HEAL, SKILL_ITEM_HEAL },
		{ SKILL_ID_ROCKET, SKILL_ITEM_ROCKET },
		{ SKILL_ID_GATLIN, SKILL_ITEM_GATLIN },
		{ SKILL_ID_NUCLEAR, SKILL_ITEM_NUCLEAR }
};
const std::unordered_map<std::string, int> StageLayer::GIFT_MAP = {
		{ HEAL, gift_index_16 },
		{ ROCKET, gift_index_19 },
		{ GATLIN, gift_index_18 },
		{ NUCLEAR, gift_index_17 }
};

StageLayer* StageLayer::_instance = nullptr;

StageLayer* StageLayer::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = StageLayer::create();
	}
	return _instance;
}

StageLayer::StageLayer() : _battleUI(nullptr), _scaleDelay(0)
{
}

bool StageLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	this->addChild(SkillExecutor::getInstance(), -1);

	this->addChild(BattleLayer::getInstance());

	_battleUI = CSLoader::createNode("ui/Battle.csb");
	_heroLayout = dynamic_cast<Layout*>(_battleUI->getChildByName("Battle_heroList"));
	auto layoutPos = _heroLayout->getPosition();

	auto clipper = ClippingNode::create();
	clipper->setAnchorPoint(Vec2(0, 0.5));
	clipper->setPosition(Vec2(layoutPos.x - MOVE_BY, layoutPos.y));
	_battleUI->addChild(clipper);

	auto stencil = DrawNode::create();
	Vec2 rectangle[4];
	auto layoutSize = _heroLayout->getContentSize();
	rectangle[0] = Vec2(0, -layoutSize.height / 2);
	rectangle[1] = Vec2(MOVE_BY, -layoutSize.height / 2);
	rectangle[2] = Vec2(MOVE_BY, layoutSize.height / 2);
	rectangle[3] = Vec2(0, layoutSize.height / 2);
	Color4F white(1, 1, 1, 1);
	stencil->drawPolygon(rectangle, 4, white, 1, white);
	clipper->setStencil(stencil);

	_heroLayout->removeFromParentAndCleanup(false);
	_heroLayout->setPosition(Vec2(MOVE_BY, 0));
	clipper->addChild(_heroLayout);
	this->addChild(_battleUI, 0);

	_buffUI = Node::create();
	_buffUI->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_buffUI->setPosition(340, 585);
	this->addChild(_buffUI);

	for (auto buttonIter = BUTTON_MAP.cbegin(); buttonIter != BUTTON_MAP.cend(); ++buttonIter)
	{
		auto btnName = buttonIter->first;
		auto eachBtn = dynamic_cast<Button*>(_battleUI->getChildByName(btnName));
		eachBtn->addTouchEventListener(CC_CALLBACK_2(StageLayer::touchHandler, this));
	}

	auto stopBtn = dynamic_cast<Button*>(_battleUI->getChildByName("Battle_stop"));
	stopBtn->addTouchEventListener(CC_CALLBACK_2(StageLayer::stopHandler, this));

	_listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = [](Touch* touch, Event* event){ return true; };
	_listener->onTouchEnded = CC_CALLBACK_2(StageLayer::stageHandler, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, _battleUI);
	_listener->setEnabled(false);

	createWarnSprites();


#if DEBUG_MODE
	debugText = Text::create();
	this->addChild(debugText);
	Size contentSize = Size(300, 200);
	debugText->setContentSize(contentSize);
	debugText->setSize(contentSize);
	debugText->setPosition(Point(20, 20));
	debugText->setAnchorPoint(Point(0, 0));
#endif
	return true;
}

void StageLayer::startClock()
{
	this->schedule(schedule_selector(StageLayer::updateClock));
}

void StageLayer::stopClock()
{
	this->unschedule(schedule_selector(StageLayer::updateClock));
}

void StageLayer::updateClock(float at)
{
	int battleTime = BattleLayer::getInstance()->getBattleTime();
	int minute = battleTime / 60;
	int second = battleTime % 60;
	string clockStr = StringUtils::format("%02d:%02d", minute, second);

	auto timeText = dynamic_cast<Text*>(_battleUI->getChildByName("Battle_time"));
	timeText->setString(clockStr);
}

void StageLayer::initSkills()
{
	auto heroVec = BattleLayer::getInstance()->getBaseVectorByGroupType(GROUP_TYPE_HERO);
	for (int i = 0; i < 4; ++i)
	{
		auto headName = StringUtils::format("heroList%d", i);
		auto headLayout = dynamic_cast<Layout*>(_heroLayout->getChildByName(headName));
		headLayout->setVisible(i < heroVec.size());
		if (headLayout->isVisible())
		{
			auto hero = static_cast<Hero*>(heroVec[i]);
			this->updateHeal(hero);
			auto iconName = StringUtils::format("heroList%d_head", i);
			auto iconImage = dynamic_cast<ImageView*>(headLayout->getChildByName(iconName));
			auto imageStr = StringUtils::format("res/ui_pic_head%d.png", hero->getTemplateId());
			iconImage->loadTexture(imageStr, Widget::TextureResType::PLIST);
			headLayout->addTouchEventListener(CC_CALLBACK_2(StageLayer::healHandler, this));
		}
		else
		{
			headLayout->addTouchEventListener(nullptr);
		}
	}
	this->updateSkillItem();
}

bool StageLayer::checkSkillOption(cocos2d::ui::Button* button)
{
	auto buttonName = button->getName();
	auto bnttonIter = BUTTON_MAP.find(buttonName);
	if (bnttonIter == BUTTON_MAP.end())
	{
		return false;
	}
	auto skillId = bnttonIter->second;
	auto itemId = ITEM_MAP.find(skillId)->second;
	return BattleLayer::getInstance()->getSkillItemNum(itemId) >= 1;
}

void StageLayer::stageHandler(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (_scaleDelay <= 0)
	{
		return;
	}
	
	auto location = touch->getLocation();
	for (auto buttonIter = BUTTON_MAP.cbegin(); buttonIter != BUTTON_MAP.cend(); ++buttonIter)
	{
		auto btnName = buttonIter->first;
		if (_buttonName == btnName)
		{
			continue;
		}
		auto eachBtn = dynamic_cast<Button*>(_battleUI->getChildByName(btnName));
		auto scene = Director::getInstance()->getRunningScene();
		auto cameras = scene->getDefaultCamera();
		//eachBtn->hitTest();
		if (eachBtn->hitTest(location, cameras,NULL))
		{
			return;
		}
	}

	if (_buttonName == HEAL)
	{
		this->stopSkillTime();
		return;
	}

	auto worldPos = BattleLayer::getInstance()->convertToNodeSpace(location);
	if (_buttonName == GATLIN)
	{
		Point disNodePoint = BattleLayer::getInstance()->nodeForPosition(worldPos);
		if (PathFind::getTheOnlyInstance()->AstarIsBlock(disNodePoint.x, disNodePoint.y))
		{
			return;
		}
	}
	auto btnIter = BUTTON_MAP.find(_buttonName);
	if (btnIter == BUTTON_MAP.end())
	{
		return;
	}
	ValueMap target = {
			{ "x", cocos2d::Value(worldPos.x) },
			{ "y", cocos2d::Value(worldPos.y) }
	};
	auto skillId = btnIter->second;
	auto itemId = ITEM_MAP.find(skillId)->second;
	if (INVALID_FLAG == BattleLayer::getInstance()->minusSkillItem(itemId, 1))
	{
		return;
	}
	SkillExecutor::getInstance()->executeSkill(NULL, skillId, Value(target));
	this->updateSkillItem();
	this->stopSkillTime();
}

void StageLayer::healHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	this->stopSkillTime();
	auto headLayout = dynamic_cast<Layout*>(ref);
	auto layoutName = headLayout->getName();
	int index = layoutName.at(layoutName.size() - 1) - '0';
	auto heroVec = BattleLayer::getInstance()->getBaseVectorByGroupType(GROUP_TYPE_HERO);
	if (index >= heroVec.size())
	{
		return;
	}
	auto heroView = static_cast<Hero*>(heroVec.at(index));
	heroView->reviveHandler();
}

void StageLayer::stopHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}

	CCDirector::getInstance()->pause();
	PopupLayer::getInstance()->popup(BATTLE_POP_STOP);
}

void StageLayer::update(float delta)
{
	if (_scaleDelay <= 0)
	{
		return;
	}
	_scaleDelay -= 1;
	if (_scaleDelay <= 0)
	{
		this->stopSkillTime();
		return;
	}
	auto percent = 100.0f * _scaleDelay / SCALE_DELAY;
	auto slowProgress = dynamic_cast<LoadingBar*>(_battleUI->getChildByName("Battle_slowProgress"));
	slowProgress->setPercent(percent);
}

void StageLayer::stopSkillTime()
{
	_scaleDelay = 0;
	_buttonName.clear();
	this->unscheduleUpdate();
	_listener->setEnabled(false);
	Director::getInstance()->getScheduler()->setTimeScale(1);
	auto slowFrame = _battleUI->getChildByName("Battle_slowFrame");
	slowFrame->setVisible(false);
	auto slowProgress = _battleUI->getChildByName("Battle_slowProgress");
	slowProgress->setVisible(false);
	_heroLayout->setVisible(false);
	_heroLayout->setPositionX(MOVE_BY);
	for (auto buttonIter = BUTTON_MAP.cbegin(); buttonIter != BUTTON_MAP.cend(); ++buttonIter)
	{
		auto eachName = buttonIter->first;
		auto eachBtn = dynamic_cast<Button*>(_battleUI->getChildByName(eachName));
		eachBtn->setBright(true);
		eachBtn->setEnabled(true);
	}
}

void StageLayer::touchHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (_scaleDelay > 0)
	{
		this->stopSkillTime();
		return;
	}
	auto button = dynamic_cast<Button*>(ref);
	_buttonName = button->getName();
	if (!this->checkSkillOption(button))
	{
		auto giftIndex = GIFT_MAP.find(_buttonName)->second;
		PlatFormInterface::getInstance()->showPayLayout(giftIndex);
		return;
	}
	_scaleDelay = SCALE_DELAY;
	_listener->setEnabled(true);
	Director::getInstance()->getScheduler()->setTimeScale(0.1);
	auto slowFrame = _battleUI->getChildByName("Battle_slowFrame");
	slowFrame->setVisible(true);
	auto slowProgress = _battleUI->getChildByName("Battle_slowProgress");
	slowProgress->setVisible(true);
	for (auto buttonIter = BUTTON_MAP.cbegin(); buttonIter != BUTTON_MAP.cend(); ++buttonIter)
	{
		auto eachName = buttonIter->first;
		if (eachName == _buttonName)
		{
			continue;
		}
		auto eachBtn = dynamic_cast<Button*>(_battleUI->getChildByName(eachName));
		eachBtn->setBright(false);
		eachBtn->setEnabled(false);
	}
	if (_buttonName == HEAL)
	{
		_heroLayout->setVisible(true);
		_heroLayout->runAction(MoveBy::create(MOVE_DELAY, Vec2(-MOVE_BY, 0)));
	}
	this->scheduleUpdate();
	this->updateSkillItem();
}

void StageLayer::createWarnSprites()
{
	_warnDeadSprite = WarnSprite::createWarnSprite(1);
	this->addChild(_warnDeadSprite, 1);
	_warnDeadSprite->setPosition(400, 500);
	_warnDeadSprite->setRotation(24);
	_warnDeadSprite->setVisible(false);
	//BattleLayer::getInstance()->setDeadWarnSprite(_warnDeadSprite);
	_warnQuestSprite = WarnSprite::createWarnSprite(2);
	this->addChild(_warnQuestSprite, 2);
	_warnQuestSprite->setPosition(400, 500);
	_warnQuestSprite->setRotation(24);
	_warnQuestSprite->setVisible(false);
	//BattleLayer::getInstance()->setQuestWarnSprite(_warnQuestSprite);
	_warnEnemySprite = WarnSprite::createWarnSprite(3);
	this->addChild(_warnEnemySprite, 3);
	_warnEnemySprite->setPosition(400, 500);
	_warnEnemySprite->setRotation(24);
	_warnEnemySprite->setVisible(false);


	//BattleLayer::getInstance()->setEnemyWarnSprite(_warnEnemySprite);
}

void StageLayer::addBuffSprite(const int &buffId)
{
	auto buffMeta = Template::getInstance()->getMetaById(TEMPLATE_BUFF, buffId);
	auto sprite = Sprite::createWithSpriteFrameName(buffMeta["res"]);
	sprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	sprite->setTag(buffId);
	_buffUI->addChild(sprite);
	auto index = _buffUI->getChildrenCount() - 1;
	sprite->setPositionX(index * BUFF_GAP);
}

cocos2d::Node* StageLayer::getBuffSprite(const int &buffId)
{
	return _buffUI->getChildByTag(buffId);
}

void StageLayer::removeBuffSprite(const int &buffId)
{
	_buffUI->removeChildByTag(buffId);
	auto buffSprites = _buffUI->getChildren();
	auto buffSize = buffSprites.size();
	for (ssize_t i = 0; i < buffSize; ++i)
	{
		auto buffChild = buffSprites.at(i);
		buffChild->setPositionX(i * BUFF_GAP);
	}
}

void StageLayer::updateHeal(Hero* hero)
{
	auto heroVec = BattleLayer::getInstance()->getBaseVectorByGroupType(GROUP_TYPE_HERO);
	for (int i = 0; i < heroVec.size(); ++i)
	{
		if (hero == heroVec[i])
		{
			auto headName = StringUtils::format("heroList%d", i);
			auto headLayout = dynamic_cast<Layout*>(_heroLayout->getChildByName(headName));

			auto deadName = StringUtils::format("heroList%d_dead", i);
			auto deadImage = headLayout->getChildByName(deadName);
			deadImage->setVisible(hero->getCurrentHp() <= 0);

			auto progressName = StringUtils::format("heroList%d_progress", i);
			auto hpProgress = dynamic_cast<LoadingBar*>(headLayout->getChildByName(progressName));
			auto percent = 100 * hero->getCurrentHp() / hero->getFinalHp();
			hpProgress->setPercent(percent);
		}
	}
}

void StageLayer::updateSkillItem()
{
	for (auto buttonIter = BUTTON_MAP.cbegin(); buttonIter != BUTTON_MAP.cend(); ++buttonIter)
	{
		auto btnName = buttonIter->first;
		auto textName = btnName + "_num";
		auto eachText = dynamic_cast<Text*>(_battleUI->getChildByName(textName));
		auto skillId = buttonIter->second;
		auto itemId = ITEM_MAP.find(skillId)->second;
		auto skillNum = BattleLayer::getInstance()->getSkillItemNum(itemId);
		eachText->setText(StringUtils::format("%d", skillNum));
	}
}

void StageLayer::startWhiteSmoke()
{
	if (nullptr == _smokeLayer)
	{
		auto battleSize = _battleUI->getContentSize();
		_smokeLayer = LayerColor::create(Color4B(255, 255, 255, 255), battleSize.width, battleSize.height);
		this->addChild(_smokeLayer, 100);
	}
	auto fadeOut = FadeOut::create(1.0f);
	_smokeLayer->setOpacity(255);
	_smokeLayer->runAction(fadeOut);
}

void StageLayer::clearAll()
{
	this->stopSkillTime();
	_buffUI->removeAllChildren();
	if (nullptr != _smokeLayer)
	{
		_smokeLayer->stopAllActions();
		_smokeLayer->setOpacity(0);
	}
#if DEBUG_MODE
	while ( !debugContentArr.empty())
	{
		debugContentArr.pop();
	}
#endif
}

#if DEBUG_MODE
void StageLayer::showDebugLog(string logStr)
{
	debugContentArr.push(logStr);
	if (debugContentArr.size() > DEBUG_LOG_LINE)
	{
		debugContentArr.pop();
	}

	string content;
	for (size_t i = 0; i < debugContentArr.size(); i++)
	{
		string subStr = debugContentArr.front();
		content += subStr + "\n";
		debugContentArr.pop();
		debugContentArr.push(subStr);
	}

	debugText->setText(content);
}
#endif
