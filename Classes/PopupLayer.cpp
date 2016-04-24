#include "PopupLayer.h"
#include "SimpleAudioEngine.h"
#include "Config.h"
#include "Template.h"
#include "StarLevel.h"
#include "Utils.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "GameData.h"
#include "GuideLayer.h"
#include "PlatFormInterface.h"
#include "Base.h"
#include "BattleLayer.h"

USING_NS_CC;
using namespace ui;

PopupLayer* PopupLayer::_instance = nullptr;

PopupLayer* PopupLayer::getInstance()
{
	if (nullptr == _instance)
	{
		_instance = PopupLayer::create();
	}
	return _instance;
}

PopupLayer::PopupLayer() 
	: _enabledLayer(POPUP_NULL), 
	_listener(nullptr)
{
}

PopupLayer::~PopupLayer()
{
	_eventDispatcher->removeEventListener(_listener);
	for (const auto &layer : _layers)
	{
		layer->cleanup();
	}
}

bool PopupLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	_listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = [](Touch* touch, Event* event){ return true; };
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
	_listener->setEnabled(false);
	return true;
}

void PopupLayer::initWithLayers(cocos2d::Layer* layer, ...)
{
	va_list args;
	va_start(args, layer);

	_layers.reserve(3);
	_layers.pushBack(layer);

	Layer *l = va_arg(args, Layer*);
	while (l)
	{
		_layers.pushBack(l);
		l = va_arg(args, Layer*);
	}
	va_end(args);
}

void PopupLayer::popup(const int &layer)
{
	if (layer < 0 || layer >= _layers.size())
	{
		return;
	}
	if (_enabledLayer != POPUP_NULL)
	{
		_layers.at(_enabledLayer)->removeFromParent();
	}
	_enabledLayer = layer;
	this->addChild(_layers.at(layer));
	_listener->setEnabled(true);
	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();

	refresh();
	PlatFormInterface::getInstance()->adv_start();
	//CCDirector::getInstance()->pause();
}

void PopupLayer::hide()
{
	if (_enabledLayer != POPUP_NULL)
	{
		_layers.at(_enabledLayer)->removeFromParent();
		_listener->setEnabled(false);
		_enabledLayer = POPUP_NULL;
	}
	//if (CCDirector::getInstance()->isPaused()) CCDirector::getInstance()->resume();
}

void PopupLayer::refresh()
{
	switch (_enabledLayer)
	{
	case BATTLE_POP_WIN:
		refreshWinLayer();
		break;
	case BATTLE_POP_LOSE:
		refreshLoseLayer();
		break;
	case BATTLE_POP_STOP:
		refreshStopLayer();
		break;
	default:
		break;
	}
}

void PopupLayer::refreshWinLayer()
{
	auto layer = _layers.at(_enabledLayer);
	
	auto stageId = StarLevel::getInstance()->getStageId();
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, stageId);

	auto levels = StarLevel::getInstance()->getCurrnetLevels();
	int curLevel = 0;

	for (size_t i = 0; i < levels.size(); i++)
	{
		auto index = Utils::int2String(i+1);

		auto starBtn = dynamic_cast<Button *>(layer->getChildByName("BattleWin_star" + index));
		starBtn->setBright(levels[i] != 0);

		auto starText = dynamic_cast<Text *>(layer->getChildByName("BattleWin_star" + index + "Text"));
		starText->setString(meta["starDesc" + index]);
		
		curLevel += levels[i];
	}

	//通关奖励
	auto awardCell = dynamic_cast<Layout *>(layer->getChildByName("BattleWin_award1"));
	
	auto itemId = Utils::string2Int(meta["award"]);
	auto itemMeta = Template::getInstance()->getMetaById(TEMPLATE_ITEM, itemId);
	auto awardIcon = dynamic_cast<ImageView *>(awardCell->getChildByName("award1_icon"));
	awardIcon->loadTexture(itemMeta["icon"], Widget::TextureResType::PLIST);

	auto awardNum = dynamic_cast<Text *>(awardCell->getChildByName("award1_num"));
	awardNum->setString(Utils::int2String(Utils::string2Int(meta["awardNum"])));


	auto starLevelStr = GameData::getInstance()->getGameDataForKey(stageId, GD_STAGE_STAR);
	auto starLevel = Utils::string2Int(starLevelStr);
	

	//星级奖励
	for (size_t i = 2; i <= 4; i++)
	{
		auto index = Utils::int2String(i);

		auto awardCell = dynamic_cast<Layout *>(layer->getChildByName("BattleWin_award" + index));
		auto awardIcon = dynamic_cast<Button *>(awardCell->getChildByName("award" + index + "_icon"));
		auto awardGet = dynamic_cast<Button *>(awardCell->getChildByName("award" + index + "_get"));


		auto awardIndex = Utils::int2String(i-1);
		auto itemId = Utils::string2Int(meta["starAward" + awardIndex]);
		auto itemMeta = Template::getInstance()->getMetaById(TEMPLATE_ITEM, itemId);
		awardIcon->loadTextureNormal(itemMeta["icon"], Widget::TextureResType::PLIST);

		auto awardNum = dynamic_cast<Text *>(awardCell->getChildByName("award" + index + "_num"));
		awardNum->setString(meta["num" + awardIndex]);

		//历史已达成
		if (starLevel >= i - 1)
		{
			awardGet->setVisible(true);
			awardGet->setBright(false);
			awardIcon->setBright(false);
		}
		else
		{
			//本次达成
			if (curLevel >= i - 1)
			{
				awardGet->setVisible(false);
				awardIcon->setBright(true);
			}
			else//本次未达成
			{
				awardGet->setVisible(false);
				awardIcon->setBright(false);
			}
		}

	}
}

