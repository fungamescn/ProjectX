#include "TeamHireLayer.h"
#include "BattleScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIButton.h"
#include "ui/UILayout.h"
#include "ui/UIPageView.h"
#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "Config.h"
#include "Utils.h"
#include "Hero.h"
#include "GameData.h"
#include "Template.h"
#include "PlatFormInterface.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
using namespace ui;

TeamHireLayer::TeamHireLayer(){};
TeamHireLayer::~TeamHireLayer(){};

bool TeamHireLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	this->loadTeamHireUI();
	return true;
}

void TeamHireLayer::loadTeamHireUI()
{
	teamHireLayer = (Layer*)CSLoader::createNode("ui/TeamHire.csb");

	addChild(teamHireLayer);

	int money = GameData::getInstance()->getMoney();
	updateMoney();

	auto dollarsLayout = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_dollars"));
	auto dollarsBtn = dynamic_cast<Button *>(dollarsLayout->getChildByName("dollars_add"));
	dollarsBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_addMoney));

	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto myTeam = dynamic_cast<Layout *>(hirePage->getChildByName("PageHire_myTeam"));

	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	heroView->addEventListener(CC_CALLBACK_0(TeamHireLayer::updateTeamHirePage, this));

	auto backBtn = dynamic_cast<Button *>(teamHireLayer->getChildByName("TeamHire_backPage"));
	backBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_back));

	auto battleStartBtn = dynamic_cast<Button *>(teamHireLayer->getChildByName("TeamHire_battleStart"));
	battleStartBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_battleStart));

	


	for (size_t i = 1; i <= MAX_HERO_VEC_LENGTH; i++)
	{
		auto index = Utils::int2String(i);

		auto panel = dynamic_cast<Layout *>(myTeam->getChildByName("TeamFire" + index));
		panel->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_fire));

		
		auto fireBtn = dynamic_cast<Button *>(panel->getChildByName("fireBtn" + index));
		fireBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_fire));
	}

	for (size_t i = 1; i <= HERO_TYPE_NUM; i++)
	{
		auto index = Utils::int2String(i);
		auto hireView = dynamic_cast<Layout *>(heroView->getChildByName("hireView_hero" + index));

		auto hireButton = dynamic_cast<Button *>(hireView->getChildByName("hireButton_" + index));
		hireButton->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_hire));

		auto unlockButton = dynamic_cast<Button *>(hireView->getChildByName("unlockButton_" + index));
		unlockButton->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_unlock));

		auto upButton = dynamic_cast<Button *>(hireView->getChildByName("up_" + index));
		upButton->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_up));

		auto upMaxButton = dynamic_cast<Button *>(hireView->getChildByName("upMax_" + index));
		upMaxButton->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_upMax));

		auto hero = Hero::create(1000 + i);
		hero->setPosition(80, 235);
		hero->setDirection(1);
		hero->stand();
		hero->hideHpBar();
		hero->setScale(1.2);
		hero->unscheduleUpdate();
		hireView->addChild(hero);
	}

	auto prePageBtn = dynamic_cast<Button *>(hirePage->getChildByName("PageHire_preHire"));
	prePageBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_prePage));

	auto nextPageBtn = dynamic_cast<Button *>(hirePage->getChildByName("PageHire_nextHire"));
	nextPageBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::teamHire_nextPage));

	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(teamHireLayer->getChildByName("TeamHire_gift" + Utils::int2String(k)));
		giftBtn->addTouchEventListener(this, toucheventselector(TeamHireLayer::giftHandler));
	}

	refreshHireHeroVec();
	refreshTeamHireUI();
	refreshHeroHireUI();
	refreshGiftBtn();
}

