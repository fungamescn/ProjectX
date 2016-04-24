#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "Base.h"
#include "Config.h"
#include "HpBarMonster.h"

USING_NS_CC;

typedef cocos2d::Vector<cocos2d::SpriteFrame*> FRAME_VECTOR;

enum ELEMENT_DEAD_ACTION
{
	ELEMENT_DEAD_AREA_DAMAGE = 1
};

struct ElementModel
{
	int id;
	std::string name;
	std::string res;
	int materialType;
	int shadowX;
	int shadowY;
	int showHpBar;
	int hpY;
	int selectX;
	int selectY;
	int touchCenterX;
	int touchCenterY;
	int touchWidth;
	int touchHeight;
	std::vector<float> depthSP;//深度起点(相对的瓦片坐标)
	std::vector<float> depthEP;//深度终点(相对的瓦片坐标)
	std::vector<int> block;
	std::vector<Vec2> collison;
	float selfScale;
	float bombScale;
	std::string sound;
};

struct ElementData
{
	ElementModel* model;
	std::string groupId;
	std::string name;
	std::string stage;
	int sceneX;
	int sceneY;
	int hp;
	int maxHp;
	float attack;
	int canDestroy;
	bool isQuest;
	bool isWarn;
	int offline;
	std::string offlineParam1;
	std::string offlineParam2;
	std::string offlineParam3;
	bool canAttacked;
	int modelId;
	int dropPlan;
	int isSlow;
	int isShock;
	std::string miniSprite;
};

class Element : public Base
{
public:
	CREATE_FUNC(Element);
	bool init();
	static int idBuilder();
	ElementData* data;
	int getId();
	std::string getGroupId();
	void setElementData(const int &tid, int &groupType, Point &point);
	void setState(std::string state);
	virtual int getBaseType() override{ return BASE_TYPE_ELEMENT; };
	virtual Point aimOffset() override;
	virtual void hurt() override;
	virtual void dead() override;
	virtual void doAIAction(float time) override;
	virtual OBB2d* getOBB2d() override;
	virtual int getTemplateId() override;
	virtual float getCurrentHp() override;
	virtual void setCurrentHp(const float &hp) override;
	virtual float getFinalHp() override;
	virtual void setFinalHp(const float &hp) override;
	virtual float getAttack() override;
	void setAttack(const float &attack) override;
	virtual float getDefense() override;
	virtual float getShield() override;
	virtual void setShield(const float &shp) override;
	virtual cocos2d::Rect getTouchRect() override;
	virtual int getMaterialType() override;
	virtual bool isQuest() override;
	virtual bool isWarn() override;
	virtual bool canAim(){ return data->canAttacked; };
	virtual cocos2d::Vec2 getShadowPoint();

	virtual void releaseSkill() override;
	virtual void retainSkill() override;
	virtual void releaseBullet() override;
	virtual void retainBullet() override;
	virtual void releaseAttack() override;
	virtual void retainAttack() override;
	virtual bool canRecovey() override;
	virtual std::string& getMiniSpriteNum() override;
public:
	std::vector<Vec2> movePoint;
private:
	static int idCount;
	int _id;
	int _tid;
#if DEBUG_MODE
	DrawNode* _drawNode;
#endif
	std::string _state;
	cocos2d::Sprite* _tipPart = nullptr;
	cocos2d::Sprite* _normalBody = nullptr;
	cocos2d::Sprite* _deadBody = nullptr;
	HpBarMonster* _hpBar = nullptr;
	 
	void deadActionCall();
	void deadAction();
};
#endif