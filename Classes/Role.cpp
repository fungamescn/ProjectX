#include "Role.h"
#include "BattleLayer.h"
#include "Utils.h"
#include "Astar.h"
#include "BulletPool.h"
#include "HpBarMonster.h"
#include "HurtLogic.h"
#include "SkillExecutor.h"
#include "Element.h"
#include "RandomLib.h"
#include "StageLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

const std::vector<std::vector<int>> Role::ACTION_DICT = {
		{ 0, 1, 4, 5, 6, 7 },
		{ 0, 1 },
		{ 2, 3 }
};
//各种状态下对应部位的状�?
//-1:无动�? 0:待机, 1:移动, 2:受击, 3:死亡, 4攻击, 5:魔法
const std::vector<std::vector<int>> Role::STATE_DICT = {
		{ 0, 0, -1 }, //待机
		{ 1, 1, -1 }, //移动
		{ 4, 0, -1 }, //原地攻击
		{ 5, 0, -1 }, //原地施法
		{ 4, 1, -1 }, //移动攻击
		{ 5, 1, -1 }, //移动施法
		{ -1, -1, 2 }, //受击
		{ -1, -1, 3 }, //死亡
};

int Role::roleTag = 0;

Role::Role(const int &roleId) : 
totalDirs(8),
roleType("monster"),
movefps(0),
_roleId(roleId),
_roleState(ROLE_STATE_STAND),
_direction(0),
_shootDelta(0),
_isRetreat(false),
_directChanged(true),
_destVec(Vec2::ZERO)
{

}
Role::~Role()
{
	if (_attribute) delete _attribute;
}

Role* Role::create(const int &roleId, const int &groupType)
{
	Role *role = new(std::nothrow) Role(roleId);
	CCASSERT(role, StringUtils::format("create role fialed %d.", roleId).c_str());
	role->setGroupType(groupType);
	role->init();
	role->autorelease();
	return role;
}

bool Role::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	this->setTag(++roleTag);
	_attribute = new Attribute(this);
	this->createSpriteChilds();
	this->initSpriteChilds();
	this->scheduleUpdate();

	return true;
}

void Role::createSpriteChilds()
{
	_shadowPart = Sprite::create();
	this->addChild(_shadowPart, -10);

	_lowerPart = Sprite::create();
	this->addChild(_lowerPart);

	_upperPart = Sprite::create();
	this->addChild(_upperPart);

	_specialPart = Sprite::create();
	this->addChild(_specialPart);

	_segmentSprites = { _upperPart, _lowerPart, _specialPart };

	_shieldPart = Sprite::create();
	this->addChild(_shieldPart);

	this->initHpBar();

	aimSprite = Sprite::createWithSpriteFrameName("aim.png");
	this->addChild(aimSprite, 20);
	aimSprite->setVisible(false);
#if DEBUG_MODE
	_drawNode = DrawNode::create();
	this->addChild(_drawNode, 100);

	auto label = Label::createWithSystemFont(Utils::int2String(roleTag), "Arial", 24);
	label->setPosition(this->aimOffset());
	this->addChild(label, 1000);
#endif
}

void Role::initSpriteChilds()
{
	this->setScale(_attribute->getScaling());
	auto offsetY = _attribute->getOffsetY();
	_lowerPart->setPositionY(offsetY);
	_upperPart->setPositionY(offsetY);
	_specialPart->setPositionY(offsetY);
	_shieldPart->setPositionY(offsetY);
	_specialPart->setVisible(false);

	auto fireEffect = _attribute->getFireEffect();
	if (!fireEffect.empty())
	{
		if (nullptr == _firePart)
		{
			_firePart = Sprite::createWithSpriteFrameName(fireEffect);
			_firePart->setAnchorPoint(Vec2(0.2f, 0.5f));
			this->addChild(_firePart);
		}
		else
		{
			_firePart->cleanup();
			_firePart->setSpriteFrame(fireEffect);
		}
		_firePart->setVisible(false);
	}
	else if (nullptr != _firePart)
	{
		this->removeChild(_firePart, true);
		_firePart = nullptr;
	}

	auto arrowData = _attribute->getArrowData();
	if (arrowData && !arrowData->res.empty())
	{
		if (nullptr == _tipPart)
		{
			_tipPart = Sprite::createWithSpriteFrameName(arrowData->res);
			this->addChild(_tipPart);
		}
		else
		{
			_tipPart->cleanup();
			_tipPart->setSpriteFrame(arrowData->res);
		}
		_tipPart->setVisible(true);
		_tipPart->setPosition(0, arrowData->yPos);
		auto action = MoveBy::create(arrowData->time, Vec2(0, -arrowData->distance));
		_tipPart->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}
	else if (nullptr != _tipPart)
	{
		this->removeChild(_tipPart, true);
		_tipPart = nullptr;
	}
	if(this->getBaseType() == BASE_TYPE_MONSTER)
	{
		hpBar->setPositionY(_attribute->getHpY());
	}
	hpBar->initialize(group_type);
	hpBar->updateView(_attribute->getCurrentHp(), _attribute->getFinalHp());
	this->updateShield();
    
    if (this->roleType != "hero")
        armature = Armature::create("zombie1");
    else
        armature = Armature::create("nuge");
    _upperPart->addChild(armature);
    //armature->setPosition(Point(400, 200));
    
    
	auto resId = _attribute->getResId();
	auto roleStr = this->roleType + StringUtils::format("_%d", resId);
	if (this->getBaseType() == BASE_TYPE_HERO)
	{
		auto animationStr = roleStr + "_animation.plist";
		AnimationCache::getInstance()->addAnimationsWithFile(animationStr);
	}
	auto shadowStr = roleStr + "_3.png";
	_shadowPart->initWithSpriteFrameName(shadowStr);
	if (this->getBaseType() != BASE_TYPE_HERO)
	{
		this->createAI(_attribute->getAiid());
	}
	aimSprite->setPosition(this->aimOffset());
	this->setOldRole(NULL);
	this->findPathList.clear();
#if DEBUG_MODE
	_drawNode->clear();
	auto size = _attribute->getRoleObb();
	auto vertex0 = Vec2(-size.width / 2, 0);
	auto vertex1 = Vec2(size.width / 2, 0);
	auto vertex2 = Vec2(size.width / 2, size.height);
	auto vertex3 = Vec2(-size.width / 2, size.height);
	Vec2 points[] = { vertex0, vertex1, vertex2, vertex3 };
	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));