void TeamHireLayer::teamHire_hire(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	auto button = dynamic_cast<Button *>(object);
	auto btnName = button->getName();
	auto index = btnName.at(btnName.size() - 1) - '0';
	META_LINE  heroMeta = Template::getInstance()->getMetaById(TEMPLATE_HERO, index + 1000);
	int cost = atoi(heroMeta["hireCost"].c_str());
	for (size_t i = 0; i < MAX_HERO_VEC_LENGTH; i++)
	{
		if (i >= soldierVec.size())
		{
			if (cost == 0 || (cost > 0 && GameData::getInstance()->minusMoney(cost) >= 0 ))
			{
				BattleScene::getInstance()->updateMoney();
				soldierVec.push_back(index);
				GameData::getInstance()->setGameDataForKey(soldierVec.size(), Utils::int2String(index));
			}
			break;
		}
		else if (soldierVec[i] == 0)
		{
			if (cost == 0 || (cost > 0 && GameData::getInstance()->minusMoney(cost) >= 0 ))
			{
				BattleScene::getInstance()->updateMoney();
				soldierVec[i] = index;
				GameData::getInstance()->setGameDataForKey(i + 1, Utils::int2String(index));
			}
			break;
		}
	}

	refreshTeamHireUI();
}

void TeamHireLayer::teamHire_unlock(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button *>(object);
	auto btnName = button->getName();
	auto index = btnName.at(btnName.size() - 1) - '0';
	META_LINE  heroMeta = Template::getInstance()->getMetaById(TEMPLATE_HERO, index + 1000);
	int unlockCost = atoi(heroMeta["unlockCost"].c_str());
	if (unlockCost > 0 && GameData::getInstance()->getGameDataForKey(index, GD_TEAM_LOCK) == "")
	{
		PlatFormInterface::getInstance()->showPayLayout(index);
		/*if (GameData::getInstance()->minusMoney(unlockCost) >= 0)
		{
			BattleScene::getInstance()->updateMoney();
			GameData::getInstance()->setGameDataForKey(index + 1000, "1", GD_TEAM_LOCK);
			refreshHeroHireAndLockUI(index);
		}*/
	}
}

void TeamHireLayer::teamHire_up(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button *>(object);
	auto btnName = button->getName();
	auto index = btnName.at(btnName.size() - 1) - '0';
	int heroId = 1000 + index;
	META_LINE  heroMeta = Template::getInstance()->getMetaById(TEMPLATE_HERO, heroId);
	std::string levelValue = GameData::getInstance()->getGameDataForKey(heroId, GD_TEAM_LEVEL);
	int level = (levelValue == "") ? 0 : Utils::string2Int(levelValue);
	//人物属性与等级相关
	META_LINE keys = { { "level", Utils::int2String(level) }, { "heroId", Utils::int2String(heroId) } };
	META_LINE roleMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_LEVEL, keys);
	if (roleMeta["upCost"] == "0") return;
	if (GameData::getInstance()->minusMoney(Utils::string2Int(roleMeta["upCost"])) >= 0)
	{
		BattleScene::getInstance()->updateMoney();
		GameData::getInstance()->setGameDataForKey(heroId, Utils::int2String(level + 1), GD_TEAM_LEVEL);
		refreshHeroLevelUI(index);
		PlatFormInterface::getInstance()->record("英雄", "等级", heroId, level + 1);
	}
}

void TeamHireLayer::teamHire_upMax(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	auto button = dynamic_cast<Button *>(object);
	auto btnName = button->getName();
	auto index = btnName.at(btnName.size() - 1) - '0';
	int heroId = 1000 + index;
	META_LINE  heroMeta = Template::getInstance()->getMetaById(TEMPLATE_HERO, heroId);
	std::string levelValue = GameData::getInstance()->getGameDataForKey(heroId, GD_TEAM_LEVEL);
	int level = (levelValue == "") ? 0 : Utils::string2Int(levelValue);
	//人物属性与等级相关
	META_LINE keys = { { "level", Utils::int2String(level) }, { "heroId", Utils::int2String(heroId) } };
	META_LINE roleMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_LEVEL, keys);
	if (roleMeta["maxCost"] !="0")
	{ 
		PlatFormInterface::getInstance()->showPayLayout(index + 7);
		/*if (GameData::getInstance()->minusMoney(Utils::string2Int(roleMeta["maxCost"])) >= 0)
		{
			BattleScene::getInstance()->updateMoney();
			GameData::getInstance()->setGameDataForKey(heroId, Utils::int2String(MAX_LEVEL), GD_TEAM_LEVEL);
			refreshHeroLevelUI(index);
		}*/
	}
}

