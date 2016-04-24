#ifndef __LEVELINFO_LAYER_H__
#define __LEVELINFO_LAYER_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"

enum
{
	LEVEL_INFO_TAG = 1,
	CONTINUE_ZOOM_TAG,
	BACK_ZOOM_TAG
};

class LevelInfoLayer : public cocos2d::Layer
{
public:
	CREATE_FUNC(LevelInfoLayer);
	virtual bool init() override;
	LevelInfoLayer();
	~LevelInfoLayer();
	void updateView(bool initialize = false);
	void updateCurrency();

private:
	//static const int PROPERTY_MAX_LVL;
	static const float SCALE_BEGIN_DELAY;
	static const float SCALE_END_DELAY;

	int _currentPage;
	void updateShopItem();
	void updateProperty(const int &propertyId);
	void pageHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void backHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void continueHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void buyHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void currencyHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void levelUpHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void levelMaxHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void giftHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
};

#endif