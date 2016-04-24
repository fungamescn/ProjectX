/*战斗图层*/
#ifndef __BATTLE_LAYER_H__
#define __BATTLE_LAYER_H__

#include "cocos2d.h"
#include "TouchLayer.h"
#include "Role.h"
#include "Hero.h"
#include "AIManager.h"
#include "EventManager.h"
#include "Element.h"
#include "BattleItem.h"

class BattleLayer : public TouchLayer
{
public:
	BattleLayer();
	~BattleLayer();
	static BattleLayer* getInstance();
	CREATE_FUNC(BattleLayer);
	virtual bool init();
	void initStage(int level);//初始化关卡
	void cleanUp(int stage);
	//OPENGL坐标与瓦片坐标之间的转换
	cocos2d::Point tileCoordForPosition(cocos2d::Point position);
	cocos2d::Point positionForTileCoord(cocos2d::Point tileCoord);
	cocos2d::Sprite * getTiledMap();
	bool cleanOldChild(int level);
	void gameStart();
	void gameOver(int result);
	void initElements(int stage);
	void initTeam(std::vector<int> vec);
	void adjustTeamPos(int x,int y);
	//OPENGL坐标转遮罩坐标
	cocos2d::Point nodeForPosition(cocos2d::Point position);
	cocos2d::Point positionForNode(cocos2d::Point node);

	int getWeightDir(Point position, Point touchLocation);
	//增加单位
	void addBase(Base * base);
	//删除单位
	void removeBase(Base* base);
	//单位死亡处理
	void onBaseDead(Base* base);

	cocos2d::Vec2 getNearVec2(cocos2d::Vec2 position, cocos2d::Vec2 position2, vector<cocos2d::Vec2> vec);

	//获取角色集合
	std::vector<Base *> getBasesList();
	//获取怪物id隐射表
	std::vector<Base *> getMonstersByTid(string tid){ return monsterMap[tid]; };
	//获取物件列表
	std::vector<Base *> getElementsByGroupId(string groupId){ return elementMap[groupId]; };

	std::vector<Base *>& getBaseVectorByGroupType(int groupType);
	//设置强制攻击目标
	CC_SYNTHESIZE(Base *, _forceRole, ForceRole);

	EventManager * getEventManager(){ return _eventManager; };

	void createOneElement(int tid, int groupType, cocos2d::Point &point);
	void createElements(int tid, int count, int groupType, std::vector<cocos2d::Point> &points);
	void createElements(int tid, int count, int groupType, int randomId);
	void removeElement(Element * element);
	Role* createOneMonster(int tid, int groupType, cocos2d::Point point, bool isTiledPos = true);
	void createMonsters(int tid, int count, int groupType, std::vector<cocos2d::Point> &points);
	void createMonsters(int tid, int count, int groupType, int randomId);

	void createBattleItem(int tid ,cocos2d::Point point,bool isTile = true);
	void removeBattleItem(BattleItem* battleItem);

	void calcTilePointBoundary(cocos2d::Point &p);//瓦片坐标边界处理
	void callBackPlayer(Hero * hero);
	void areaChecking(float dt);
	bool isHeroAllDied();
	Vec2 getPositionForIsoAt(const Vec2& pos);
	void slowPlaySpeed();
	void resumePlaySpeed();
	void startShake();
	float getBattleTime(){ return _battleTime; };
	void refreshHireHeros();
    void refreshEnemy(float dt);
	void upMoneySprite(int num);
	void addSkillItemById(int itemId, int num);
	int minusSkillItem(int itemId, int num);
	int getSkillItemNum(int itemid);
	void refreshHeroProp(int roleId);
	std::string getMiniMap();
	bool getRoleEnable(){ return _roleEnable; };
	void setRoleEnable(bool enable){ _roleEnable = enable; };
public:
	std::vector<Base *> heroVec;//英雄集合
	std::vector<Base *> justVec;//正义集合
	std::vector<Base *> evilVec;//邪恶集合
	std::vector<Base *> neutralVec; //中立集合
	std::vector<Base *> otherVec; //中立集合

	std::map<int, int> skillItemMap;
private:
	virtual void update(float t);  //定时任务
	void updateViewZOrder(); //更新layer中role的叠放次序
	void updateMonsterAction(float t);
	void updateViewPointCenter(bool soon = false);	//设置屏幕中心点坐标位置
	void updateViewPointCenter2(bool soon = false);
	void updateViewPointCenterCallBack();
	void updateWarnSprite();
	void updateTouchPointVisible();
	void updateCeateMonsterFromQueue();
	void updateTimeScale();
	void shakeUpdate();

private:
	static BattleLayer* _instance;
	int _stageId;
	int _zoderUpdate = 0;
	int _updateWarnSpriteCount = 0;
	int _updateTouchWarn = 0;
	int _updateCreateMonster = 0;
	int _updateCreateMonsterMax = 1;
	int _updateAreaChecking = 0;
	//cocos2d::TMXTiledMap * _tileMap; //瓦片地图
    cocos2d::Sprite * _tileMap;
	std::vector<Base *> roleArray; //场上活着的role集合（包括hero和monster），主要参与深度排序
	std::map<string, std::vector<Base*>> monsterMap;
	std::map<string, std::vector<Base*>> elementMap;
	std::list<Base *> elementZOrderList;
	AIManager* _aiMagager;
	EventManager * _eventManager;
	bool moveFlag = false;
	cocos2d::Vec2 lastHeroPos;
	//cocos2d::Sprite * _aimSprite;
	cocos2d::Sprite * _touchSprite;
	float _diagonalAngle;
	cocos2d::Vec2 getWarnSpriteDev(int& angle);
	int _slowPlayCount = 0;//慢动作播放请求次数
	int _shakeFactor = -1;
	int _shakeDelay = 0;
	cocos2d::Vec2 _shakeOriginalPos;
	float _battleTime;
	int _soundId;
	bool _roleEnable = true;
protected:
	void setHeroDistancePoint(cocos2d::Point touchLocation);
	virtual void onSingleCLick(cocos2d::Point touchPoint) override;        //单击  
	virtual void onDoubleClick(cocos2d::Point touchPoint) override;        //双击  
	virtual bool onClickBlack(cocos2d::Point touchPoint) override;

	void createStageTiels();
};

#endif