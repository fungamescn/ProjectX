#ifndef __TOUCH_LAYER_H__
#define __TOUCH_LAYER_H__

#include "cocos2d.h"

const int minSwipdistance = 100;
const int minSwiptime = 1000;    //毫秒  
const int maxClickedDis = 20;

enum E_SWIP_DIR
{
	E_INVAILD,
	E_LEFT,
	E_RIGHT,
	E_UP,
	E_DOWN
};
class TouchLayer : public cocos2d::Layer
{
public:
	TouchLayer();
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	//void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
	void updateSingleDelay(float at);
	void updateDoubleDelay(float at);
	void touchMove(float at);
	long long getCurrentTime();
	void stopSchedule();
	void initListener();

private:
	int touchCounts = 0;
	bool isMoved;
	int touchMoveCount = 0;
	cocos2d::Point touchPoint;
protected:
	virtual void onSingleCLick(cocos2d::Point touchPoint);        //单击  
	virtual void onDoubleClick(cocos2d::Point touchPoint);        //双击  
	virtual bool onClickBlack(cocos2d::Point touchPoint);
};

#endif