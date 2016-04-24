#include "LevelInfoLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "BattleScene.h"
#include "Config.h"
#include "Template.h"
#include "Utils.h"
#include "GameData.h"
#include "ui/UIImageView.h"
#include "PlatFormInterface.h"

USING_NS_CC;
using namespace ui;

const float LevelInfoLayer::SCALE_BEGIN_DELAY = 0.05f;
const float LevelInfoLayer::SCALE_END_DELAY = 0.1f;

LevelInfoLayer::LevelInfoLayer() : _currentPage(1)
{
}

LevelInfoLayer::~LevelInfoLayer()
{
}

bool LevelInfoLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	auto levelInfo = CSLoader::createNode("ui/LevelInfo.csb");
	levelInfo->setTag(LEVEL_INFO_TAG);
	this->addChild(levelInfo);

	for (int i = 1; i <= 2; i++)
	{
		auto buttonStr = StringUtils::format("LevelInfo_pageButton%d", i);
		auto pageButton = dynamic_cast<Button*>(levelInfo->getChildByName(buttonStr));
		pageButton->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::pageHandler, this));
	}

	auto continueBtn = dynamic_cast<Button*>(levelInfo->getChildByName("LevelInfo_battleContinue"));
	continueBtn->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::continueHandler, this));

	auto backBtn = dynamic_cast<Button*>(levelInfo->getChildByName("LevelInfo_back"));
	backBtn->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::backHandler, this));

	auto itemPage = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageItem"));
	auto buyPage = dynamic_cast<Layout*>(itemPage->getChildByName("pageItem_buyItem"));

	int shopId = 1;
	do 
	{
		for (int i = 1; i <= 2; ++i)
		{
			auto itemStr = StringUtils::format("buyItem_%d_%d", shopId, i);
			auto buyItem = dynamic_cast<Button*>(buyPage->getChildByName(itemStr));
			if (nullptr == buyItem)
			{
				shopId = -1;
				break;
			}
			buyItem->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::buyHandler, this));
		}
		++shopId;
	} while (shopId >= 1);

	auto currencyLayout = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_dollars"));
	auto currencyBtn = dynamic_cast<Button*>(currencyLayout->getChildByName("dollars_add"));
	currencyBtn->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::currencyHandler, this));

	auto pageSkill = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageSkill"));
	for (int i = 1; i <= 4; ++i)
	{
		auto indexStr = Utils::int2String(i);
		auto skillItem = dynamic_cast<Layout*>(pageSkill->getChildByName("pageSkill_skill" + indexStr));
		auto levelButton = dynamic_cast<Button*>(skillItem->getChildByName("skill_upButton"));
		levelButton->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::levelUpHandler, this));
		auto maxButton = dynamic_cast<Button*>(skillItem->getChildByName("skill_upAllButton"));
		maxButton->addTouchEventListener(CC_CALLBACK_2(LevelInfoLayer::levelMaxHandler, this));
	}

	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(levelInfo->getChildByName("LevelInfo_gift" + Utils::int2String(k)));
		giftBtn->addTouchEventListener(this, toucheventselector(LevelInfoLayer::giftHandler));
	}

	return true;
}

void LevelInfoLayer::currencyHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	PlatFormInterface::getInstance()->showPayLayout(gift_index_0);
}


