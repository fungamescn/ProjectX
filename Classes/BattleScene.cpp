#include "BattleScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "SimpleAudioEngine.h"
#include "BattleLayer.h"
#include "StageLayer.h"
#include "Template.h"
#include "Utils.h"
#include "PopupLayer.h"
#include "ui/UIDeprecated.h"
#include "GameData.h"
#include "TeamHireLayer.h"
#include "BuffLogic.h"
#include "LevelInfoLayer.h"
#include "PlatFormInterface.h"
#include "GuideLayer.h"

USING_NS_CC;
using namespace std;
using namespace ui;
using namespace cocostudio;
BattleScene* BattleScene::_instance = nullptr;

BattleScene* BattleScene::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = BattleScene::create();
	}
	return _instance;
}

bool BattleScene::init()
{
	CCLOG("%s", FileUtils::getInstance()->getWritablePath().c_str());
	//int money = Utils::string2Int(GameData::getInstance()->getGameDataForKey(7, 0));
	this->loadAsset();
	loadGameStartUI();
	loadLevelSelectionUI();
	teamHireLayer = TeamHireLayer::create();
	LoadingLayer = (Layer*)CSLoader::createNode("ui/loading.csb");
	levelInfoLayer = LevelInfoLayer::create();
	layerMultiplex = LayerMultiplex::create(
		gameStartLayer,
		levelSelectionLayer,
		levelInfoLayer,
		teamHireLayer,
		LoadingLayer,
		StageLayer::getInstance(),
		nullptr);
	addChild(layerMultiplex, 0);
	this->initPopupLayer();
	this->initNoticeLayer();
    
    //直接进入游戏 jgd
    this->btnUpEffect(4);
    
	return true;
}

void BattleScene::initPopupLayer()
{
	Layer* winLayer = (Layer*)CSLoader::createNode("ui/BattleWin.csb");
	Widget* WinQuit = (Widget*)winLayer->getChildByName("BattleWin_quit");
	WinQuit->addTouchEventListener(CC_CALLBACK_2(BattleScene::quitHandler, this));
	Widget* WinReplay = (Widget*)winLayer->getChildByName("BattleWin_replay");
	WinReplay->addTouchEventListener(CC_CALLBACK_2(BattleScene::replayHandler, this));
	Widget* WinGo = (Widget*)winLayer->getChildByName("BattleWin_go");
	WinGo->addTouchEventListener(CC_CALLBACK_2(BattleScene::winGoHander, this));

	Layer* loseLayer = (Layer*)CSLoader::createNode("ui/BattleLose.csb");
	Widget* loseQuit = (Widget*)loseLayer->getChildByName("BattleLose_quit");
	loseQuit->addTouchEventListener(CC_CALLBACK_2(BattleScene::quitHandler, this));
	Widget* loseReplay = (Widget*)loseLayer->getChildByName("BattleLose_replay");
	loseReplay->addTouchEventListener(CC_CALLBACK_2(BattleScene::replayHandler, this));
	Widget* loseGo = (Widget*)loseLayer->getChildByName("BattleLose_go");
	loseGo->addTouchEventListener(CC_CALLBACK_2(BattleScene::loseGoHander, this));

	Layer* stopLayer = (Layer*)CSLoader::createNode("ui/BattleStop.csb");
//	stopLayer->setOnEnterCallback(CC_CALLBACK_0(BattleScene::refreshStopLayer, this));
	Widget* stopQuit = (Widget*)stopLayer->getChildByName("BattleStop_quit");
	stopQuit->addTouchEventListener(CC_CALLBACK_2(BattleScene::quitHandler, this));
	Widget* stopReplay = (Widget*)stopLayer->getChildByName("BattleStop_replay");
	stopReplay->addTouchEventListener(CC_CALLBACK_2(BattleScene::replayHandler, this));
	Widget* stopGo = (Widget*)stopLayer->getChildByName("BattleStop_go");
	stopGo->addTouchEventListener(CC_CALLBACK_2(BattleScene::resumeHandler, this));
	
	PopupLayer::getInstance()->initWithLayers(winLayer, loseLayer, stopLayer, nullptr);
	this->addChild(PopupLayer::getInstance());
}

void BattleScene::refreshStopLayer()
{

}

