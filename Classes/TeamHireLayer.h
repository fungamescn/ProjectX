#ifndef __TEAM_HIRE_H__
#define __TEAM_HIRE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"

class TeamHireLayer : public cocos2d::Layer
{
public:
	TeamHireLayer();
	~TeamHireLayer();
	virtual bool init();
	CREATE_FUNC(TeamHireLayer);

	cocos2d::Layer * getTeamHireLayer();
	std::vector<int>& getSoldierVec();
	void refreshUI();
	void updateMoney();

public:
	cocos2d::Layer * teamHireLayer;

private:
	void teamHire_prePage(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_nextPage(cocos2d::Object* object, cocos2d::ui::TouchEventType type);

	void loadTeamHireUI();
	void teamHire_hire(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_fire(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_unlock(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_up(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_upMax(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_back(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_battleStart(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void teamHire_addMoney(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void myTeam_touchHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type);

	void giftHandler(cocos2d::Object* object, cocos2d::ui::TouchEventType type);

	void refreshHireHeroVec();
	void refreshTeamHireUI();
	void refreshHeroHireUI();
	void refreshHeroLevelUI(int num);
	void refreshHeroHireAndLockUI(int num);
	void refreshGiftBtn();
	void updateTeamHirePage();

private:
	std::vector<int> soldierVec;
	float teamHireLayer_originalScale_0 = 0;
	float teamHireLayer_originalScale_1 = 0;
};

#endif