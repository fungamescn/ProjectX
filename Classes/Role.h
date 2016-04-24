#ifndef __ROLE_H__
#define __ROLE_H__

#include <list>
#include "cocos2d.h"
#include "Base.h"
#include "RoleAI.h"
#include "Attribute.h"
#include "HpBarBase.h"
#include "Config.h"
#include "cocostudio/CocoStudio.h"
using namespace cocostudio;
enum ROLE_STATE
{
	ROLE_STATE_STAND,
	ROLE_STATE_MOVE,
	ROLE_STATE_ATTAK,
	ROLE_STATE_CONJURE,
	ROLE_STATE_MOVE_ATTACK,
	ROLE_STATE_MOVE_CONJURE,
	ROLE_STATE_HURT,
	ROLE_STATE_DEAD,
};

enum ACTION_TAG
{
	ACTION_TAG_SHIELD = 1,
	ACTION_TAG_FIRE,
	ACTION_TAG_HURT
};

//人物,怪物基类
class Role : public Base, public RoleAI
{
public:
	bool isLeader = false;

	static Role* create(const int &roleId, const int &groupType);

	Role(const int &roleId);
	~Role();
	virtual bool init();
	void createSpriteChilds();
	void initSpriteChilds();
	virtual void refreshProp(int roleId);
	void setDirection(const int &direction, bool update = false);
	void stand();
	void move();
	void attack();
	void conjure();
	void moveAttack(bool retreat = false);
	void moveConjure(bool retreat = false);
	void beAttacked();
	virtual void hurt() override;
	virtual void dead() override;
	void deadEndCall();
	int getRoleId() const;
	std::list<cocos2d::Point> findPath(cocos2d::Point destination);
	virtual void doFindPath(cocos2d::Point destination);//执行AI寻路
	virtual void doAIAction(float del) override;//执行AI逻辑
	virtual int getBaseType() override{ return BASE_TYPE_MONSTER; };
	virtual void update(float delta) override;
	void setRolePosition(cocos2d::Point position);
	cocos2d::Vec2 getShootPoint();
	
	Attribute* getAttribute(){ return _attribute; };
	virtual OBB2d* getOBB2d() override;
	virtual cocos2d::Rect getTouchRect() override;
	void clearFindPathList();
	virtual int getTemplateId() override;
	float getSpeed();
	virtual int getMaterialType() override;

	virtual float getCurrentHp() override;
	virtual void setCurrentHp(const float &hp) override;
	virtual float getFinalHp() override;
	virtual void setFinalHp(const float &hp) override;
	virtual float getAttack() override;
	virtual void setAttack(const float &attack) override;
	virtual float getDefense() override;
	virtual float getShield() override;
	virtual void setShield(const float &shp) override;
	virtual bool isQuest() override;
	virtual bool isWarn() override;
	virtual cocos2d::Vec2 getShadowPoint() { return this->getPosition(); };
	virtual void hideHpBar(){ hpBar->setVisible(false); };

	virtual void createRole(const int &roleId, const int &groupType, cocos2d::Point &point, bool isTiledPos = true);
	virtual void releaseSkill() override;
	virtual void retainSkill() override;
	virtual void releaseBullet() override;
	virtual void retainBullet() override;
	virtual void releaseAttack() override;
	virtual void retainAttack() override;
	virtual bool canRecovey() override;
	virtual void setGroupType(const int &grouptype) override;
	virtual std::string& getMiniSpriteNum() override;

private:
	int _roleId;//角色 id
	//0:待机, 1:移动, 2:原地攻击, 3:原地施法, 4:移动攻击, 5:移动施法, 6:受击, 7:死亡
	int _roleState = 0;//角色状态
	int _direction = 0;//上半身方向
	float _shootDelta = 0;//子弹时间统计
	bool _isRetreat = false;//是否后退
	bool _directChanged;//是否改变了方向(用于重复调动作判断)
	bool _useSkill = false;
	int _skillNum = 0;
	int _bulletNum = 0;
	int _aiActionCount = 0;
	Base * _oldRole = NULL; //锁定目标
	cocos2d::Sprite* _shadowPart = nullptr;//阴影
	cocos2d::Sprite* _lowerPart = nullptr;//下半身
	cocos2d::Sprite* _upperPart = nullptr;//上半身
	cocos2d::Sprite* _specialPart = nullptr;//特殊部分(死亡动作等...)
	cocos2d::Sprite* _firePart = nullptr;//开火特效
	cocos2d::Sprite* _shieldPart = nullptr;//护盾
	cocos2d::Sprite* _tipPart = nullptr;
	std::vector<cocos2d::Sprite*> _segmentSprites;
    Armature *armature = NULL;
    
    
	cocos2d::Vec2 _destVec;
	cocos2d::Vec2 _attackVec;
	Attribute* _attribute;
	cocos2d::Point _oldNodePoint;
#if DEBUG_MODE
	cocos2d::DrawNode* _drawNode;
#endif
	void changeState(const int &state);
	void playSegment(const unsigned int &segmentId, const int &repeat = 0, bool reverse = false, cocos2d::CallFunc* endCall = nullptr);
	void stopSegment(const int &segmentId);
	cocos2d::Vec2 getNextdestVec();
	void moveUpdate();
	void attackUpdate(float delta);
	void moveAttackUpdate(float delta);
	void conjureUpdate();
	void moveConjureUpdate();
	void updateFireEffect();
	void updateShield();
	void attackHurt(Base* enemey, float delta);
protected:
	const int ROLE_SEGMENTS = 3; //0:"upper", 1 : "lower", 2 : "special", 3 : "shadow"
	//0:待机, 1:移动, 2:受击, 3:死亡, 4攻击, 5:魔法
	static const std::vector<std::vector<int>> ACTION_DICT;//动作表
	static const std::vector<std::vector<int>> STATE_DICT;//状态表
	static int roleTag;
	std::string roleType = "monster";//类型前缀
	int totalDirs = 8;//总方向(一半有资源)
	int kind;//类型：生物、机械、建筑
	int movefps;
	int actionUpdateCount = 0;//action update次数
	std::list<cocos2d::Point> findPathList;  //寻路坐标集合
	HpBarBase* hpBar = nullptr;//血条

	virtual int calRoleState(); //计算本次RoleState的状态
	//设置和获取roleState
	int& getRoleState(){ return _roleState; };
	void setRoleState(const int &roleState){ _roleState = roleState; };
	Base * getOldRole();
	void setOldRole(Base * Role);
	cocos2d::Point getNearNode(Base * role);
	virtual void setAIAttTarget() override;//设定AI锁定目标
	virtual void createAI(int aiid) override;
	virtual Base * getTargetAI() override;
	virtual Base * getFollowAI() override;
	virtual Base * getSkillTargetAI() override;
	virtual int getToPositiontDir(const cocos2d::Vec2& dest, bool isAttack = false);
	virtual void initHpBar();
	Base * getRecentEnemyByGroupType(const int groupType, float skillDistance, bool useIds);
	Base * getFollowEnemyByGroupType(const int groupType, bool useIds);
	void deadEvent();
	void changeSkillState();
	void hurtEffect();
};
#endif