void BattleScene::quitHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (CCDirector::getInstance()->isPaused())
	{
		CCDirector::getInstance()->resume();
	}
	BattleLayer::getInstance()->refreshHireHeros();
	PopupLayer::getInstance()->hide();
	BuffLogic::getInstance()->scheduleBuffs(false);
	BuffLogic::getInstance()->clearAll();
	StageLayer::getInstance()->stopSkillTime();

	selectMainStage = maxStage / 1000;
	selectSubStage = maxStage % 1000;

	if (selectMainStage == 0)
	{
		switchLayer(UI_GAME_START);
	}
	else
	{
		switchLayer(UI_LEVEL_SELECTION);
	}

	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(MUSIC_BACKGROUD_FILE);
	PlatFormInterface::getInstance()->adv_start();
}

void BattleScene::resumeHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (CCDirector::getInstance()->isPaused())
	{
		CCDirector::getInstance()->resume();
	}
	
	PopupLayer::getInstance()->hide();
}

void BattleScene::replayHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (CCDirector::getInstance()->isPaused())
	{
		CCDirector::getInstance()->resume(); 
	}
	BattleLayer::getInstance()->refreshHireHeros();
	static_cast<TeamHireLayer *>(teamHireLayer)->refreshUI();
	PopupLayer::getInstance()->hide();
	BuffLogic::getInstance()->clearAll();
	StageLayer::getInstance()->stopSkillTime();
	switchLayer(UI_LOADING);
}

void BattleScene::winGoHander(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}

	if (getStageLevel() == MAX_STAGE)
	{
		return;
	}

	if (CCDirector::getInstance()->isPaused())
	{
		CCDirector::getInstance()->resume();
	}
	BattleLayer::getInstance()->refreshHireHeros();
	static_cast<TeamHireLayer *>(teamHireLayer)->refreshUI();
	PopupLayer::getInstance()->hide();
	BuffLogic::getInstance()->clearAll();
	StageLayer::getInstance()->stopSkillTime();
	
	selectSubStage++;
	if (selectSubStage > 10)
	{
		selectMainStage++;
		selectSubStage = 1;
	}

	switchLayer(UI_LEVEL_SELECTION);
//	switchLayer(UI_LOADING);

}

void BattleScene::loseGoHander(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	PlatFormInterface::getInstance()->showPayLayout(gift_index_25);
}

void BattleScene::allResurrection()
{
	if (CCDirector::getInstance()->isPaused())
	{
		CCDirector::getInstance()->resume();
	}

	PopupLayer::getInstance()->hide();

	auto heroVec = BattleLayer::getInstance()->heroVec;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		static_cast<Hero*>(heroVec[i])->reviveHandler();
	}

	BattleLayer::getInstance()->scheduleUpdate();
	StageLayer::getInstance()->startClock();
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_BACKGROUD_FILE, true);
}

void BattleScene::loadGameStartUI()
{
	gameStartLayer = (Layer*)CSLoader::createNode("ui/GameStart.csb");
	auto startBtn = dynamic_cast<Button *>(gameStartLayer->getChildByName("GameStart_start"));
	startBtn->addTouchEventListener(this, toucheventselector(BattleScene::gameStart_start));
	auto exitBtn = dynamic_cast<Button *>(gameStartLayer->getChildByName("GameStart_exit"));
	exitBtn->addTouchEventListener(this, toucheventselector(BattleScene::gameStart_exit));
	auto giftBtn = dynamic_cast<Button *>(gameStartLayer->getChildByName("GameStart_gift"));
	giftBtn->addTouchEventListener(this, toucheventselector(BattleScene::gameStart_gift));
	gameStartLayer_originalScale_1 = giftBtn->getScale();

	refreshGameStartUI();
    
    
}


//
void BattleScene::refreshGameStartUI()
{
	auto giftBtn = dynamic_cast<Button *>(gameStartLayer->getChildByName("GameStart_gift"));
	giftBtn->stopAllActions();
	giftBtn->setScale(gameStartLayer_originalScale_1);
	auto action = ScaleTo::create(0.4, gameStartLayer_originalScale_1 * 1.1);
	auto action2 = ScaleTo::create(0.4, gameStartLayer_originalScale_1);
	giftBtn->runAction(RepeatForever::create(Sequence::create(action, action2, NULL)));
}

void BattleScene::gameStart_exit(Object* object, TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	Director::getInstance()->end();
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)  
	exit(0);
	#endif
}