#endif
}

void Role::refreshProp(int roleId)
{
	if (roleId && roleId != _roleId) return;
	int oldFHp = _attribute->getFinalHp();
	int oldCHp = _attribute->getCurrentHp();
	if (_attribute)
	{
		delete _attribute;
		_attribute = nullptr;
	}
	_attribute = new Attribute(this);
	int newFHp = _attribute->getFinalHp();
	int newCHp = _attribute->getCurrentHp();
	hpBar->updateView(_attribute->getCurrentHp(), _attribute->getFinalHp());
	CCLOG("ofhp:%d  ochp:%d  nfhp:%d  nchp:%d  ", oldFHp, oldCHp, newFHp, newCHp);
}

void Role::initHpBar()
{
	hpBar = HpBarMonster::create();
	this->addChild(hpBar, 10);
}

//void Role::playSegment(const unsigned int &segmentId, const int &repeat /* = 0 */, 
//	bool reverse /* = false */, cocos2d::CallFunc* endCall /* = nullptr */)
//{
//	if (segmentId >= _segmentSprites.size())
//	{
//		return;
//	}
//	auto sprite = _segmentSprites[segmentId];
//	sprite->cleanup();
//	sprite->init();
//	auto segmentStates = STATE_DICT[_roleState];
//	auto actionId = segmentStates[segmentId];
//	if (actionId < 0)
//	{
//		return;
//	}
//	int scaleX = _direction > this->totalDirs / 2 ? -1 : 1;
//	auto dir = _direction > this->totalDirs / 2 ? this->totalDirs - _direction : _direction;
//	auto resId = _attribute->getResId();
//	auto actionStr = this->roleType + StringUtils::format("_%d_%d_%d_%d", resId, segmentId, actionId, dir);
//	auto animation = AnimationCache::getInstance()->getAnimation(actionStr);
//	if (!animation)
//	{
//		return;
//	}
//	Action* action = Animate::create(animation);
//	if (reverse)
//	{
//		action = action->reverse();
//	}
//	if (repeat <= 0)
//	{
//		action = RepeatForever::create(static_cast<ActionInterval*>(action));
//	}
//	else
//	{
//		action = Repeat::create(static_cast<FiniteTimeAction*>(action), repeat);
//		if (endCall)
//		{
//			action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
//		}
//	}
//	sprite->setVisible(true);
//	sprite->runAction(action);
//	sprite->setScaleX(scaleX);
//}

//void Role::stopSegment(const int &segmentId)
//{
//	if (segmentId==2 &&this->getSlow())
//	{
//		this->setSlow(false);
//		BattleLayer::getInstance()->resumePlaySpeed();
//	}
//
//	if (segmentId >= _segmentSprites.size())
//	{
//		return;
//	}
//	auto sprite = _segmentSprites[segmentId];
//	sprite->setVisible(false);
//	sprite->cleanup();
//	sprite->init();
//}

void Role::setDirection(const int &direction, bool update /* = false */)
{
	if (_direction == direction)
	{
		return;
	}
	_direction = direction;
	_directChanged = true;
	if (!update)
	{
		return;
	}
	switch (_roleState)
	{
	case ROLE_STATE_STAND:
		this->stand();
		break;
	case ROLE_STATE_MOVE:
		this->move();
		break;
	case ROLE_STATE_ATTAK:
		this->attack();
		break;
	case ROLE_STATE_CONJURE:
		this->conjure();
		break;
	case ROLE_STATE_MOVE_ATTACK:
		this->moveAttack(_isRetreat);
		break;
	case ROLE_STATE_MOVE_CONJURE:
		this->moveConjure(_isRetreat);
		break;
	default:
		break;
	}
}

void Role::changeState(const int &state)
{
	_directChanged = false;
	if (_roleState == state)
	{
		return;
	}
	_roleState = state;
	if (_roleState != ROLE_STATE_ATTAK && _roleState != ROLE_STATE_MOVE_ATTACK)
	{
		if (_firePart)
		{
			_firePart->pause();
			_firePart->setVisible(false);
		}
	}
}

void Role::stand()
{
	if (_roleState == ROLE_STATE_STAND && !_directChanged)
	{
		return;
	}
	this->changeState(ROLE_STATE_STAND);
	//this->playSegment(0);
	//this->playSegment(1);
	//this->stopSegment(2);
    if(armature!=NULL)
    armature->getAnimation()->play("stand");
    int scaleX = _direction > this->totalDirs / 2 ? 1 : -1;
    _upperPart->setScaleX(scaleX);
}

void Role::hurtEffect()
{
//	if (_upperPart->getColor() == Color3B::RED)
//	{
//		_upperPart->setColor(Color3B::WHITE);
//		_lowerPart->setColor(Color3B::WHITE);
//	}
//	this->stopActionByTag(ACTION_TAG_HURT);
//	_upperPart->setColor(Color3B::RED);
//	_lowerPart->setColor(Color3B::RED);
//	auto sequence = this->getActionByTag(ACTION_TAG_HURT);
//	if (nullptr == sequence)
//	{
//		sequence = Sequence::create(DelayTime::create(0.1f), CallFunc::create([&](){
//			_upperPart->setColor(Color3B::WHITE);
//			_lowerPart->setColor(Color3B::WHITE);
//		}), NULL);
//		sequence->setTag(ACTION_TAG_HURT);
//	}
//	this->runAction(sequence);
}

