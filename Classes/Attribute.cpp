#include "Attribute.h"
#include "Template.h"
#include "Utils.h"
#include "Config.h"
#include "BuffLogic.h"
#include "GameData.h"
USING_NS_CC;

const float Attribute::RUN_MULTIPLE = 0.5f;

Attribute::Attribute(Base* base) : 
_base(base), 
_resId(0),
_level(1),
_currentHp(0.0f),
_finalHp(0.0f),
_speed(0),
_attack(0.0f),
_shield(0.0f),
_finalShield(0.0f),
_defense(0.0f),
_shotDistance(0.0f),
_bulletId(0),
_scaling(1.0f),
_offsetY(0),
_hpY(0),
_fireRate(0.0f),
_accuracy(0),
_bulletSpeed(0.0f),
_bulletDur(0.0f),
_maxHit(0),
_materialType(0),
_restrictLive(0),
_restrictMachine(0),
_restrictBuild(0),
_useShield(false),
_isQuest(false),
_isWarn(false),
_attGroup(0),
_bulletRepeat(0),
_bombRadius(0),
_attackType(1),
_offline(0),
_isDeath(0),
_aiid(0),
_dropPlan(0),
_isSlow(0),
_isShock(0),
_bulletEffect(""),
_fireEffect(""),
_sound(""),
_bulletSound(""),
_arrowData(nullptr),
_tailData(nullptr)
{
	_base->retain();
	auto baseType = _base->getBaseType();
	auto metaId = _base->getTemplateId();
	auto metaIndex = baseType == BASE_TYPE_HERO ? TEMPLATE_HERO : TEMPLATE_MONSTER;
	auto roleMeta = Template::getInstance()->getMetaById(metaIndex, metaId);
	_resId = atoi(roleMeta["resId"].c_str());
	_offsetY = atoi(roleMeta["offsetY"].c_str());
	auto roleObb = Utils::str_split(roleMeta["Obb"], "|");
	_roleObb = Size(atoi(roleObb[0].c_str()), atoi(roleObb[1].c_str()));
	_bornXVec = Utils::str_split_f(roleMeta["bornX"], "|");
	_bornYVec = Utils::str_split_f(roleMeta["bornY"], "|");
	_bulletId = atoi(roleMeta["bulletId"].c_str());
	_sound = roleMeta["sound"];
	if (baseType == BASE_TYPE_HERO)
	{
		_miniSprite = "miniMap_hero.png";
		_materialType = MATER_TYPE_HERO;
		std::string levelValue = GameData::getInstance()->getGameDataForKey(metaId, GD_TEAM_LEVEL);
		int level = (levelValue == "") ? 0 : Utils::string2Int(levelValue);
		META_LINE keys = { { "level", Utils::int2String(level) }, { "heroId", Utils::int2String(metaId) } };
		roleMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_LEVEL, keys);
		_restrictLive = atoi(roleMeta["restrictLive"].c_str());
		_restrictMachine = atoi(roleMeta["restrictMachine"].c_str());
		_restrictBuild = atoi(roleMeta["restrictBuild"].c_str());
	}
	else
	{
		_miniSprite = roleMeta["miniSprite"];
		_materialType = atoi(roleMeta["materialType"].c_str());
		if (baseType == BASE_TYPE_MONSTER)
		{
			_hpY = atoi(roleMeta["hpY"].c_str());
			_aiid = atoi(roleMeta["aiid"].c_str());
			_isQuest = atoi(roleMeta["isQuest"].c_str()) == 0 ? false : true;
			_isWarn = atoi(roleMeta["isWarn"].c_str()) == 0 ? false : true;
			_offline = atoi(roleMeta["offline"].c_str());
			_scaling = atof(roleMeta["scaling"].c_str());
			_offParam = { atoi(roleMeta["params1"].c_str()), atoi(roleMeta["params2"].c_str()) };
			if (!roleMeta["arrowRes"].empty())
			{
				_arrowData = new ArrowData();
				_arrowData->res = roleMeta["arrowRes"];
				_arrowData->yPos = atoi(roleMeta["arrowPosY"].c_str());
				_arrowData->distance = GUIDE_ARROW_DISTANCE;
				_arrowData->time = GUIDE_ARROW_TIME;
			}
		}
		_isDeath = atoi(roleMeta["isDeath"].c_str());
		_dropPlan = Utils::string2Int(roleMeta["dropPlan"]);
		_isSlow = Utils::string2Int(roleMeta["isSlow"]);
		_isShock = Utils::string2Int(roleMeta["isShock"]);
	}
	_finalHp = atof(roleMeta["hp"].c_str());
	_speed = atoi(roleMeta["speed"].c_str());
	_attack = atof(roleMeta["attack"].c_str());
	_defense = atof(roleMeta["defense"].c_str());
	_shotDistance = atof(roleMeta["shotDistance"].c_str());
	_finalShield = _shield = atof(roleMeta["shield"].c_str());
	if (baseType == BASE_TYPE_HERO)
	{
		for (int i = 1; i <= 4; ++i)
		{
			auto idStr = Utils::int2String(i);
			auto propertyStr = GameData::getInstance()->getGameDataForKey(i, GD_PROPERTY);
			if (propertyStr.empty())
			{
				continue;
			}
			auto propertyLvl = Utils::string2Int(propertyStr);
			META_LINE keys = { { "skillId", idStr }, { "level", propertyStr } };
			auto propertyMeta = Template::getInstance()->getMetaByKeys(TEMPLATE_PROPERTY, keys);
			auto propertyType = atoi(propertyMeta["type"].c_str());
			auto percent = 1.0f + atof(propertyMeta["per"].c_str());
			switch (propertyType)
			{
			case PROP_TYPE_ATTACK:
				_attack *= percent;
				break;
			case PROP_TYPE_SPEED:
				_speed *= percent;
				break;
			case PROP_TYPE_DEFENSE:
				_defense *= percent;
				break;
			case PROP_TYPE_FINAL_HP:
				_finalHp *= percent;
				break;
			default:
				break;
			}
		}
	}
	_currentHp = this->getFinalHp();
	if (_bulletId == 0)
	{
		return;
	}
	META_LINE bulletMeta = Template::getInstance()->getMetaById(TEMPLATE_BULLET, _bulletId);
	_attackType = atoi(bulletMeta["type"].c_str());
	_fireRate = atof(bulletMeta["fireRate"].c_str());
	for (int i = 1; i <= 3; ++i)
	{
		auto effectName = StringUtils::format("hitEffect%d", i);
		_hitEffect.push_back(bulletMeta[effectName]);
	}
	_attGroup = atoi(bulletMeta["attGroup"].c_str());
	_useShield = atoi(bulletMeta["useShield"].c_str()) ? true : false;
	if (_attackType == 0)
	{
		return;
	}
	_accuracy = atoi(bulletMeta["accuracy"].c_str());
	_bulletSpeed = atof(bulletMeta["bulletSpeed"].c_str());
	_bulletDur = atof(bulletMeta["bulletDur"].c_str());
	_maxHit = atoi(bulletMeta["maxHit"].c_str());
	_fireEffect = bulletMeta["fireEffect"];
	_bulletRepeat = atof(bulletMeta["repeat"].c_str());
	_bulletEffect = bulletMeta["bulletEffect"];
	_bulletSound = bulletMeta["sound"];
	auto bulletOrigin = Utils::str_split(bulletMeta["origin"], "|");
	_bulletOrigin = Vec2(atoi(bulletOrigin[0].c_str()), atoi(bulletOrigin[1].c_str()));
	auto bulletObb = Utils::str_split(bulletMeta["Obb"], "|");
	_bulletObb = Size(atoi(bulletObb[0].c_str()), atoi(bulletObb[1].c_str()));
	_bombRadius = atoi(bulletMeta["radius"].c_str());

	if (!bulletMeta["bulletTail"].empty())
	{
		auto tailArr = Utils::str_split(bulletMeta["bulletTail"], "|");
		_tailData = new TailData();
		_tailData->fade = atof(tailArr[0].c_str());
		_tailData->stroke = atof(tailArr[1].c_str());
		auto color = strtol(tailArr[2].c_str(), nullptr, 16);
		int red = (color & 0xFF0000) >> 16;
		int green = (color & 0x00FF00) >> 8;
		int blue = color & 0x0000FF;
		_tailData->color3b = Color3B(red, green, blue);
	}
	this->applyScaling();
}