void BattleScene::gameStart_start(Object* object, TouchEventType type)
{
	auto button = dynamic_cast<Button *>(object);
	if (gameStartLayer_originalScale_0 == 0)
	{
		gameStartLayer_originalScale_0 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, gameStartLayer_originalScale_0 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, gameStartLayer_originalScale_0);

		CallFunc * endCall;
		if (maxStage == 1)
		{
			endCall = CallFunc::create(CC_CALLBACK_0(BattleScene::btnUpEffect, this, UI_LOADING));
		}
		else
		{
			endCall = CallFunc::create(CC_CALLBACK_0(BattleScene::btnUpEffect, this, UI_LEVEL_SELECTION));
		}
		
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		return;
	}
}

void BattleScene::gameStart_gift(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
{
	if (type != TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	PlatFormInterface::getInstance()->showPayLayout(gift_index_26);
}

void BattleScene::btnUpEffect(int toLayer)
{
	switchLayer(toLayer);
}

void BattleScene::loadLevelSelectionUI()
{
	levelSelectionLayer = (Layer*)CSLoader::createNode("ui/LevelSelection.csb");
	auto levelView = dynamic_cast<PageView *>(levelSelectionLayer->getChildByName("LevelSelection_levelView"));

	auto scrollLevel = dynamic_cast<ScrollView *>(levelSelectionLayer->getChildByName("LevelSelection_scrollLevel"));
	for (size_t j = 0; j < 4; j++)
	{
		auto pageIndex = Utils::int2String(j / 3 + 1);
		auto levelView_page = dynamic_cast<Layout *>(levelView->getChildByName("levelView_page" + pageIndex));
		auto bigLevel = dynamic_cast<Button *>(levelView_page->getChildByName("bigLevel" + Utils::int2String(j)));
		bigLevel->addTouchEventListener(this, toucheventselector(BattleScene::levelSelection_bigSelect));
	}

	auto levelViewPre = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_pre"));
	levelViewPre->addTouchEventListener(CC_CALLBACK_2(BattleScene::levelSelection_pre, this));

	auto levelViewNext = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_next"));
	levelViewNext->addTouchEventListener(CC_CALLBACK_2(BattleScene::levelSelection_next, this));

	auto levelContainerBox = dynamic_cast<Layout *>(levelSelectionLayer->getChildByName("LevelSelection_level"));
	for (size_t i = 1; i <= 10; i++)
	{
		auto levelContainer = dynamic_cast<Layout *>(levelContainerBox->getChildByName("level_container" + Utils::int2String(i)));
		levelContainer->addTouchEventListener(this, toucheventselector(BattleScene::levelSelection_select));
	}

	auto backBtn = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_back"));
	backBtn->addTouchEventListener(this, toucheventselector(BattleScene::levelSelection_back));

	auto continueBtn = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_continue"));
	continueBtn->addTouchEventListener(this, toucheventselector(BattleScene::levelSelection_continue));


	auto maxStg = GameData::getInstance()->getGameDataForKey(GD_MAX_STAGE);
	maxStage = maxStg == "" ? 1 : Utils::string2Int(maxStg);
	selectMainStage = maxStage / 1000;
	selectSubStage = maxStage % 1000;

	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_gift" + Utils::int2String(k)));
		giftBtn->addTouchEventListener(this, toucheventselector(BattleScene::levelSelection_gift));
	}

//	refreshLevelSelectionUI();
}

void BattleScene::refreshLevelSelectionUI()
{
	auto levelView = dynamic_cast<PageView *>(levelSelectionLayer->getChildByName("LevelSelection_levelView"));

	for (size_t i = 0; i < 4; i++)
	{
		auto index = Utils::int2String(i);

		auto pageIndex = Utils::int2String(i / 3 + 1);
		auto levelView_page = dynamic_cast<Layout *>(levelView->getChildByName("levelView_page" + pageIndex));
		auto mainLevel = dynamic_cast<Button *>(levelView_page->getChildByName("bigLevel" + index));
		
		if (selectMainStage == i)
		{
			mainLevel->setScale(1.1);
		}
		else
		{
			mainLevel->setScale(1);
		}

		int maxMainStage = maxStage / 1000;
		if (i <= maxMainStage)
		{
			mainLevel->setBright(true);
			mainLevel->setEnabled(true);
		}
		else
		{
			mainLevel->setBright(false);
			mainLevel->setEnabled(false);
		}
	}

	auto levelContainerBox = dynamic_cast<Layout *>(levelSelectionLayer->getChildByName("LevelSelection_level"));
	for (size_t i = 1; i <= 10; i++)
	{
		string index = Utils::int2String(i);
		bool active = selectMainStage * 1000 + i <= maxStage;
		
		auto levelContainer = dynamic_cast<Layout *>(levelContainerBox->getChildByName("level_container" + index));
		levelContainer->setEnabled(active);
	
		auto bgBtn = dynamic_cast<Button *>(levelContainer->getChildByName("container" + index + "_frame"));
		bgBtn->setBright(active);

		auto text = dynamic_cast<Text *>(levelContainer->getChildByName("container" + index + "_num"));
		text->setVisible(active);


		auto stageId = selectMainStage * 1000 + i;
		auto starNumStr = GameData::getInstance()->getGameDataForKey(stageId, GD_STAGE_STAR);
		auto starNum = Utils::string2Int(starNumStr);
		for (size_t j = 1; j <= 3; j++)
		{
			auto starIcon = dynamic_cast<Button *>(levelContainer->getChildByName("container" + index + "_star" + Utils::int2String(j)));
			starIcon->setVisible(active);
			starIcon->setBright(j <= starNum);
		}
	}

	for (size_t k = 1; k <= 5; k++)
	{
		auto giftBtn = dynamic_cast<Button *>(levelSelectionLayer->getChildByName("LevelSelection_gift"+Utils::int2String(k)));
		giftBtn->stopAllActions();
		giftBtn->setScale(1);
		auto action = ScaleBy::create(0.4, 1.1);
		giftBtn->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}
	
	auto stageLevel = getStageLevel();
	auto stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, stageLevel);
	if (stageMeta["guideStart"] != "")
	{
		auto guideId = Utils::string2Int(stageMeta["guideStart"]);
		GuideLayer::getInstance()->showById(guideId);
	}
}