void LevelInfoLayer::buyHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button*>(ref);
	auto buttonName = button->getName();
	auto splitVector = Utils::str_split(buttonName, "_");
	auto shopId = atoi(splitVector[1].c_str());
	auto index = atoi(splitVector[2].c_str());
	auto indexStr = Utils::int2String(index);
	auto shopMeta = Template::getInstance()->getMetaById(TEMPLATE_SHOP, shopId);
	auto moneyType = atoi(shopMeta["moneyType" + indexStr].c_str());
	auto moneyNum = atoi(shopMeta["moneyNum" + indexStr].c_str());
	/*if (GD_MONEY_FUNC_ERROR == GameData::getInstance()->minusMoney(moneyNum))
	{
		return;
	}*/
	auto itemId = atoi(shopMeta["itemId"].c_str());
	auto itemNum = atoi(shopMeta["itemNum" + indexStr].c_str());
	if (itemId == SKILL_ITEM_HEAL)
	{
		if (itemNum == 1) PlatFormInterface::getInstance()->showPayLayout(gift_index_16);
		else PlatFormInterface::getInstance()->showPayLayout(gift_index_20);
	}
	else if (itemId == SKILL_ITEM_NUCLEAR)
	{
		if (itemNum == 1) PlatFormInterface::getInstance()->showPayLayout(gift_index_17);
		else PlatFormInterface::getInstance()->showPayLayout(gift_index_21);
	}
	else if (itemId == SKILL_ITEM_GATLIN)
	{
		if (itemNum == 1) PlatFormInterface::getInstance()->showPayLayout(gift_index_18);
		else PlatFormInterface::getInstance()->showPayLayout(gift_index_22);
	}
	else if (itemId == SKILL_ITEM_ROCKET)
	{
		if (itemNum == 1) PlatFormInterface::getInstance()->showPayLayout(gift_index_19);
		else PlatFormInterface::getInstance()->showPayLayout(gift_index_23);
	}
}

void LevelInfoLayer::pageHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button*>(ref);
	auto btnName = button->getName();
	_currentPage = btnName.at(btnName.size() - 1) - '0';
	this->updateView();
}

void LevelInfoLayer::continueHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	auto button = dynamic_cast<Button*>(ref);
	if (type == Widget::TouchEventType::BEGAN)
	{
		auto beginAction = ScaleTo::create(SCALE_BEGIN_DELAY, 1.2f);
		beginAction->setTag(CONTINUE_ZOOM_TAG);
		button->runAction(beginAction);
		return;
	}
	if (type == Widget::TouchEventType::ENDED)
	{
		button->stopActionByTag(CONTINUE_ZOOM_TAG);
		auto endAction = ScaleTo::create(SCALE_END_DELAY, 1);
		auto endCall = CallFunc::create([](){
			BattleScene::getInstance()->switchLayer(UI_TEAM_HIRE);
		});
		button->runAction(Sequence::create(endAction, endCall, NULL));
		return;
	}
}

void LevelInfoLayer::backHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	auto button = dynamic_cast<Button *>(ref);
	if (type == Widget::TouchEventType::BEGAN)
	{
		auto beginAction = ScaleTo::create(SCALE_BEGIN_DELAY, 1.2f);
		beginAction->setTag(BACK_ZOOM_TAG);
		button->runAction(beginAction);
		return;
	}
	if (type == Widget::TouchEventType::ENDED)
	{
		button->stopActionByTag(BACK_ZOOM_TAG);
		auto endAction = ScaleTo::create(SCALE_END_DELAY, 1);
		auto endCall = CallFunc::create([](){
			BattleScene::getInstance()->switchLayer(UI_LEVEL_SELECTION);
		});
		button->runAction(Sequence::create(endAction, endCall, NULL));
		return;
	}
}

