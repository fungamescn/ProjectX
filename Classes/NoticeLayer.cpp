#include "NoticeLayer.h"

USING_NS_CC;

NoticeLayer* NoticeLayer::_instance = nullptr;

bool NoticeLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	auto winSize = CCDirector::getInstance()->getWinSize();
	this->setPositionX(winSize.width / 2);
	this->setPositionY(winSize.height / 2 + 100);

	_text = Label::createWithSystemFont("", "DroidSerif-Bold", 40);
	_text->setColor(Color3B::YELLOW);
	_text->enableShadow(Color4B::BLACK);
	this->addChild(_text);
	return true;
}

NoticeLayer * NoticeLayer::getInstance()
{
	if (!_instance)
	{
		_instance = NoticeLayer::create();
	}
	return _instance;
}

void NoticeLayer::show(std::string str)
{
	_text->setString(str);
	_text->setOpacity(255);
	_text->stopAllActions();
	_text->runAction(FadeOut::create(6));
}