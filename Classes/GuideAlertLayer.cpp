#include "GuideAlertLayer.h"
#include "GuideAlertLayer.h"
#include "BattleScene.h"
#include "ui/UIButton.h"
#include "BattleLayer.h"

USING_NS_CC;

GuideAlertLayer * GuideAlertLayer::_instance = nullptr;

GuideAlertLayer * GuideAlertLayer::getInstance()
{
	if (!_instance)
	{
		_instance = GuideAlertLayer::create();
		_instance->retain();
	}
	return _instance;
}

bool GuideAlertLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto mainLayer = CSLoader::createNode("ui/GuideAlert.csb");
	this->addChild(mainLayer);

	_titleImg = static_cast<ImageView*>(mainLayer->getChildByName("GuideAlert_title"));
	_contentImg = static_cast<ImageView*>(mainLayer->getChildByName("GuideAlert_goods"));


	auto button_ok = static_cast<Button*>(mainLayer->getChildByName("GuideAlert_ok"));
	button_ok->addTouchEventListener(CC_CALLBACK_2(GuideAlertLayer::hide, this));

	auto button_cancel = static_cast<Button*>(mainLayer->getChildByName("GuideAlert_cancel"));
	button_cancel->addTouchEventListener(CC_CALLBACK_2(GuideAlertLayer::hide, this));
	return true;
}

void GuideAlertLayer::show(string title, string cont)
{
//	if (this->getParent() != NULL)
//	{
//		return;
//	}


	_titleImg->loadTexture(title + ".png", Widget::TextureResType::PLIST);
	_contentImg->loadTexture(cont + ".png", Widget::TextureResType::PLIST);
	BattleScene::getInstance()->addChild(this);

//	BattleLayer::getInstance()->setRoleEnable(false);
//	CCDirector::getInstance()->pause();
}


void GuideAlertLayer::hide(Ref* ref, Widget::TouchEventType type)
{
	if (!this->getParent())
	{
		return;
	}
	BattleScene::getInstance()->removeChild(this);
//	BattleLayer::getInstance()->setRoleEnable(true);
//	CCDirector::getInstance()->resume();
}