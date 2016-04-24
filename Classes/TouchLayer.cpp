#include "TouchLayer.h"  
#include "Utils.h"

USING_NS_CC;

TouchLayer::TouchLayer() : isMoved(false)
{
	
}

void TouchLayer::updateSingleDelay(float ft)
{
	if (touchCounts == 1) {
		onSingleCLick(touchPoint);
		touchCounts = 0;
	}

}

void TouchLayer::updateDoubleDelay(float ft)
{
	if (touchCounts == 2)
	{
		onDoubleClick(touchPoint);
		touchCounts = 0;
	}
}

void TouchLayer::onSingleCLick(cocos2d::Point touchPoint)
{

}

void TouchLayer::onDoubleClick(cocos2d::Point touchPoint)
{

}

long long TouchLayer::getCurrentTime()
{
	struct timeval tm;
	gettimeofday(&tm, NULL);
	return (long long)(tm.tv_sec * 1000 + tm.tv_usec / 1000);
}


bool TouchLayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	touchMoveCount = 0;
	if (onClickBlack(touch->getLocation()))
	{
		touchCounts = 0;
		return true;
	}
	touchPoint = touch->getLocation();
	onSingleCLick(touchPoint);
	return true;
	/*
	if (touchCounts == 1) {
		touchPoint = touch->getLocation();
		++touchCounts;
		updateDoubleDelay(0);
	}
	else if (touchCounts == 0) {
		touchPoint = touch->getLocation();
		++touchCounts;
		scheduleOnce(schedule_selector(TouchLayer::updateSingleDelay), 0.18f);
	}
	return true;
	*/
}

void TouchLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
	isMoved = true;
	touchPoint = touch->getLocation();
}

void TouchLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
	isMoved = false;
}

void TouchLayer::initListener()
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(TouchLayer::onTouchBegan, this); //[&](Touch *touch, Event *unused_event)->bool { return true; };
	listener->onTouchEnded = CC_CALLBACK_2(TouchLayer::onTouchEnded, this);
	listener->onTouchMoved = CC_CALLBACK_2(TouchLayer::onTouchMoved, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool TouchLayer::onClickBlack(cocos2d::Point touchPoint)
{
	return false;
}

void TouchLayer::touchMove(float at)
{
	if (!isMoved) return;
	if (!Utils::checkDelayFrame(touchMoveCount, 10)) return;
	CCLOG("touchPoint%f, %f", touchPoint.x, touchPoint.y);
	if (!onClickBlack(touchPoint))
	{
		onSingleCLick(touchPoint);
	}
}