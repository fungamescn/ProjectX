#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "LevelInfoLayer.h"
#include "TeamHireLayer.h"
#include "NoticeLayer.h"

class BattleScene : public cocos2d::Scene
{
public:
	static BattleScene* getInstance();
	virtual bool init();
	CREATE_FUNC(BattleScene);
	void loadAsset();
	void startGame(float at);

	void loadGameStartUI();
	void loadLevelSelectionUI();

	void refreshGameStartUI();
	void refreshLevelSelectionUI();

	void prestrainLoad();
	void callBackPrestrainLoad();
	void callBackPrestrainSpritePlist(std::string str);
	void callBackPrestrainAnimationPlist(std::string str);
	void switchLayer(unsigned const int &LayerIndex);
	int getStageLevel();
	void btnUpEffect(int toLayer);
	void updateMoney();
	void unLockNextStage(int stage);

	void refreshUI(int pageIndex);
	void updateAll();
	void allResurrection();
private:
	static BattleScene* _instance;
	int selectMainStage = 0;
	int selectSubStage = 0;
	int maxStage = 0;
	int _loadNum;
	int _loadMaxNum;
	std::vector<int> soldierVec;

	cocos2d::LayerMultiplex * layerMultiplex;
	cocos2d::Layer * gameStartLayer;
	cocos2d::Layer * levelSelectionLayer;
	LevelInfoLayer* levelInfoLayer;
	TeamHireLayer * teamHireLayer;
	cocos2d::Layer * LoadingLayer;
	

//	GameStartLayer * gameStartLayer;
	float gameStartLayer_originalScale_0 = 0;
	float gameStartLayer_originalScale_1 = 0;
	float levelSelectionLayer_originalScale_0 = 0;
	float levelSelectionLayer_originalScale_1 = 0;
	float levelSelectionLayer_originalScale_2 = 0;

	void gameStart_start(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void gameStart_gift(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void gameStart_exit(cocos2d::Object* object, cocos2d::ui::TouchEventType type);

	void levelSelection_bigSelect(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void levelSelection_select(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void levelSelection_back(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void levelSelection_continue(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void levelSelection_gift(cocos2d::Object* object, cocos2d::ui::TouchEventType type);
	void levelSelection_pre(cocos2d::Object* object, cocos2d::ui::Widget::TouchEventType type);
	void levelSelection_next(cocos2d::Object* object, cocos2d::ui::Widget::TouchEventType type);

	void initPopupLayer();
	void quitHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void resumeHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void replayHandler(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void winGoHander(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
	void loseGoHander(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);

	void initNoticeLayer();

	void refreshStopLayer();

protected:
	void changeLoadingAngle(float at);
	std::string owner = "杭州泛城科技有限公司@魔力组  @funcity";
};

#endif