void TeamHireLayer::teamHire_fire(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	auto button = dynamic_cast<Widget *>(object);
	auto btnName = button->getName();
	auto index = btnName.at(btnName.size() - 1) - '0';

	soldierVec[index - 1] = 0;
	GameData::getInstance()->setGameDataForKey(index, "0");
	refreshTeamHireUI();
}


void TeamHireLayer::teamHire_prePage(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	auto pageIndex = heroView->getCurPageIndex() - 1;

	if (pageIndex == -1)
	{
		pageIndex = heroView->getPages().size() - 1;
	}

	heroView->scrollToPage(pageIndex);
}

void TeamHireLayer::teamHire_nextPage(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	auto pageIndex = heroView->getCurPageIndex() + 1;

	if (pageIndex == heroView->getPages().size())
	{
		pageIndex = 0;
	}

	heroView->scrollToPage(pageIndex);
}

void TeamHireLayer::refreshTeamHireUI()
{
	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));

	auto myTeam = dynamic_cast<Layout *>(hirePage->getChildByName("PageHire_myTeam"));
	for (size_t i = 1; i <= 4; i++)
	{
		auto index = Utils::int2String(i);
		bool active = i <= soldierVec.size() && soldierVec[i - 1] != 0;

		auto panel = dynamic_cast<Layout *>(myTeam->getChildByName("TeamFire" + index));
		auto fireBtn = dynamic_cast<Button *>(panel->getChildByName("fireBtn" + index));

		int soldNum = 0;
		for (size_t j = 0; j < soldierVec.size(); j++)
		{
			if (soldierVec[j])
			{
				soldNum++;
			}
		}
		bool btnActive = soldNum <= 1 ? false : active;
		fireBtn->setBright(btnActive);
		fireBtn->setEnabled(btnActive);
		panel->setEnabled(btnActive);

		auto noHeroImg = dynamic_cast<ImageView *>(panel->getChildByName("noHeroImg"));
		noHeroImg->setVisible(!active);

		if (active)
		{
			Hero* hero = dynamic_cast<Hero *>(panel->getChildByName("hero"));
			if (hero)
			{
				if (hero->getTemplateId() == 1000 + soldierVec[i - 1])
				{
					continue;
				}
				panel->removeChild(hero);
			}

			hero = Hero::create(1000 + soldierVec[i - 1]);
			hero->setName("hero");
			hero->setPosition(Point(noHeroImg->getPosition().x, noHeroImg->getPosition().y - 50));
			hero->setDirection(1);
			hero->stand();
			hero->hideHpBar();
			hero->setScale(1.2);
			hero->unscheduleUpdate();
			panel->addChild(hero);
		}
		else
		{
			Hero* hero = dynamic_cast<Hero *>(panel->getChildByName("hero"));
			if (hero)
			{
				panel->removeChild(hero);
			}
		}
	}
}

void TeamHireLayer::refreshHeroHireUI()
{
	auto hirePage =  dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	for (size_t i = 1; i <= 4; i++)
	{
		refreshHeroHireAndLockUI(i);
		refreshHeroLevelUI(i);
	}
}

void TeamHireLayer::updateTeamHirePage()
{
	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));

	for (size_t i = 1; i <= 4; i++)
	{
		auto pageCircle = dynamic_cast<Button *>(hirePage->getChildByName("PageHire_pageCircle" + Utils::int2String(i)));
		bool active = heroView->getCurPageIndex() == i - 1;
		pageCircle->setBright(active);
		pageCircle->setEnabled(active);
	}
}


cocos2d::Layer * TeamHireLayer::getTeamHireLayer()
{
	return teamHireLayer;
}