Attribute::~Attribute()
{
	_base->release();
	_offParam.clear();
	_bulletEffect.clear();
	_fireEffect.clear();
	_hitEffect.clear();
	_bornXVec.clear();
	_bornYVec.clear();
	if (nullptr != _arrowData)
	{
		delete _arrowData;
		_arrowData = nullptr;
	}
	if (nullptr != _tailData)
	{
		delete _tailData;
		_tailData = nullptr;
	}
}

void Attribute::applyScaling()
{
	if (_scaling == 1.0f)
	{
		return;
	}
	_hpY *= _scaling;
	_offsetY *= _scaling;
	if (nullptr != _arrowData)
	{
		_arrowData->yPos *= _scaling;
	}
	_roleObb = _roleObb * _scaling;
	for (auto &bornX : _bornXVec)
	{
		bornX *= _scaling;
	}
	for (auto &bornY : _bornYVec)
	{
		bornY *= _scaling;
	}
}

int& Attribute::getLevel()
{
	return _level;
}

float Attribute::getSpeed()
{
	auto timeScale = Director::getInstance()->getScheduler()->getTimeScale();
	auto buffPair = BuffLogic::getInstance()->getBuffByPair(_base->getBaseType(), PROP_TYPE_SPEED);
	return (_speed * (1 + buffPair.second) + buffPair.first) * timeScale;
}

