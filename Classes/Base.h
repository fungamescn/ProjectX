#ifndef __BASE_H__
#define __BASE_H__

#include "cocos2d.h"
#include "OBB2d.h"

//角色类型，注意有严格的层次顺序
enum BASE_TYPE{
	BASE_TYPE_NULL,//待废弃
	BASE_TYPE_ELEMENT,
	BASE_TYPE_FRIEND,//待废弃
	BASE_TYPE_MONSTER,
	BASE_TYPE_HERO
};

enum MATER_TYPE{
	MATER_TYPE_LIVE = 1,
	MATER_TYPE_MACHINE = 2,
	MATER_TYPE_BUILD = 3,
	MATER_TYPE_HERO = 4
};

class Base : public cocos2d::Sprite
{
public:
	cocos2d::Point factPoint; //实际坐标
	cocos2d::Point factTilePoint;//实际瓦片坐标
	cocos2d::Sprite * aimSprite;
	Base() : obb2d(nullptr){};
	virtual ~Base() = default;

	virtual int getTemplateId() = 0; //模板类型
	virtual int getBaseType(){ return BASE_TYPE_NULL; };
	/*飙血特效等(无动作)*/
	virtual void hurt() = 0;
	virtual void dead() = 0;
	virtual void doAIAction(float time) = 0;
	virtual OBB2d* getOBB2d() = 0;
	virtual cocos2d::Rect getTouchRect() = 0;
	virtual cocos2d::Point aimOffset(){ return cocos2d::Point(0, this->getTouchRect().size.height * 0.5 ); };
	virtual int& getGroupType(){ return group_type; }; //阵营
	virtual void setGroupType(const int &grouptype){ group_type = grouptype; };
	virtual int getMaterialType() = 0; //材质类型

	virtual float getCurrentHp() = 0;
	virtual void setCurrentHp(const float &hp) = 0;
	virtual float getFinalHp() = 0;
	virtual void setFinalHp(const float &hp) = 0;
	virtual float getAttack() = 0;
	virtual void setAttack(const float &attack) = 0;
	virtual float getDefense() = 0;
	virtual float getShield() = 0;
	virtual void setShield(const float &shp) = 0;
	virtual cocos2d::Point getDepthPoint() { return depthPoint; };
	virtual cocos2d::Vec2 getShadowPoint() = 0;

	virtual bool isQuest() = 0;
	virtual bool isWarn() = 0;
	virtual bool canAim(){ return true; };
	//技能子弹绑定和释放
	virtual void releaseSkill() = 0;
	virtual void retainSkill() = 0;
	virtual void releaseBullet() = 0;
	virtual void retainBullet() = 0;
	virtual void releaseAttack() = 0;
	virtual void retainAttack() = 0;
	virtual bool canRecovey() = 0;
	virtual void setSlow(bool value){ _slowing = value; };
	virtual bool getSlow(){ return _slowing; };

	virtual std::string& getMiniSpriteNum() = 0;

protected:
	OBB2d* obb2d;
	int group_type;
	int _skillCount = 0;
	int _attedCount = 0;
	int _bulletCount = 0;
	cocos2d::Point depthPoint;
	bool _slowing = false;
};

#endif