void Role::move()
{
	if (_roleState == ROLE_STATE_MOVE && !_directChanged)
	{
		return;
	}
	this->changeState(ROLE_STATE_MOVE);
//	this->playSegment(0);
//	this->playSegment(1);
//	this->stopSegment(2);
    if(armature!=NULL)
        armature->getAnimation()->play("run");
    int scaleX = _direction > this->totalDirs / 2 ? 1 : -1;
    _upperPart->setScaleX(scaleX);
}

void Role::attack()
{
	if (_roleState == ROLE_STATE_ATTAK && !_directChanged)
	{
		return;
	}
	this->changeState(ROLE_STATE_ATTAK);
	this->updateFireEffect();
//	this->playSegment(0);
//	this->playSegment(1);
//	this->stopSegment(2);
    if(armature!=NULL)
        armature->getAnimation()->play("attack");
    int scaleX = _direction > this->totalDirs / 2 ? 1 : -1;
    _upperPart->setScaleX(scaleX);
}

void Role::conjure()
{
//	if (_roleState == ROLE_STATE_CONJURE && !_directChanged)
//	{
//		//return;
//	}
//	this->changeState(ROLE_STATE_CONJURE);
//	this->playSegment(0, 1, false, CallFunc::create(CC_CALLBACK_0(Role::changeSkillState, this)));
//	this->playSegment(1, 1);
//	this->stopSegment(2);
}

void Role::moveAttack(bool retreat /* = false */)
{
	if (_roleState == ROLE_STATE_MOVE_ATTACK && !_directChanged)
	{
		return;
	}
	this->changeState(ROLE_STATE_MOVE_ATTACK);
	this->updateFireEffect();
//	_isRetreat = retreat;
//	this->playSegment(0);
//	this->playSegment(1, 0, retreat);
//	this->stopSegment(2);
    if(armature!=NULL)
        armature->getAnimation()->play("attack");
    int scaleX = _direction > this->totalDirs / 2 ? 1 : -1;
    _upperPart->setScaleX(scaleX);
}

void Role::updateFireEffect()
{
	if (!_firePart || this->getCurrentHp() <= 0)
	{
		return;
	}
	_firePart->setVisible(true);
	int multiple = this->totalDirs / 8;
	float angle = ((_direction + 6 * multiple) % (8 * multiple)) * 45.0f / multiple;
	_firePart->setRotation(Utils::getRotateAngle(angle));
	_firePart->setPosition(this->getShootPoint());
	if (_firePart->getActionByTag(ACTION_TAG_FIRE))
	{
		_firePart->resume();
		return;
	}
	auto action = RepeatForever::create(Blink::create(0.08f, 1));
	action->setTag(ACTION_TAG_FIRE);
	_firePart->runAction(action);
}

void Role::moveConjure(bool retreat /* = false */)
{
//	if (_roleState == ROLE_STATE_MOVE_CONJURE && !_directChanged)
//	{
//		//return;
//	}
//	this->changeState(ROLE_STATE_MOVE_CONJURE);
//	_isRetreat = retreat;
//	this->playSegment(0, 1, false, CallFunc::create(CC_CALLBACK_0(Role::changeSkillState, this)));
//	this->playSegment(1, 1, retreat);
//	this->stopSegment(2);
}

void Role::beAttacked()
{
//	if (_roleState == ROLE_STATE_HURT && !_directChanged)
//	{
//		return;
//	}
//	this->changeState(ROLE_STATE_HURT);
//	this->stopSegment(0);
//	this->stopSegment(1);
//	this->playSegment(2, 1);
}

void Role::hurt()
{

}

void Role::deadEndCall()
{
	BattleLayer::getInstance()->onBaseDead(this);

	if (this->getSlow())
	{
		this->setSlow(false);
		BattleLayer::getInstance()->resumePlaySpeed();
	}

	if (this->getBaseType() == BASE_TYPE_HERO)
	{
		if (BattleLayer::getInstance()->isHeroAllDied())
		{
			BattleLayer::getInstance()->gameOver(0);
		}
		else
		{
			static_cast<Hero*>(this)->showReviveMenu();
		}
	}
	
	//META_LINE monsterMeta = Template::getInstance()->getMetaById(TEMPLATE_MONSTER, this->getTemplateId());
	if (getBaseType() == BASE_TYPE_MONSTER && !_attribute->getIsDeath())
	{
		BattleLayer::getInstance()->removeBase(this);
	}

	auto dropPlanId = _attribute->getDropPlan();
	if (dropPlanId > 0)
	{
		RandomLib::awardByPlanId(dropPlanId, this);
	}
}

void Role::dead()
{
	if (_roleState == ROLE_STATE_DEAD && !_directChanged)
	{
		return;
	}

	if (_attribute->isSlow())
	{
		BattleLayer::getInstance()->slowPlaySpeed();
		this->setSlow(true);
	}
	else if (this->getBaseType() == BASE_TYPE_HERO && BattleLayer::getInstance()->isHeroAllDied())
	{
		BattleLayer::getInstance()->slowPlaySpeed();
		this->setSlow(true);
	}

	if (_attribute->isShock())
	{
		BattleLayer::getInstance()->startShake();
	}
	if (getAttribute()->getDeadSound() != "") CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((MUSIC_FIRE + getAttribute()->getDeadSound()).c_str());
	this->setOldRole(NULL);
	this->changeState(ROLE_STATE_DEAD);
	this->unscheduleUpdate();
//	this->stopSegment(0);
//	this->stopSegment(1);
//	this->playSegment(2, 1, false, CallFunc::create(CC_CALLBACK_0(Role::deadEndCall, this)));
	this->updateShield();
	this->deadEvent();
    if(armature!=NULL)
        armature->getAnimation()->play("death");
}

