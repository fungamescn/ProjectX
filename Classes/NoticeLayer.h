#ifndef __NOTICE_LAYER_H__
#define __NOTICE_LAYER_H__

#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/CocosGUI.h"


USING_NS_CC;

class NoticeLayer:public Layer
{
public:
	NoticeLayer(){};
	~NoticeLayer(){};
	CREATE_FUNC(NoticeLayer);
	virtual bool init() override;
	static NoticeLayer * getInstance();

	void show(std::string str);
private:
	static NoticeLayer* _instance;
	Label* _text;
};
#endif