void PopupLayer::refreshLoseLayer()
{

}

void PopupLayer::refreshStopLayer()
{
	auto layer = _layers.at(_enabledLayer);
	auto stageId = StarLevel::getInstance()->getStageId();
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, stageId);

	auto desc = dynamic_cast<Text *>(layer->getChildByName("BattleStop_desc"));
	desc->setString(meta["desc"]);

	auto desc1 = dynamic_cast<Text *>(layer->getChildByName("BattleStop_star1Text"));
	desc1->setString(meta["starDesc1"]);

	auto desc2 = dynamic_cast<Text *>(layer->getChildByName("BattleStop_star2Text"));
	desc2->setString(meta["starDesc2"]);

	auto desc3 = dynamic_cast<Text *>(layer->getChildByName("BattleStop_star3Text"));
	desc3->setString(meta["starDesc3"]);

	auto miniMap = dynamic_cast<ImageView *>(layer->getChildByName("BattleStop_miniMap"));
	std::string miniMapStr = BattleLayer::getInstance()->getMiniMap();
	for (auto iter = _spriteVec.begin(); iter != _spriteVec.end(); ++iter)
	{
		Sprite * sp = *iter;
		_spriteVec.erase(iter);
		iter--;
		miniMap->removeChild(sp);
	}
	//miniMap->removeAllProtectedChildren();
	if (miniMapStr == "") return;
	miniMap->loadTexture(miniMapStr, Widget::TextureResType::PLIST);
	miniMap->setContentSize(Size(miniMap->getVirtualRendererSize()));
	Size miniMapSize = Size(miniMap->getVirtualRendererSize());
	Vec2 miniMapPos = miniMap->getPosition();
	Size mapSize = BattleLayer::getInstance()->getTiledMap()->getContentSize();
	float widthPer = miniMapSize.width/ mapSize.width;
	float heightPer = miniMapSize.height / mapSize.height;
	int typeArr[] = { GROUP_TYPE_HERO, GROUP_TYPE_EVIL, GROUP_TYPE_JUST, GROUP_TYPE_NEUTRAL, GROUP_TYPE_OTHER };
	for (auto groupType : typeArr)
	{
		std::vector<Base *> baseVec = BattleLayer::getInstance()->getBaseVectorByGroupType(groupType);
		for (std::vector<Base *>::iterator iter = baseVec.begin(); iter != baseVec.end(); ++iter)
		{
			Base * it = *iter;
			if (groupType == GROUP_TYPE_HERO)
			{
				std::string miniSprite = it->getMiniSpriteNum();
				Vec2 rolePos = it->getPosition();
				Vec2 miniRolePos = Vec2(rolePos.x * widthPer, rolePos.y * heightPer);
				Sprite * roleSprite = Sprite::createWithSpriteFrameName(miniSprite);
				miniMap->addChild(roleSprite);
				_spriteVec.pushBack(roleSprite);
				roleSprite->setPosition(miniRolePos);
			}else if (it->getCurrentHp() > 0 && it->getMiniSpriteNum() !="")
			{
				std::string miniSprite = it->getMiniSpriteNum();
				Vec2 rolePos = it->getPosition();
				Vec2 miniRolePos = Vec2(rolePos.x * widthPer, rolePos.y * heightPer);
				Sprite * roleSprite = Sprite::createWithSpriteFrameName(miniSprite);
				miniMap->addChild(roleSprite);
				_spriteVec.pushBack(roleSprite);
				roleSprite->setPosition(miniRolePos);
			}
		}
	}
}