void LevelInfoLayer::updateView(bool initialize /* = false */)
{
	if (initialize)
	{
		_currentPage = 1;
	}
	auto levelInfo = this->getChildByTag(LEVEL_INFO_TAG);
	for (int i = 1; i <= 2; i++)
	{
		auto buttonStr = StringUtils::format("LevelInfo_pageButton%d", i);
		auto pageButton = dynamic_cast<Button*>(levelInfo->getChildByName(buttonStr));
		auto brightStyle = (i == _currentPage) ? Widget::BrightStyle::HIGHLIGHT : Widget::BrightStyle::NORMAL;
		pageButton->setBrightStyle(brightStyle);
	}
	auto pageItem = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageItem"));
	auto pageSkill = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageSkill"));
	pageItem->setVisible(_currentPage == 1);
	pageSkill->setVisible(_currentPage == 2);
	if (_currentPage == 1)
	{
		auto stageLevel = BattleScene::getInstance()->getStageLevel();
		auto stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, stageLevel);
		auto levelDesc = dynamic_cast<Layout*>(pageItem->getChildByName("pageItem_levelDesc"));
		auto levelName = dynamic_cast<Text*>(levelDesc->getChildByName("levelDesc_levelName"));
		levelName->setString(stageMeta["name"]);
		auto requireText = dynamic_cast<Text*>(levelDesc->getChildByName("levelDesc_requireText"));
		requireText->setString(stageMeta["desc"]);
		for (int i = 1; i <= 3; i++)
		{
			auto starStr = StringUtils::format("levelDesc_starText%d", i);
			auto starText = dynamic_cast<Text*>(levelDesc->getChildByName(starStr));
			if (!stageMeta[StringUtils::format("starEvent%d", i)].empty())
			{
				starText->setString(stageMeta[StringUtils::format("starDesc%d", i)]);
			}
		}

		auto awardNum = dynamic_cast<Text*>(levelDesc->getChildByName("levelDesc_awardNum"));
		awardNum->setString(Utils::int2String(Utils::string2Int(stageMeta["awardNum"])));
		auto monsterVec = Utils::str_split(stageMeta["monsterId"], "|");
		for (int i = 1; i <= 4; i++)
		{
			auto monsterIcon = dynamic_cast<ImageView*>(levelDesc->getChildByName("levelDesc_monster" + Utils::int2String(i)));
			if (i > monsterVec.size())
			{
				monsterIcon->setVisible(false);
			}
			else
			{
				monsterIcon->setVisible(true);
				auto monsterMeta = Template::getInstance()->getMetaById(TEMPLATE_MONSTER, Utils::string2Int(monsterVec[i - 1]));
				monsterIcon->loadTexture(monsterMeta["icon"], Widget::TextureResType::PLIST);
			}
		}

		this->updateCurrency();
		this->updateShopItem();
	}
	else if (_currentPage == 2)
	{
		for (int i = 1; i <= 4; ++i)
		{
			this->updateProperty(i);
		}
	}

	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(levelInfo->getChildByName("LevelInfo_gift" + Utils::int2String(k)));
		giftBtn->stopAllActions();
		giftBtn->setScale(1);
		auto action = ScaleBy::create(0.4, 1.1);
		giftBtn->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}

}

void LevelInfoLayer::updateCurrency()
{
	auto levelInfo = this->getChildByTag(LEVEL_INFO_TAG);
	auto currencyLayout = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_dollars"));
	auto currencyText = dynamic_cast<Text*>(currencyLayout->getChildByName("dollars_num"));
	auto currency = GameData::getInstance()->getMoney();
	currencyText->setText(Utils::int2String(currency));
}

void LevelInfoLayer::updateShopItem()
{
	auto levelInfo = this->getChildByTag(LEVEL_INFO_TAG);
	auto itemPage = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageItem"));
	auto buyPage = dynamic_cast<Layout*>(itemPage->getChildByName("pageItem_buyItem"));

	auto shopId = 1;
	while (true)
	{
		auto itemStr = StringUtils::format("buyItem_%d", shopId);
		auto buyText = dynamic_cast<Text*>(buyPage->getChildByName(itemStr));
		if (nullptr == buyText)
		{
			break;
		}
		auto shopMeta = Template::getInstance()->getMetaById(TEMPLATE_SHOP, shopId);
		auto itemId = atoi(shopMeta["itemId"].c_str());
		auto itemNum = GameData::getInstance()->getItemById(itemId);
		buyText->setText(Utils::int2String(itemNum));
		++shopId;
	}
}

