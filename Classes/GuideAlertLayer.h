#ifndef __GUIDEALERTLAYER_H__
#define __GUIDEALERTLAYER_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

class GuideAlertLayer : public Layer
{
public:
	GuideAlertLayer(){};
	~GuideAlertLayer(){};
	CREATE_FUNC(GuideAlertLayer);
	virtual bool init() override;
	static GuideAlertLayer * getInstance();
	void show(string title, string cont);
	void hide(Ref* ref, Widget::TouchEventType type);

private:
	static GuideAlertLayer * _instance;
	ImageView * _titleImg;
	ImageView * _contentImg;
};

#endif