#ifndef __STAGELAYER_H__
#define __STAGELAYER_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "WarnSprite.h"
#include <ui/UIButton.h>
#include "Config.h"
#include "ui/UILayout.h"
#include "Hero.h"
#include "ui/UIText.h"

#if DEBUG_MODE
#include "ui/UIText.h"
#endif

class StageLayer : public cocos2d::Layer
{
public:
	StageLayer();
	~StageLayer() = default;
	CREATE_FUNC(StageLayer);
	virtual bool init() override;
	static StageLayer* getInstance();
	virtual void update(float delta) override;
	WarnSprite * getDeadWarnSprite(){ return _warnDeadSprite; };
	WarnSprite * getQuestWarnSprite(){ return _warnQuestSprite; };
	WarnSprite * getEnemyWarnSprite(){ return _warnEnemySprite; };
	void createWarnSprites();
	void addBuffSprite(const int &buffId);
	cocos2d::Node* getBuffSprite(const int &buffId);
	void removeBuffSprite(const int &buffId);
	void initSkills();
	void updateHeal(Hero* hero);
	void stopSkillTime();
	void startWhiteSmoke();
	void clearAll();
	void startClock();
	void stopClock();
	void updateSkillItem();
#if DEBUG_MODE
	void showDebugLog(string logStr);
#endif
private:
	static const int BUFF_GAP = 50;
	static const int MOVE_BY = 420;
	static const int SCALE_DELAY = 600;
	static const float MOVE_DELAY;
	static const std::string HEAL;
	static const std::string ROCKET;
	static const std::string GATLIN;
	static const std::string NUCLEAR;
	static const std::unordered_map<std::string, int> BUTTON_MAP;
	static const std::unordered_map<std::string, int> GIFT_MAP;
	static const std::unordered_map<int, int> ITEM_MAP;

	static StageLayer* _instance;
	int _scaleDelay;
	std::string _buttonName;
	cocos2d::Node* _battleUI = nullptr;
	cocos2d::Node* _buffUI = nullptr;
	WarnSprite * _warnDeadSprite = nullptr;
	WarnSprite * _warnQuestSprite = nullptr;
	WarnSprite * _warnEnemySprite = nullptr;
	cocos2d::LayerColor* _smokeLayer = nullptr;
	cocos2d::ui::Layout* _heroLayout = nullptr;
	cocos2d::EventListenerTouchOneByOne* _listener = nullptr;
	bool checkSkillOption(cocos2d::ui::Button* button);
	void touchHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void stopHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void healHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void stageHandler(cocos2d::Touch* touch, cocos2d::Event* event);
	void updateClock(float at);
#if DEBUG_MODE
	cocos2d::ui::Text * debugText;
	std::queue<string> debugContentArr;
#endif
//	void showTypewriterFont(float at);
};

#endif