void Role::deadEvent()
{
	_destVec = Vec2::ZERO;
	if (_tipPart)
	{
		_tipPart->cleanup();
		_tipPart->setVisible(false);
	}
	if (this->roleType != "hero")
	{
		_shadowPart->setVisible(false);
	}
	auto offline = _attribute->getOffline();
	if (offline == 1)
	{
		auto offParam = _attribute->getOffParam();
		auto radius = offParam[0];
		int attGroup = offParam[1];
		auto radiusSq = radius * radius;
		auto origin = this->getShadowPoint();
		for (auto type : GROUP_TYPE)
		{
			if ((attGroup & type) == 0)
			{
				continue;
			}
			auto baseGroup = BattleLayer::getInstance()->getBaseVectorByGroupType(type);
			for (auto base : baseGroup)
			{
				if (base->getCurrentHp() <= 0 || origin.getDistanceSq(base->getShadowPoint()) > radiusSq)
				{
					continue;
				}
				HurtLogic::hurtHp(this, base, true);
			}
		}
	}
}

std::list<Point> Role::findPath(Point destination)
{
	findPathList.clear();
	if (this->getCurrentHp() <= 0)
	{
		return findPathList;
	}
	//Size tileSize = BattleLayer::getInstance()->getTiledMap()->getTileSize();
	int  tileWidth = 2668;
	int  tileHeight = 2230;
	PathFind * _pathFind = PathFind::getTheOnlyInstance();
	Point nodeStartPoint = BattleLayer::getInstance()->nodeForPosition(this->getPosition());
	Point nodeEndPoint = BattleLayer::getInstance()->nodeForPosition(destination);
	PathFind * pathFind = PathFind::getTheOnlyInstance();
	int nodeStartX = nodeStartPoint.x;
	int nodeStartY = nodeStartPoint.y;
	int nodeEndX = nodeEndPoint.x;
	int nodeEndY = nodeEndPoint.y;
	findPathList = pathFind->AStarSearch(nodeStartX, nodeStartY, nodeEndX, nodeEndY, this->getBaseType() == BASE_TYPE_HERO);
	list<Point>::iterator i;
	return findPathList;
}

void Role::doFindPath(cocos2d::Point destination)
{
	this->findPath(destination);
	//第一个点是当前位�?
	if (this->findPathList.size() <= 1)
	{
		return;
	}
	findPathList.pop_front();//去掉当前�?
	_destVec = Vec2::ZERO;
	//this->moveByStep();
}

Vec2 Role::getNextdestVec()
{
	auto position = this->getPosition();
	auto point = findPathList.front();
	auto positionNode = BattleLayer::getInstance()->positionForNode(point);
	findPathList.pop_front();
	auto mapWidth = BattleLayer::getInstance()->getTiledMap()->getContentSize().width * 2;
	auto mapHeight = BattleLayer::getInstance()->getTiledMap()->getContentSize().height * 2;
	if (point.x <= mapWidth * mapWidth && point.y <= mapHeight * mapHeight &&
		point.x >= 0 && point.y >= 0)
	{
		return _destVec = positionNode;
	}
	this->findPathList.clear();
	return _destVec = Vec2::ZERO;
}

void Role::moveUpdate()
{
	if (_destVec.isZero())
	{
		if (findPathList.size() <= 0)
		{
			return;
		}
		getNextdestVec();
		if (_destVec.isZero()) return;
		auto direction = this->getToPositiontDir(_destVec);
		this->setDirection(direction);
		this->move();
	}
	else
	{
		auto speed = _attribute->getSpeed();
		auto position = this->getPosition();
		if (_destVec.distance(position) <= speed)
		{
			this->setRolePosition(_destVec);
			_destVec = Vec2::ZERO;
		}
		else
		{
			auto radian = Utils::vecAngleToX(_destVec - position);
			auto stepX = speed * cos(radian);
			auto stepY = speed * sin(radian);
			this->setRolePosition(Point(position.x + stepX, position.y + stepY));
		}
	}
}

void Role::attackHurt(Base* enemey, float delta)
{
	if (_attribute->getBulletId() <= 0)
	{
		return;
	}
	auto fireRate = _attribute->getFireRate();
	if ((_shootDelta += delta) >= fireRate)
	{
		_shootDelta = 0;
		auto attackType = _attribute->getAttackType();
		if (attackType == 0)
		{
			HurtLogic::hurtHp(this, enemey, _attribute->isUseShield());
		}
		else
		{
			BulletPool::getInstance()->getBullet()->shoot(this, enemey);
		}
	}
}

void Role::attackUpdate(float delta)
{
	Base * monster = this->getOldRole();
	if (monster != NULL)
	{
		auto direction = this->getToPositiontDir(monster->getPosition() + monster->aimOffset(), true);
		this->setDirection(direction);
		this->attack();
		this->attackHurt(monster, delta);
	}
}

void Role::moveAttackUpdate(float delta)
{
	Base * monster = this->getOldRole();
	if (monster != NULL)
	{
		this->attackHurt(monster, delta);
		if (_destVec.isZero())
		{
			if (findPathList.size() <= 0) return;
			getNextdestVec();
			if (_destVec.isZero()) return;
			auto direction = this->getToPositiontDir(monster->getPosition() + monster->aimOffset(), true);
			//CCLOG("direction:%d", (int)direction);
			this->setDirection(direction);
			this->moveAttack();
		}
		else
		{
			auto speed = _attribute->getSpeed();
			auto position = this->getPosition();
			if (_destVec.distance(position) <= speed)
			{
				this->setRolePosition(_destVec);
				_destVec = Vec2::ZERO;
			}
			else
			{
				auto radian = Utils::vecAngleToX(_destVec - position);
				auto stepX = speed * cos(radian);
				auto stepY = speed * sin(radian);
				this->setRolePosition(Point(position.x + stepX, position.y + stepY));
			}
		}
	}
}

