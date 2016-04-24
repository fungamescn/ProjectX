#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include "cocos2d.h"
#include "Base.h"

enum
{
	PROP_TYPE_ATTACK = 1,
	PROP_TYPE_SPEED,
	PROP_TYPE_DEFENSE,
	PROP_TYPE_FINAL_HP
};

struct ArrowData
{
	std::string res;
	int yPos;
	int distance;
	float time;
};

struct TailData
{
	float fade;
	float stroke;
	cocos2d::Color3B color3b;
};

class Attribute
{
public:
	Attribute(Base* base);
	~Attribute();
	void applyScaling();
	static const float RUN_MULTIPLE;

	int& getLevel();
	float getSpeed();
	float getAttack();
	void setAttack(const float& attack);
	float getDefense();
	float& getShotDistance();
	int& getOffsetY();
	int& getHpY();
	float& getFireRate();
	int& getAccuracy();
	float& getBulletSpeed();
	float& getBulletDur();
	int& getMaxHit();
	int& getRangeWidth();
	int& getRangeHeight();
	int& getMaterialType();
	int& getAttGroup();
	int& getRestrictLive();
	int& getRestrictMachine();
	int& getRestrictBuild();
	int& getIsDeath();
	bool& isUseShield();
	bool& isWarn();
	bool& isQuest();
	float& getBulletRepaet();
	std::string& getBulletEffect();
	std::string& getFireEffect();
	std::string& getHitEffect(const int &index);
	std::string& getDeadSound();
	std::string& getBulletSound();
	float getBornXByDir(const int &dir);
	float getBornYByDir(const int &dir);
	cocos2d::Size& getRoleObb();
	cocos2d::Vec2& getBulletOrigin();
	cocos2d::Size& getBuletObb();
	float& getCurrentHp();
	//@return +hp:1, -hp:-1, else:0
	int setCurrentHp(float hp);
	float& getShield();
	void setShield(const float &shp);
	float& getFinalShield();
	float getFinalHp();
	void setFinalHp(const float &hp);
	int& getAiid();
	int& getResId();
	int& getBombRadius();
	int& getAttackType();
	int& getBulletId();
	int& getOffline();
	float& getScaling();
	std::vector<int>& getOffParam();
	ArrowData* getArrowData();
	TailData* getTailData();
	int& getDropPlan();
	int& isSlow();
	int& isShock();
	std::string& getMiniSprite();
private:
	Base* _base;
	int _resId;
	int _level;
	float _currentHp;
	float _finalHp;
	int _speed;
	float _attack;
	float _shield;
	float _finalShield;
	float _defense;
	float _shotDistance;
	int _bulletId;
	float _scaling;

	int _offsetY;
	int _hpY;
	float _fireRate;
	int _accuracy;
	float _bulletSpeed;
	float _bulletDur;
	int _maxHit;
	int _materialType; //材质类型
	int _restrictLive;
	int _restrictMachine;
	int _restrictBuild;
	bool _useShield;
	bool _isQuest;
	bool _isWarn;
	int _attGroup;
	float _bulletRepeat;
	int _bombRadius;
	int _attackType;//0近战, 1子弹
	int _offline;
	int _isDeath;
	std::vector<int> _offParam;
	std::string _bulletEffect;
	std::string _fireEffect;
	std::string _sound;
	std::string _bulletSound;
	std::vector<std::string> _hitEffect;
	std::vector<float> _bornXVec;
	std::vector<float> _bornYVec;
	cocos2d::Size _roleObb;
	cocos2d::Vec2 _bulletOrigin;
	cocos2d::Size _bulletObb;
	int _aiid;
	int _dropPlan = 0;

	ArrowData* _arrowData;
	TailData* _tailData;

	int _isSlow = 0;
	int _isShock = 0;
	std::string _miniSprite;
};

#endif