void BattleScene::levelSelection_bigSelect(Object* object, TouchEventType type)
{
	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	auto button = dynamic_cast<Button *>(object);
	selectMainStage = button->getName().at(button->getName().length() - 1) - '0';

	if (maxStage / 1000 >= selectMainStage)
	{
		selectSubStage = 10;
	}
	else
	{
		selectSubStage = maxStage % 1000;
	}

	refreshLevelSelectionUI();
}

void BattleScene::levelSelection_select(Object* object, TouchEventType type)
{
	auto button = dynamic_cast<Layout *>(object);

	if (levelSelectionLayer_originalScale_0 == 0)
	{
		levelSelectionLayer_originalScale_0 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, levelSelectionLayer_originalScale_0 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		auto btnName = button->getName();
		auto index = btnName.substr(15, btnName.size());
		selectSubStage = Utils::string2Int(index);

		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, levelSelectionLayer_originalScale_0);
		auto endCall = CallFunc::create(CC_CALLBACK_0(BattleScene::btnUpEffect, this, UI_LEVEL_INFO));
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		return;
	}
}

void BattleScene::levelSelection_back(Object* object, TouchEventType type)
{
	auto button = dynamic_cast<Button *>(object);

	if (levelSelectionLayer_originalScale_1 == 0)
	{
		levelSelectionLayer_originalScale_1 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, levelSelectionLayer_originalScale_1 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, levelSelectionLayer_originalScale_1);
		auto endCall = CallFunc::create(CC_CALLBACK_0(BattleScene::btnUpEffect, this, UI_GAME_START));
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		return;
	}
}

void BattleScene::levelSelection_continue(Object* object, TouchEventType type)
{
	auto button = dynamic_cast<Button *>(object);

	if (levelSelectionLayer_originalScale_2 == 0)
	{
		levelSelectionLayer_originalScale_2 = button->getScale();
	}

	if (type == TouchEventType::TOUCH_EVENT_BEGAN)
	{
		Action *zoomAction = ScaleTo::create(BTN_SCALE_UP_TIME, levelSelectionLayer_originalScale_2 * 1.2f);
		zoomAction->setTag(kZoomActionTag);
		button->runAction(zoomAction);
		return;
	}

	if (type == TouchEventType::TOUCH_EVENT_ENDED)
	{
		button->stopActionByTag(kZoomActionTag);
		Action *zoomAction = ScaleTo::create(BTN_SCALE_DOWN_TIME, levelSelectionLayer_originalScale_2);
		auto endCall = CallFunc::create(CC_CALLBACK_0(BattleScene::btnUpEffect, this, UI_LEVEL_INFO));
		zoomAction = Sequence::create(static_cast<FiniteTimeAction*>(zoomAction), endCall, NULL);
		button->runAction(zoomAction);
		return;
	}
}

void BattleScene::levelSelection_gift(cocos2d::Object* object, cocos2d::ui::TouchEventType type)
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