void TeamHireLayer::refreshHeroHireAndLockUI(int num)
{
	auto heroId = num + 1000;
	std::string index = Utils::int2String( num );
	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	auto hireView = dynamic_cast<Layout *>(heroView->getChildByName("hireView_hero" + index));
	auto unlockButton = dynamic_cast<Button *>(hireView->getChildByName("unlockButton_" + index));
	auto hireButton = dynamic_cast<Button *>(hireView->getChildByName("hireButton_" + index));
	auto goldIcon = dynamic_cast<ImageView *>(hireView->getChildByName("hireGold_" + index));
	auto rmbIcon = dynamic_cast<ImageView *>(hireView->getChildByName("unlockRmb_" + index));
	auto goldCost = dynamic_cast<Text *>(hireView->getChildByName("hireCostGold_" + index));
	auto goldCost2 = dynamic_cast<Text *>(hireView->getChildByName("unlockCostRmb_" + index));
	META_LINE  heroMeta = Template::getInstance()->getMetaById(TEMPLATE_HERO, heroId);
	int lockCost = atoi(heroMeta["unlockCost"].c_str());
	if (lockCost > 0 && GameData::getInstance()->getGameDataForKey(heroId, GD_TEAM_LOCK) == "")
	{
		unlockButton->setVisible(true);
		hireButton->setVisible(false);
		goldIcon->setVisible(false);
		rmbIcon->setVisible(false);
		goldCost->setVisible(false);
		goldCost2->setText(Utils::int2String(lockCost));
	}
	else{
		unlockButton->setVisible(false);
		hireButton->setVisible(true);
		goldIcon->setVisible(true);
		rmbIcon->setVisible(false);
		goldCost->setVisible(true);
		goldCost->setText(heroMeta["hireCost"]);
	}
}

void TeamHireLayer::refreshGiftBtn()
{
	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(teamHireLayer->getChildByName("TeamHire_gift" + Utils::int2String(k)));
		giftBtn->stopAllActions();
		giftBtn->setScale(1);
		auto action = ScaleBy::create(0.4, 1.1);
		giftBtn->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}
}

void TeamHireLayer::refreshHeroLevelUI(int num)
{
	auto heroId = num + 1000;
	std::string index = Utils::int2String( num );
	auto hirePage = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_pageHire"));
	auto heroView = dynamic_cast<PageView *>(hirePage->getChildByName("PageHire_heroView"));
	auto hireView = dynamic_cast<Layout *>(heroView->getChildByName("hireView_hero" + index));
	auto levelText = dynamic_cast<Text *>(hireView->getChildByName("upCLevelNum_" + index));
	auto nextLevelText = dynamic_cast<Text *>(hireView->getChildByName("upCLevelNumAdd_" + index));
	auto hpText = dynamic_cast<Text *>(hireView->getChildByName("upHpNum_" + index));
	auto nextHpText = dynamic_cast<Text *>(hireView->getChildByName("upHpNumAdd_" + index));
	auto attText = dynamic_cast<Text *>(hireView->getChildByName("upAttackNum_" + index));
	auto nextAttText = dynamic_cast<Text *>(hireView->getChildByName("upAttackNumAdd_" + index));
	auto defText = dynamic_cast<Text *>(hireView->getChildByName("upDefenceNum_" + index));
	auto nextDefText = dynamic_cast<Text *>(hireView->getChildByName("upDefenceNumAdd_" + index));
	auto upText = dynamic_cast<Text *>(hireView->getChildByName("upCostGold_" + index));
	auto upMaxText = dynamic_cast<Text *>(hireView->getChildByName("upCostRmb_" + index));
	auto upBtn = dynamic_cast<Button *>(hireView->getChildByName("up_" + index));
	auto upMaxBtn = dynamic_cast<Button *>(hireView->getChildByName("upMax_" + index));
	std::string levelValue = GameData::getInstance()->getGameDataForKey(heroId, GD_TEAM_LEVEL);
	int level = (levelValue == "") ? 0 : Utils::string2Int(levelValue);
	//人物属性与等级相关
	META_LINE keys = { { "level", Utils::int2String(level) }, { "heroId", Utils::int2String(heroId) } };
	META_LINE roleMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_LEVEL, keys);
	levelText->setText(Utils::int2String(level));
	hpText->setText(roleMeta["hp"]);
	attText->setText(roleMeta["attack"]);
	defText->setText(roleMeta["defense"]);
	if (roleMeta["upCost"]  == "0")
	{
		nextLevelText->setText("-");
		nextHpText->setText("-");
		nextAttText->setText("-");
		nextDefText->setText("-");
		upText->setText("-");
		upMaxText->setText("-");
		upBtn->setBright(false);
		upBtn->setEnabled(false);
		upMaxBtn->setBright(false);
		upMaxBtn->setEnabled(false);
	}
	else{
		int nextLevel = level + 1;
		META_LINE nextKeys = { { "level", Utils::int2String(nextLevel) }, { "heroId", Utils::int2String(heroId) } };
		META_LINE nextRoleMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_LEVEL, nextKeys);
		nextLevelText->setText(nextRoleMeta["level"]);
		nextHpText->setText(nextRoleMeta["hp"]);
		nextAttText->setText(nextRoleMeta["attack"]);
		nextDefText->setText(nextRoleMeta["defense"]);
		upText->setText(roleMeta["upCost"]);
		upMaxText->setText(roleMeta["maxCost"]);
		upBtn->setEnabled(true);
		upMaxBtn->setEnabled(true);
		upBtn->setBright(true);
		upMaxBtn->setBright(true);
	}
}