void Role::conjureUpdate()
{
	Base * monster = this->getOldRole();
	if (monster != NULL)
	{
		if (aiSkillDataList.size() > 0)
		{
			AISKILLDATA sdata = aiSkillDataList.front();
			nextSkillFrame = sdata.frameDelay;
			ValueMap vm = {
					{ "x", cocos2d::Value(monster->getPosition().x + monster->aimOffset().x) },
					{ "y", cocos2d::Value(monster->getPosition().y + monster->aimOffset().y) }
			};
			SkillExecutor::getInstance()->executeSkill(this, sdata.skillId, cocos2d::Value(vm));
			if (aiSkillDataList.size() > 1)
			{
				aiSkillDataList.push_back(sdata);
				aiSkillDataList.pop_front();
			}
			auto direction = this->getToPositiontDir(monster->getPosition() + monster->aimOffset(), true);
			this->setDirection(direction);
			_useSkill = true;
			this->conjure();
		}
	}
}

void Role::moveConjureUpdate()
{
	Base * monster = this->getOldRole();
	if (monster != NULL)
	{
		if (aiSkillDataList.size() > 0)
		{
			AISKILLDATA sdata = aiSkillDataList.front();
			nextSkillFrame = sdata.frameDelay;
			ValueMap vm = {
					{ "x", cocos2d::Value(monster->getPosition().x + monster->aimOffset().x) },
					{ "y", cocos2d::Value(monster->getPosition().y + monster->aimOffset().y) }
			};
			SkillExecutor::getInstance()->executeSkill(this, sdata.skillId, cocos2d::Value(vm));
			if (aiSkillDataList.size() > 1)
			{
				aiSkillDataList.push_back(sdata);
				aiSkillDataList.pop_front();
			}
			auto direction = this->getToPositiontDir(monster->getPosition() + monster->aimOffset(), true);
			this->setDirection(direction);
			this->moveConjure();
		}
	}
}

int Role::calRoleState()
{
	Base * monster = getOldRole();
	bool hasPath = false;
	if (this->findPathList.size() > 0){

		hasPath = true;
	}
	if (monster == NULL)
	{
		if (hasPath) return ROLE_STATE_MOVE;
		else return ROLE_STATE_STAND;
	}
	else{
		if (nextSkillFrame <= 0 && this->aiSkillDataList.size() > 0)
		{
			if (hasPath) return ROLE_STATE_MOVE_CONJURE;
			else return ROLE_STATE_CONJURE;
		}
		else{
			if (hasPath) return ROLE_STATE_MOVE_ATTACK;
			else return ROLE_STATE_ATTAK;
		}
	}
}

void Role::update(float delta)
{
	if (!BattleLayer::getInstance()->getRoleEnable())
	{
		return;
	}

	if (_useSkill) return;
	if (this->getCurrentHp() <= 0)
	{
		this->dead();
		return;
	}
	int roleState = calRoleState();
	switch (roleState)
	{
	case ROLE_STATE_STAND:
		this->stand();
		break;
	case ROLE_STATE_MOVE:
		this->moveUpdate();
		break;
	case ROLE_STATE_ATTAK:
		this->attackUpdate(delta);
		break;
	case ROLE_STATE_CONJURE:
		this->conjureUpdate();
		break;
	case ROLE_STATE_MOVE_ATTACK:
		this->moveAttackUpdate(delta);
		break;
	case ROLE_STATE_MOVE_CONJURE:
		this->moveConjureUpdate();
		break;
	case ROLE_STATE_HURT:
		break;
	default:
		break;
	}
}

void Role::createAI(int aiid)
{
	setAIID(aiid);
	META_LINE aiMeta = Template::getInstance()->getMetaById(TEMPLATE_AI, aiid);
	setFollowAI(aiMeta["aiFollowTarget"]);
	setSelectAI(aiMeta["aiSelectTarget"]);
	aiSkillDataList.clear();
	aiFindpath.clear();
	_useSkill = false;
	if (Utils::string2Int(aiMeta["aiPath"]))
	{
		vector<string> result = Utils::str_split(aiMeta["aiPath"], "|");
		for (vector<string >::iterator iter = result.begin(); iter != result.end(); ++iter)
		{
			vector<string> strVec = Utils::str_split(*iter, ",");
			Point pt = BattleLayer::getInstance()->positionForTileCoord(Point(atof(strVec.at(0).c_str()), atof(strVec.at(1).c_str())));
			aiFindpath.push_back(pt);
		}
	}
	if (Utils::string2Int(aiMeta["aiSkill"].c_str()) && Utils::string2Int(aiMeta["aiSkillDelay"]))
	{
		vector<string> skillResult = Utils::str_split(aiMeta["aiSkill"], "|");
		vector<string> skillDelayResult = Utils::str_split(aiMeta["aiSkillDelay"], "|");
		for (unsigned int i = 0; i < skillResult.size(); i++)
		{
			AISKILLDATA skillData;
			skillData.skillId = Utils::string2Int(skillResult.at(i));
			skillData.frameDelay = Utils::string2Int(skillDelayResult.at(i));
			auto skillMeta = Template::getInstance()->getMetaById(TEMPLATE_SKILL, skillData.skillId);
			skillData.distance = atof(skillMeta["skillDistance"].c_str());
			aiSkillDataList.push_back(skillData);
		}
	}
	nextSkillFrame = 0.01;
}

void Role::doAIAction(float del)
{
	//在多少帧释放技�?
	if (aiSkillDataList.size() > 0) nextSkillFrame = nextSkillFrame - del;
	_aiActionCount++;
	if (_aiActionCount > 10)
	{
		_aiActionCount = 0;
	}else
	{
		//return;
	}
	//跑步或者死亡状�?
	if (this->getCurrentHp() <= 0 || _useSkill) return;
	//设置攻击目标
	setAIAttTarget();
}