void BattleScene::levelSelection_pre(cocos2d::Object* object, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}

	auto levelView = dynamic_cast<PageView *>(levelSelectionLayer->getChildByName("LevelSelection_levelView"));
	auto pageIndex = levelView->getCurPageIndex() - 1;
	levelView->scrollToPage(pageIndex);
}

void BattleScene::levelSelection_next(cocos2d::Object* object, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto levelView = dynamic_cast<PageView *>(levelSelectionLayer->getChildByName("LevelSelection_levelView"));
	auto pageIndex = levelView->getCurPageIndex() + 1;
	levelView->scrollToPage(pageIndex);
}


//加载完资源开始游戏
void BattleScene::startGame(float at)
{
	auto selectStage = getStageLevel();
	BattleLayer::getInstance()->initStage(selectStage);

	std::vector<int> soldierVec;
	soldierVec = teamHireLayer->getSoldierVec();
	BattleLayer::getInstance()->initTeam(soldierVec);
	BattleLayer::getInstance()->gameStart();

	switchLayer(UI_STAGE_LAYER);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_BACKGROUD_FILE, true);
}



//加载资源
void BattleScene::loadAsset()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("other.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("map/miniMap.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("warnSprite.plist");

	AnimationCache::getInstance()->addAnimationsWithFile("bulletAnimation.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("hitAnimation.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("shieldAnimation.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("warnSpriteAnimation.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("skillAnimation.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("other_animation.plist");
    
    
   
    ArmatureDataManager::getInstance()->addArmatureFileInfo("nuge0.png", "nuge0.plist", "nuge.ExportJson");
    ArmatureDataManager::getInstance()->addArmatureFileInfo("zombie10.png", "zombie10.plist", "zombie1.ExportJson");

	for (int id = 1001; id <= 1004; id++)
	{
		META_LINE role = Template::getInstance()->getMetaById(TEMPLATE_HERO, id);
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect((MUSIC_FIRE + role["sound"]).c_str());
		int bulletId = atoi(role["bulletId"].c_str());
		META_LINE bulletMeta = Template::getInstance()->getMetaById(TEMPLATE_BULLET, bulletId);
		if (bulletMeta["sound"] != "")
			CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect((MUSIC_FIRE + bulletMeta["sound"]).c_str());
	}
	//CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("music/sound_fire.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MUSIC_BACKGROUD_FILE);
}

//资源预加载
void BattleScene::prestrainLoad()
{
	_loadNum = 0; 
	auto currentStage = getStageLevel();
	BattleLayer::getInstance()->cleanUp(currentStage);
	META_LINE keys = { { "stageId", Utils::int2String(currentStage) } };
	std::vector<META_LINE>  stageTiles = Template::getInstance()->getMetaVectorByKeys(TEMPLATE_STAGE_TILES, keys);
	_loadMaxNum =  stageTiles.size();
	for (auto stageTile : stageTiles)
	{
		Director::getInstance()->getTextureCache()->addImageAsync("map/" + stageTile["tiledSprite"] + ".png", CC_CALLBACK_0(BattleScene::callBackPrestrainLoad, this));

	}
	META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, currentStage);
	if (stageMeta["plistStr"] != "")
	{
		vector<string> plistVec = Utils::str_split(stageMeta["plistStr"], "|");
		//_loadMaxNum = _loadMaxNum + plistVec.size();
		for (auto plist : plistVec)
		{
			vector<string> plistInfo = Utils::str_split(plist, "-");
			int type = plistInfo[0] == "m" ? TEMPLATE_MONSTER : TEMPLATE_ELEMENT_MODEL;
			META_LINE role = Template::getInstance()->getMetaById(type, atoi(plistInfo[1].c_str()));
			if (role["sound"] != "")
				CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect((MUSIC_FIRE + role["sound"]).c_str());
			if (type == TEMPLATE_MONSTER)
			{
				_loadMaxNum++;
				Director::getInstance()->getTextureCache()->addImageAsync("monster_" + role["resId"] + ".png", CC_CALLBACK_0(BattleScene::callBackPrestrainAnimationPlist, this, "monster_" + role["resId"]));
				int bulletId = atoi(role["bulletId"].c_str());
				META_LINE bulletMeta = Template::getInstance()->getMetaById(TEMPLATE_BULLET, bulletId);
				if (bulletMeta["sound"] != "")
					CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect((MUSIC_FIRE + bulletMeta["sound"]).c_str());

			}
		}

	}

	_loadMaxNum++;
	Director::getInstance()->getTextureCache()->addImageAsync("elements.png", CC_CALLBACK_0(BattleScene::callBackPrestrainAnimationPlist, this, "elements"));

	_loadMaxNum++;
	Director::getInstance()->getTextureCache()->addImageAsync("battleItem.png", CC_CALLBACK_0(BattleScene::callBackPrestrainSpritePlist, this, "battleItem"));
}

void BattleScene::callBackPrestrainLoad()
{
	_loadNum++;
	if (_loadNum == _loadMaxNum)
	{
		scheduleOnce(schedule_selector(BattleScene::startGame), 1);
	}
}

void BattleScene::callBackPrestrainSpritePlist(std::string str)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str + ".plist");
	_loadNum++;
	if (_loadNum == _loadMaxNum)
	{
		scheduleOnce(schedule_selector(BattleScene::startGame), 1);
	}
}