void LevelInfoLayer::updateProperty(const int &propertyId)
{
	auto idStr = Utils::int2String(propertyId);
	auto levelInfo = this->getChildByTag(LEVEL_INFO_TAG);
	auto pageSkill = dynamic_cast<Layout*>(levelInfo->getChildByName("LevelInfo_pageSkill"));
	auto skillItem = dynamic_cast<Layout*>(pageSkill->getChildByName("pageSkill_skill" + idStr));
	auto levelText = dynamic_cast<Text*>(skillItem->getChildByName("skill_level"));
	auto effectText = dynamic_cast<Text*>(skillItem->getChildByName("skill_effectNum"));
	auto nextText = dynamic_cast<Text*>(skillItem->getChildByName("skill_nextNum"));
	auto upText = dynamic_cast<Text*>(skillItem->getChildByName("skill_upCostNum"));
	auto upAllText = dynamic_cast<Text*>(skillItem->getChildByName("skill_upAllCostNum"));

	auto propertyStr = GameData::getInstance()->getGameDataForKey(propertyId, GD_PROPERTY);
	auto propertyLvl = propertyStr.empty() ? 0 : Utils::string2Int(propertyStr);
	levelText->setText(StringUtils::format("%d/%d", propertyLvl, PROPERTY_MAX_LVL));
	META_LINE keys = { { "skillId", idStr }, { "level", Utils::int2String(propertyLvl) } };
	auto propertyMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_PROPERTY, keys);
	int percent = atof(propertyMeta["per"].c_str()) * 100;
	effectText->setText(StringUtils::toString(percent) + "%");
	upText->setText(propertyMeta["cost"]);
	upAllText->setText(propertyMeta["maxCost"]);
	if (propertyLvl >= PROPERTY_MAX_LVL)
	{
		nextText->setText("-");
	}
	else
	{
		META_LINE nextKeys = { { "skillId", idStr }, { "level", Utils::int2String(propertyLvl + 1) } };
		auto nextMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_PROPERTY, nextKeys);
		int nextPer = atof(nextMeta["per"].c_str()) * 100;
		nextText->setText(StringUtils::toString(nextPer) + "%");
	}
}

void LevelInfoLayer::levelUpHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button*>(ref);
	auto propertyItem = dynamic_cast<Layout*>(button->getParent());
	auto propertyName = propertyItem->getName();
	auto propertyId = propertyName.at(propertyName.size() - 1) - '0';
	auto propertyStr = GameData::getInstance()->getGameDataForKey(propertyId, GD_PROPERTY);
	auto propertyLvl = propertyStr.empty() ? 0 : Utils::string2Int(propertyStr);
	if (propertyLvl >= PROPERTY_MAX_LVL)
	{
		return;
	}
	META_LINE keys = { { "skillId", Utils::int2String(propertyId) }, { "level", Utils::int2String(propertyLvl) } };
	auto propertyMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_PROPERTY, keys);
	auto levelUpCost = atoi(propertyMeta["cost"].c_str());
	if (GD_MONEY_FUNC_ERROR == GameData::getInstance()->minusMoney(levelUpCost))
	{
		return;
	}
	auto levelUpStr = Utils::int2String(propertyLvl + 1);
	GameData::getInstance()->setGameDataForKey(propertyId, levelUpStr, GD_PROPERTY);
	this->updateProperty(propertyId);
	this->updateCurrency();
	PlatFormInterface::getInstance()->record("技能", "等级", propertyId, propertyLvl + 1);
}

void LevelInfoLayer::levelMaxHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button*>(ref);
	auto propertyItem = dynamic_cast<Layout*>(button->getParent());
	auto propertyName = propertyItem->getName();
	auto propertyId = propertyName.at(propertyName.size() - 1) - '0';
	auto propertyStr = GameData::getInstance()->getGameDataForKey(propertyId, GD_PROPERTY);
	auto propertyLvl = propertyStr.empty() ? 0 : Utils::string2Int(propertyStr);
	if (propertyLvl >= PROPERTY_MAX_LVL)
	{
		return;
	}
	META_LINE keys = { { "skillId", Utils::int2String(propertyId) }, { "level", Utils::int2String(propertyLvl) } };
	auto propertyMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_PROPERTY, keys);
	auto levelMaxCost = atoi(propertyMeta["maxCost"].c_str());
	PlatFormInterface::getInstance()->showPayLayout(propertyId + 11);
	/*if (GD_MONEY_FUNC_ERROR == GameData::getInstance()->minusMoney(levelMaxCost))
	{
		return;
	}
	auto levelMaxStr = Utils::int2String(PROPERTY_MAX_LVL);
	GameData::getInstance()->setGameDataForKey(propertyId, levelMaxStr, GD_PROPERTY);
	this->updateProperty(propertyId);
	this->updateCurrency();
	*/
}

void LevelInfoLayer::giftHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	auto button = dynamic_cast<Button *>(object);
	auto index = button->getName().at(button->getName().length() - 1) - '0';

	if (index == 1)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_1);
	}
	else if (index == 2)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_5);
	}
	else if (index == 3)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_6);
	}
	else if (index == 4)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_7);
	}
	else if (index == 5)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_24);
	}
}