// 优先技能目标选择->普工技能选择
void Role::setAIAttTarget()
{
	Base * nextRole = getSkillTargetAI();
	if (nextRole == NULL)
	{
		nextRole = getTargetAI();
	}
	this->setOldRole(nextRole);
	if (findPathList.size() <= 0 && aiFindpath.size() > 0)
	{
		Point pt = aiFindpath.front();
		doFindPath(pt);
		aiFindpath.pop_front();
	}
	else if (aiFindpath.size() <= 0){
		Base * d_role = getFollowAI();
		if (d_role != NULL)
		{
			if (d_role->getShadowPoint().getDistanceSq(this->getShadowPoint())
				> this->getAttribute()->getShotDistance()* this->getAttribute()->getShotDistance()){
				/*if (d_role->getBaseType() != BASE_TYPE_ELEMENT && static_cast<Role *>(d_role)->getAIFollow() == "-1" && static_cast<Role *>(d_role)->aiFindpath.size() <= 0 && static_cast<Role *>(d_role)->findPathList.size() <= 0)
				{
					if (findPathList.size() <= 0)
					{
						doFindPath(d_role->getPosition());
					}
				}*/
				//else
					if (d_role->getBaseType() == BASE_TYPE_ELEMENT)
				{
					std::vector<Vec2> movePoint = static_cast<Element *>(d_role)->movePoint;
					if (movePoint.size() > 0)
					{
						//doFindPath(BattleLayer::getInstance()->getNearVec2(d_role->getPosition() + d_role->aimOffset(), this->getPosition(), movePoint));
						this->findPathList.clear();
						this->findPathList.push_front(BattleLayer::getInstance()->getNearVec2(d_role->getPosition() + d_role->aimOffset(), this->getPosition(), movePoint));
					}
					else{
						//doFindPath(d_role->getPosition());
						this->findPathList.clear();
						this->findPathList.push_front(d_role->getPosition());
					}
				}
				else{
					Point nextPt = getNearNode(d_role);
					if (!nextPt.isZero())
					{
						this->findPathList.clear();
						this->findPathList.push_front(nextPt);
					}
				}
			}
			else
			{
				if (d_role->getBaseType() != BASE_TYPE_ELEMENT && static_cast<Role *>(d_role)->getAIFollow() == "-1")
				{
					this->findPathList.clear();
				}
			}
		}
	}
}

//AI目标选择 ->原射击目�?X-> 寻找特定目标 X-> 随机目标
Base * Role::getTargetAI()
{
	string sids = this->getAISelect();
	if (sids == "-1") return NULL;
	Base * oldRole = this->getOldRole();
	if (oldRole != NULL &&  oldRole->getCurrentHp() > 0)
	{
		string idstr = "|" + Utils::int2String((oldRole)->getTemplateId()) + "|";
		if (sids == "0" || sids.find(idstr) < sids.size())
		{
			if (oldRole->getShadowPoint().getDistanceSq(this->getShadowPoint())
				<= this->getAttribute()->getShotDistance()* this->getAttribute()->getShotDistance()){
				return oldRole;
			}
		}
	}
	int enemyGroupType = this->getGroupType()&GROUP_TYPE_JUST ? GROUP_TYPE_EVIL : GROUP_TYPE_HERO;
	if (sids != "0")
	{  //首先选择AI目标
		Base * enemyBase = getRecentEnemyByGroupType(enemyGroupType, getAttribute()->getShotDistance(), true);
		if (this->getGroupType()&GROUP_TYPE_EVIL)
		{
			enemyGroupType = GROUP_TYPE_JUST;
			Base * enemyBase1 = getRecentEnemyByGroupType(enemyGroupType, getAttribute()->getShotDistance(), true);
			if (enemyBase != NULL && enemyBase != NULL)
			{
				if (enemyBase1->getShadowPoint().getDistanceSq(this->getShadowPoint()) >= enemyBase->getShadowPoint().getDistanceSq(this->getShadowPoint())) return enemyBase;
				return enemyBase1;
			}
			else if (enemyBase != NULL)
			{
				return enemyBase;
			}
			else if (enemyBase1 != NULL)
			{
				return enemyBase1;
			}
		}
		else
		{
			if (enemyBase != NULL) return enemyBase;
		}
	}
	Base * enemyBase = getRecentEnemyByGroupType(enemyGroupType, getAttribute()->getShotDistance(), false);
	if (this->getGroupType()&GROUP_TYPE_EVIL) enemyGroupType = GROUP_TYPE_JUST;
	else return enemyBase;
	Base * enemyBase1 = getRecentEnemyByGroupType(enemyGroupType, getAttribute()->getShotDistance(), false);
	if (enemyBase1 && enemyBase)
	{
		if (enemyBase1->getShadowPoint().getDistanceSq(this->getShadowPoint()) >= enemyBase->getShadowPoint().getDistanceSq(this->getShadowPoint())) return enemyBase;
		return enemyBase1;
	}
	return enemyBase1 != NULL ? enemyBase1 : enemyBase;
}

Base * Role::getFollowAI()
{
	string sids = this->getAIFollow();
	if (sids == "-1") return NULL;
	int enemyGroupType = this->getGroupType()&GROUP_TYPE_JUST ? GROUP_TYPE_EVIL : GROUP_TYPE_HERO;
	if (sids != "0")
	{  //首先选择AI目标
		Base * enemyBase = getFollowEnemyByGroupType(enemyGroupType, true);
		if (this->getGroupType()&GROUP_TYPE_EVIL)
		{
			enemyGroupType = GROUP_TYPE_JUST;
			Base * enemyBase1 = getFollowEnemyByGroupType(enemyGroupType, true);
			if (enemyBase != NULL && enemyBase != NULL)
			{
				if (enemyBase1->getShadowPoint().getDistanceSq(this->getShadowPoint()) >= enemyBase->getShadowPoint().getDistanceSq(this->getShadowPoint())) return enemyBase;
				return enemyBase1;
			}
			else if (enemyBase != NULL)
			{
				return enemyBase;
			}
			else if (enemyBase1 != NULL)
			{
				return enemyBase1;
			}
		}
		else
		{
			if (enemyBase != NULL) return enemyBase;
		}
	}
	Base * enemyBase = getFollowEnemyByGroupType(enemyGroupType, false);
	if (this->getGroupType()&GROUP_TYPE_EVIL) enemyGroupType = GROUP_TYPE_JUST;
	else return enemyBase;
	Base * enemyBase1 = getFollowEnemyByGroupType(enemyGroupType, false);
	if (enemyBase1 && enemyBase)
	{
		if (enemyBase1->getShadowPoint().getDistanceSq(this->getShadowPoint()) >= enemyBase->getShadowPoint().getDistanceSq(this->getShadowPoint())) return enemyBase;
		return enemyBase1;
	}
	return enemyBase1 != NULL ? enemyBase1 : enemyBase;
}