void TeamHireLayer::refreshUI()
{
	this->refreshHireHeroVec();
	this->refreshTeamHireUI();
	this->refreshHeroHireUI();
	this->refreshGiftBtn();
	
}

void TeamHireLayer::teamHire_back(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{

	auto button = dynamic_cast<Button *>(object);

	if (teamHireLayer_originalScale_0 == 0)
	{
		teamHireLayer_originalScale_0 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, teamHireLayer_originalScale_0 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, teamHireLayer_originalScale_0);
		auto endCall = CallFunc::create([](){
			BattleScene::getInstance()->switchLayer(UI_LEVEL_INFO);
		});
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		return;
	}
}

void TeamHireLayer::teamHire_battleStart(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	auto button = dynamic_cast<Button *>(object);

	if (teamHireLayer_originalScale_1 == 0)
	{
		teamHireLayer_originalScale_1 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, teamHireLayer_originalScale_1 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, teamHireLayer_originalScale_1);
		auto endCall = CallFunc::create([](){
			BattleScene::getInstance()->switchLayer(UI_LOADING);
		});
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		GameData::getInstance()->setGameDataForKey(GD_GUIDE_UI, "1");
		return;
	}
}

void TeamHireLayer::updateMoney()
{
	int num = GameData::getInstance()->getMoney();
	auto dollarsLayout = dynamic_cast<Layout *>(teamHireLayer->getChildByName("TeamHire_dollars"));
	auto dollarsText = dynamic_cast<Text *>(dollarsLayout->getChildByName("dollars_num"));
	dollarsText->setText(Utils::int2String(num));
}

void TeamHireLayer::teamHire_addMoney(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{

	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	PlatFormInterface::getInstance()->showPayLayout(gift_index_0);
	//BattleScene::getInstance()->updateMoney(GameData::getInstance()->getMoney());
}

void TeamHireLayer::myTeam_touchHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	CCLOG("1111111111");
}

void TeamHireLayer::giftHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
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

std::vector<int>& TeamHireLayer::getSoldierVec()
{
	return soldierVec;
}

void TeamHireLayer::refreshHireHeroVec()
{
	bool hasSolider = false;
	soldierVec.clear();
	for (int i = GD_TEAM_1; i <= GD_TEAM_4; i++)
	{
		std::string s = GameData::getInstance()->getGameDataForKey(i);
		int sid = s == "" ? 0 : Utils::string2Int(s);
		if (sid) hasSolider = true;
		if (!hasSolider && i == GD_TEAM_4)
		{
			soldierVec.clear();
			soldierVec.push_back(1);
			GameData::getInstance()->setGameDataForKey(GD_TEAM_1, "1");
		}
		else{
			soldierVec.push_back(sid);
		}
	}
}