#ifndef __POPUPLAYER_H__
#define __POPUPLAYER_H__

#include "cocos2d.h"

class PopupLayer : public cocos2d::Layer
{
public:
	PopupLayer();
	~PopupLayer();
	static PopupLayer* getInstance();
	CREATE_FUNC(PopupLayer);
	virtual bool init() override;
	void initWithLayers(cocos2d::Layer* layer, ...);
	void popup(const int &layer);
	void hide();
	void refresh();
private:
	static PopupLayer* _instance;
	static const int POPUP_NULL = -1;
	int _enabledLayer;
	cocos2d::Vector<cocos2d::Layer*> _layers;
	cocos2d::EventListenerTouchOneByOne* _listener;
	cocos2d::Vector<cocos2d::Sprite *> _spriteVec;
	void refreshWinLayer();
	void refreshLoseLayer();
	void refreshStopLayer();
};

#endif