Base * Role::getSkillTargetAI()
{
	if (aiSkillDataList.size() == 0) return NULL;
	if (nextSkillFrame > 0) return NULL;
	string sids = this->getAISelect();
	//if (sids == "-1") return NULL;
	AISKILLDATA sdata = aiSkillDataList.front();// pop_front();
	Base * oldRole = this->getOldRole();
	if (oldRole != NULL &&  oldRole->getCurrentHp() > 0)
	{
		string idstr = "|" + Utils::int2String((oldRole)->getTemplateId()) + "|";
		if (sids == "0" || sids.find(idstr) < sids.size())
		{
			if (oldRole->getShadowPoint().getDistanceSq(this->getShadowPoint())
				<= sdata.distance * sdata.distance){
				return oldRole;
			}
		}
	}
	int enemyGroupType = this->getGroupType()&GROUP_TYPE_JUST ? GROUP_TYPE_EVIL : GROUP_TYPE_HERO;
	Base * enemyBase = getRecentEnemyByGroupType(enemyGroupType, sdata.distance, false);
	if (this->getGroupType()&GROUP_TYPE_EVIL) enemyGroupType = GROUP_TYPE_JUST;
	else return enemyBase;
	Base * enemyBase1 = getRecentEnemyByGroupType(enemyGroupType, sdata.distance, false);
	if (enemyBase1 && enemyBase)
	{
		if (enemyBase1->getShadowPoint().getDistanceSq(this->getShadowPoint()) >= enemyBase->getShadowPoint().getDistanceSq(this->getShadowPoint())) return enemyBase;
		return enemyBase1;
	}
	return enemyBase1 != NULL ? enemyBase1 : enemyBase;
}

Base * Role::getOldRole()
{
	if (_oldRole == NULL || _oldRole->getCurrentHp() <= 0)
		return NULL;
	else
		return _oldRole;
}

void Role::setOldRole(Base * Role)
{
	if (this->getOldRole() != NULL) this->getOldRole()->releaseAttack();
	if (Role != NULL) Role->retainAttack();
	_oldRole = Role;
}

void Role::setRolePosition(Point position)
{
	this->setPosition(position);
	this->factTilePoint = BattleLayer::getInstance()->nodeForPosition(position);

	depthPoint = position;

	if (this->getGroupType() == GROUP_TYPE_HERO || this->getGroupType() == GROUP_TYPE_JUST)
	{
		BattleLayer::getInstance()->getEventManager()->trigger(EventManager::EVENT_TYPE_BASE_POSITION_CHANGE, this);
	}
}

Vec2 Role::getShootPoint()
{
	bool reverse = _direction > this->totalDirs / 2;
	auto dir = reverse ? this->totalDirs - _direction : _direction;
	auto bornX = _attribute->getBornXByDir(dir);
	auto bornY = _attribute->getBornYByDir(dir);
	if (reverse)
	{
		bornX *= -1;
	}
	return Vec2(bornX, bornY);
}

OBB2d* Role::getOBB2d()
{
	if (!obb2d)
	{
		obb2d = new OBB2d();
	}

	auto size = _attribute->getRoleObb();
	auto position = this->getPosition();

	auto vertex0 = Vec2(position.x - size.width / 2, position.y);
	auto vertex1 = Vec2(position.x + size.width / 2, position.y);
	auto vertex2 = Vec2(position.x + size.width / 2, position.y + size.height);
	auto vertex3 = Vec2(position.x - size.width / 2, position.y + size.height);

	obb2d->setVertex(0, vertex0);
	obb2d->setVertex(1, vertex1);
	obb2d->setVertex(2, vertex2);
	obb2d->setVertex(3, vertex3);

	return obb2d;
}

Rect Role::getTouchRect()
{
	auto size = _attribute->getRoleObb();
	return Rect(-size.width / 2, 0, size.width, size.height);
}

void Role::clearFindPathList()
{
	this->findPathList.clear();
}

Point Role::getNearNode(Base * role)
{
	Point node = BattleLayer::getInstance()->nodeForPosition(this->getPosition());
	//Point r_node = BattleLayer::getInstance()->nodeForPosition(role->getPosition());
	int offsetX[] = { -1, 0, 1, 1, 1, 0, -1, -1 };
	int offsetY[] = { -1, -1, -1, 0, 1, 1, 1, 0 };
	Point nearestNode = Point(0, 0);
	int dis_len = MAX_NUM;
	for (int i = 0; i < 8; i++)
	{
		Point nearNode = Point(node.x + offsetX[i], node.y + offsetY[i]);
		if (!PathFind::getTheOnlyInstance()->AstarIsBlock(nearNode.x, nearNode.y)){
			Point factPt = BattleLayer::getInstance()->positionForNode(nearNode);
			float dis = Utils::getDistance(factPt, role->getPosition());
			if (dis <= dis_len)
			{
				nearestNode = nearNode;
				dis_len = dis;
			}
		}
	}
	return nearestNode;
}

int Role::getTemplateId()
{
	return _roleId;
}

float Role::getCurrentHp()
{
	return _attribute->getCurrentHp();
}