int& Attribute::getAiid()
{
	return _aiid;
}

float Attribute::getAttack()
{
	auto buffPair = BuffLogic::getInstance()->getBuffByPair(_base->getBaseType(), PROP_TYPE_ATTACK);
	return _attack * (1 + buffPair.second) + buffPair.first;
}

void Attribute::setAttack(const float& attack)
{
	_attack = attack;
}

float& Attribute::getShield()
{
	return _shield;
}

float& Attribute::getFinalShield()
{
	return _finalShield;
}

float Attribute::getDefense()
{
	auto buffPair = BuffLogic::getInstance()->getBuffByPair(_base->getBaseType(), PROP_TYPE_DEFENSE);
	return _defense * (1 + buffPair.second) + buffPair.first;
}

float& Attribute::getShotDistance()
{
	return _shotDistance;
}

int& Attribute::getOffsetY()
{
	return _offsetY;
}

int& Attribute::getHpY()
{
	return _hpY;
}

float& Attribute::getFireRate()
{
	return _fireRate;
}

int& Attribute::getAccuracy()
{
	return _accuracy;
}

float& Attribute::getBulletSpeed()
{
	return _bulletSpeed;
}

float& Attribute::getBulletDur()
{
	return _bulletDur;
}

int& Attribute::getMaxHit()
{
	return _maxHit;
}

float& Attribute::getBulletRepaet()
{
	return _bulletRepeat;
}

std::string& Attribute::getBulletEffect()
{
	return _bulletEffect;
}

std::string& Attribute::getFireEffect()
{
	return _fireEffect;
}

std::string& Attribute::getHitEffect(const int &index)
{
	return _hitEffect[index];
}

float Attribute::getBornXByDir(const int &dir)
{
	return _bornXVec[dir];
}

float Attribute::getBornYByDir(const int &dir)
{
	return _bornYVec[dir];
}

cocos2d::Size& Attribute::getRoleObb()
{
	return _roleObb;
}

cocos2d::Vec2& Attribute::getBulletOrigin()
{
	return _bulletOrigin;
}

cocos2d::Size& Attribute::getBuletObb()
{
	return _bulletObb;
}

float& Attribute::getCurrentHp()
{
	return _currentHp;
}

int Attribute::setCurrentHp(float hp)
{
	auto finalHp = this->getFinalHp();
	if (hp < 0)
	{
		hp = 0;
	}
	else if (hp > finalHp)
	{
		hp = finalHp;
	}
	if (_currentHp == hp)
	{
		return 0;
	}
	auto result = hp > _currentHp ? 1 : -1;
	_currentHp = hp;
	return result;
}

float Attribute::getFinalHp()
{
	auto buffPair = BuffLogic::getInstance()->getBuffByPair(_base->getBaseType(), PROP_TYPE_FINAL_HP);
	return _finalHp * (1 + buffPair.second) + buffPair.first;
}

void Attribute::setFinalHp(const float &hp)
{
	_finalHp = hp;
}

int& Attribute::getMaterialType()
{
	return _materialType;
}

int& Attribute::getAttGroup()
{
	return _attGroup;
}

int& Attribute::getRestrictLive()
{
	return _restrictLive;
}

int& Attribute::getRestrictMachine()
{
	return _restrictMachine;
}

int& Attribute::getRestrictBuild()
{
	return _restrictBuild;
}

int& Attribute::getIsDeath()
{
	return _isDeath;
}

bool& Attribute::isUseShield()
{
	return _useShield;
}

void Attribute::setShield(const float &shp)
{
	_shield = shp;
	if (_shield < 0)
	{
		_shield = 0;
	}
}

int& Attribute::getResId()
{
	return _resId;
}

int& Attribute::getBombRadius()
{
	return _bombRadius;
}

bool& Attribute::isWarn()
{
	return _isWarn;
}

bool& Attribute::isQuest()
{
	return _isQuest;
}

int& Attribute::getAttackType()
{
	return _attackType;
}

int& Attribute::getBulletId()
{
	return _bulletId;
}

int& Attribute::getOffline()
{
	return _offline;
}

float& Attribute::getScaling()
{
	return _scaling;
}

std::vector<int>& Attribute::getOffParam()
{
	return _offParam;
}

ArrowData* Attribute::getArrowData()
{
	return _arrowData;
}

TailData* Attribute::getTailData()
{
	return _tailData;
}

int& Attribute::getDropPlan()
{
	return _dropPlan;
}

int& Attribute::isSlow()
{
	return _isSlow;
}

int& Attribute::isShock()
{
	return _isShock;
}

std::string& Attribute::getDeadSound()
{
	return _sound;
}

std::string& Attribute::getBulletSound()
{
	return _bulletSound;
}

std::string& Attribute::getMiniSprite()
{
	return _miniSprite;
}