void BattleScene::callBackPrestrainAnimationPlist(std::string str)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str + ".plist");
	AnimationCache::getInstance()->addAnimationsWithFile(str + "_animation.plist");
	_loadNum++;
	if (_loadNum == _loadMaxNum)
	{
		scheduleOnce(schedule_selector(BattleScene::startGame), 1);
	}
}



void BattleScene::switchLayer(unsigned const int &LayerIndex)
{
	layerMultiplex->switchTo(LayerIndex);
	if (LayerIndex == UI_LOADING)
	{
		this->schedule(schedule_selector(BattleScene::changeLoadingAngle), 0.1);
		prestrainLoad();
	}
	else
	{
		this->unschedule(schedule_selector(BattleScene::changeLoadingAngle));
		if (LayerIndex == UI_GAME_START)
		{
			refreshGameStartUI();
			PlatFormInterface::getInstance()->adv_start();
		}
		else if (LayerIndex == UI_TEAM_HIRE)
		{
			teamHireLayer->refreshUI();
			teamHireLayer->updateMoney();
		}
		else if (LayerIndex == UI_LEVEL_INFO)
		{
			levelInfoLayer->updateView(true);
		}
		else if (LayerIndex == UI_LEVEL_SELECTION)
		{
			refreshLevelSelectionUI();
		}
	}
//	showGuide(1);
}

void BattleScene::changeLoadingAngle(float at)
{
	auto loadBigCircle = dynamic_cast<ImageView *>(LoadingLayer->getChildByName("loading_loadBigCircle"));
	auto loadSmallCircle = dynamic_cast<ImageView *>(LoadingLayer->getChildByName("loading_loadSmallCircle"));
	loadBigCircle->setRotation((loadBigCircle->getRotation() <= -180 ? 0 : loadBigCircle->getRotation()) - 10);
	loadSmallCircle->setRotation((loadSmallCircle->getRotation() >= 180 ? 0 : loadSmallCircle->getRotation()) + 10);
}

int BattleScene::getStageLevel()
{
	auto selectStage = selectMainStage * 1000 + selectSubStage;
	return selectStage;
}

void BattleScene::updateMoney()
{
	(teamHireLayer)->updateMoney();
	(levelInfoLayer)->updateCurrency();
}

void BattleScene::unLockNextStage(int stage)
{
	if (stage < maxStage)
	{
		return;
	}

	if (maxStage == MAX_STAGE)
	{
		return;
	}

	if (maxStage % 1000 == 10)
	{
		maxStage = maxStage + 1000 - 9;
	}
	else
	{
		maxStage++;
	}

	GameData::getInstance()->setGameDataForKey(GD_MAX_STAGE, Utils::int2String(maxStage));
	PlatFormInterface::getInstance()->record("解锁关卡", "", maxStage, 0);
}

void BattleScene::refreshUI(int pid)
{
	if (pid == UI_TEAM_HIRE)
	{
		teamHireLayer->refreshUI();
	}
	else if (pid == UI_LEVEL_INFO)
	{
		levelInfoLayer->updateView(false);
	}
}

void BattleScene::updateAll()
{
	if (nullptr != teamHireLayer)
	{
		teamHireLayer->refreshUI();
	}
	if (nullptr != levelInfoLayer)
	{
		levelInfoLayer->updateView();
	}
	if (StageLayer::getInstance()->isRunning())
	{
		StageLayer::getInstance()->updateSkillItem();
	}
}

void BattleScene::initNoticeLayer()
{
	this->addChild(NoticeLayer::getInstance());
}