void Role::setCurrentHp(const float &hp)
{
	auto result = _attribute->setCurrentHp(hp);
	if (0 == result)
	{
		return;
	}
	if (result < 0)
	{
		this->hurtEffect();
	}
	hpBar->updateView(_attribute->getCurrentHp(), _attribute->getFinalHp());
}

float Role::getFinalHp()
{
	return _attribute->getFinalHp();
}

void Role::setFinalHp(const float &hp)
{
	_attribute->setFinalHp(hp);
}

int Role::getToPositiontDir(const cocos2d::Vec2& dest, bool isAttack /* = false */)
{
	auto minVec = dest - this->getPosition();
	if (isAttack)
	{
		auto size = _attribute->getRoleObb();
		minVec.y -= size.height / 2;
	}
	return Utils::getDirOct(minVec);
}

float Role::getSpeed()
{
	return _attribute->getSpeed();
}

int Role::getMaterialType()
{
	return _attribute->getMaterialType();
}

float Role::getAttack()
{
	return _attribute->getAttack();
}

void Role::setAttack(const float &attack)
{
	_attribute->setAttack(attack);
}

float Role::getDefense()
{
	return _attribute->getDefense();
}

float Role::getShield()
{
	return _attribute->getShield();
}

void Role::setShield(const float &shp)
{
	_attribute->setShield(shp);
	this->updateShield();
}

void Role::updateShield()
{
	auto shield = _attribute->getShield();
	if (shield <= 0 || this->getCurrentHp() <= 0)
	{
		_shieldPart->cleanup();
		_shieldPart->init();
		_shieldPart->setVisible(false);
		return;
	}
	_shieldPart->setVisible(true);
	if (_shieldPart->getActionByTag(ACTION_TAG_SHIELD))
	{
		return;
	}
	_shieldPart->cleanup();
	_shieldPart->init();
	auto animation = AnimationCache::getInstance()->getAnimation("shield");
	auto action = RepeatForever::create(Animate::create(animation));
	action->setTag(ACTION_TAG_SHIELD);
	_shieldPart->runAction(action);
}

bool Role::isQuest()
{
	return _attribute->isQuest();
}

bool Role::isWarn()
{
	return _attribute->isWarn();
}

Base * Role::getRecentEnemyByGroupType(const int groupType, float skillDistance, bool useIds = false)
{
	std::vector<Base *> rolesList = BattleLayer::getInstance()->getBaseVectorByGroupType(groupType);
	Base * base = NULL;
	float now_minDis = MAX_NUM;
	string ids = this->getAISelect();
	string idstr;
	for (auto it = rolesList.begin(); it != rolesList.end(); ++it){
		if ((*it)->getCurrentHp() > 0){
			if (useIds) idstr = "|" + Utils::int2String((*it)->getTemplateId()) + "|";
			if (!useIds || ids.find(idstr) < ids.size())
			{
				float minDis = (*it)->getPosition().distanceSquared(this->getPosition());
				if (minDis <= skillDistance * skillDistance)
				{
					if (base == NULL){
						base = (Base *)*it;
					}
					else{
						if ((minDis == now_minDis && (*it)->getCurrentHp() > (*it)->getCurrentHp())
							|| minDis > now_minDis)
							continue;
						base = (Base *)*it;
						now_minDis = minDis;
					}
				}
			}
		}
	}
	return base;
}

Base * Role::getFollowEnemyByGroupType(const int groupType, bool useIds)
{
	std::vector<Base *> rolesList = BattleLayer::getInstance()->getBaseVectorByGroupType(groupType);
	Base * base = NULL;
	float now_minDis = MAX_NUM;
	string ids = this->getAIFollow();
	string idstr;
	for (auto it = rolesList.begin(); it != rolesList.end(); ++it){
		if ((*it)->getCurrentHp() > 0){
			if (useIds) idstr = "|" + Utils::int2String((*it)->getTemplateId()) + "|";
			if (!useIds || ids.find(idstr) < ids.size())
			{
				float minDis = (*it)->getPosition().distanceSquared(this->getPosition());
				if (base == NULL){
					base = (Base *)*it;
				}
				else{
					if ((minDis == now_minDis && (*it)->getCurrentHp() > (*it)->getCurrentHp())
						|| minDis > now_minDis)
						continue;
					base = (Base *)*it;
					now_minDis = minDis;
				}
			}
		}
	}
	return base;
}

void Role::changeSkillState()
{
	this->_useSkill = false;
}

void Role::createRole(const int &roleId, const int &groupType, cocos2d::Point &point, bool isTiledPos /* = true */)
{
	movefps = 0;
	_roleId = roleId;
	_roleState = ROLE_STATE_STAND;
	_direction = 0; 
	_shootDelta = 0;
	_isRetreat = false;
	_directChanged = true;
	_destVec = Vec2::ZERO;
	this->setGroupType(groupType);
	delete _attribute;
	_attribute = new Attribute(this);
	this->initSpriteChilds();
	_shadowPart->setVisible(true);
	if (isTiledPos)
	{
		Point p = BattleLayer::getInstance()->positionForTileCoord(point);
		this->setRolePosition(p);
	}
	else
	{
		this->setRolePosition(point);
	}
	this->setVisible(true);
	this->scheduleUpdate();
}

void Role::releaseSkill()
{
	this->_skillCount--;
}

void Role::retainSkill()
{
	this->_skillCount++;
}

void Role::releaseBullet()
{
	this->_bulletCount--;
}

void Role::retainBullet()
{
	this->_bulletCount++;
}

void Role::releaseAttack()
{
	this->_attedCount--;
}

void Role::retainAttack()
{
	this->_attedCount++;
}

bool Role::canRecovey()
{
	if (this->_bulletCount <= 0 && this->_skillCount <= 0 && this->_attedCount <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Role::setGroupType(const int &grouptype)
{
	group_type = grouptype;
}

std::string& Role::getMiniSpriteNum()
{
	return this->_attribute->getMiniSprite();
}