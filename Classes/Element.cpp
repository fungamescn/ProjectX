#include "Element.h"
#include "Template.h"
#include "Utils.h"
#include "BattleLayer.h"
#include "HurtLogic.h"
#include "RandomLib.h"
#include "SimpleAudioEngine.h"

int Element::idCount = 0;

bool Element::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	_id = Element::idBuilder();

	_normalBody = Sprite::create();
	_normalBody->setAnchorPoint(Vec2::ZERO);
	this->addChild(_normalBody);

	_deadBody = Sprite::create();
	_deadBody->setAnchorPoint(Vec2(0.5,0.46));
	this->addChild(_deadBody);

	aimSprite = Sprite::createWithSpriteFrameName("aim.png");
	this->addChild(aimSprite);
	aimSprite->setVisible(false);
	return true;
}

int Element::idBuilder()
{
	return ++idCount;
}

std::vector<int> getPosStrVec2(int tileX, int tileY, float minX, float minY, float maxX, float maxY)
{
	std::vector<int> posVec;

	int startX = 2 * (tileX + minX);
	int startY = 2 * (tileY + minY);
	int endX = 2 * (tileX + maxX);
	int endY = 2 * (tileY + maxY);
	//	string str = StringUtils::format("%03d%03d%03d%03d", startX, startY, endX, endY);
	
	posVec.push_back(startX);
	posVec.push_back(startY);
	posVec.push_back(endX);
	posVec.push_back(endY);

	return posVec;
}

std::vector<Vec2> getElementFollowTitle(int tileX, int tileY, int minX, int minY, int maxX, int maxY)
{
	int startX = 2 * (tileX + minX);
	int endX = 2 * (tileX + maxX);
	int startY = 2 * (tileY + minY);
	int endY = 2 * (tileY + maxY);
	std::vector<Vec2> movePoint;
	movePoint.push_back(BattleLayer::getInstance()->positionForNode(Vec2(startX - 1, startY - 1)));
	movePoint.push_back(BattleLayer::getInstance()->positionForNode(Vec2(endX + 1, startY - 1)));
	movePoint.push_back(BattleLayer::getInstance()->positionForNode(Vec2(endX + 1, endY + 1)));
	movePoint.push_back(BattleLayer::getInstance()->positionForNode(Vec2(startX - 1, endY + 1)));
	return movePoint;
}

//battle Layer зјБъ
std::vector<Vec2> getPosVec(int tileX, int tileY, int minX, int minY, int maxX, int maxY)
{
	std::vector<Vec2> posVec;

	int startX = 2 * (tileX + minX);
	int endX = 2 * (tileX + maxX);
	int startY = 2 * (tileY + minY);
	int endY = 2 * (tileY + maxY);

	posVec.push_back(BattleLayer::getInstance()->positionForNode(Vec2(startX, startY)));
	posVec.push_back(BattleLayer::getInstance()->positionForNode(Vec2(endX, startY)));
	posVec.push_back(BattleLayer::getInstance()->positionForNode(Vec2(endX, endY)));
	posVec.push_back(BattleLayer::getInstance()->positionForNode(Vec2(startX, endY)));

	return posVec;
}

FRAME_VECTOR getFrameVector(std::string fileName,std::string state)
{
	int frameId = 1;
	auto formatStr = fileName + "_" + state + "_%d.png";
	auto formatChar = formatStr.c_str();
	FRAME_VECTOR frameVec;
	SpriteFrame* actionFrame;
	do
	{
		auto frameName = StringUtils::format(formatChar, frameId);
		actionFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
		if (!actionFrame)
		{
			break;
		}
		frameVec.pushBack(actionFrame);
		++frameId;
	} while (true);
	return frameVec;
}


void Element::setElementData(const int &tid, int &groupType, Point &point)
{
	_tid = tid;
	META_LINE sceneElementMeta = Template::getInstance()->getMetaById(TEMPLATE_SCENE_ELEMENT, tid);

	ElementData* eData = new ElementData();
	eData->groupId = sceneElementMeta["groupId"] == "" ? sceneElementMeta["id"] : sceneElementMeta["groupId"];
	eData->name = sceneElementMeta["name"];
	eData->stage = sceneElementMeta["stage"];
	eData->miniSprite = sceneElementMeta["miniSprite"];
	if (point.x == 0 && point.y == 0)
	{
		eData->sceneX = Utils::string2Int(sceneElementMeta["sceneX"]);
		eData->sceneY = Utils::string2Int(sceneElementMeta["sceneY"]);
	}
	else
	{
		eData->sceneX = point.x;
		eData->sceneY = point.y;
	}

	
	eData->maxHp = sceneElementMeta["hp"] == "" ? 1 : Utils::string2Int(sceneElementMeta["hp"]);
	eData->hp = eData->maxHp;
	eData->attack = Utils::string2Int(sceneElementMeta["attack"]);
	eData->canDestroy = Utils::string2Int(sceneElementMeta["canDestroy"]);
	eData->isQuest = sceneElementMeta["isQuest"] == "1";
	eData->isWarn = sceneElementMeta["isWarn"] == "1";
	eData->offline = Utils::string2Int(sceneElementMeta["offline"]);
	eData->offlineParam1 = sceneElementMeta["params1"];
	eData->offlineParam2 = sceneElementMeta["params2"];
	eData->offlineParam3 = sceneElementMeta["params3"];
	eData->canAttacked = sceneElementMeta["canAttacked"]=="1";
	eData->modelId = Utils::string2Int(sceneElementMeta["modelId"]);
	eData->dropPlan = Utils::string2Int(sceneElementMeta["dropPlan"]);
	eData->isSlow = Utils::string2Int(sceneElementMeta["isSlow"]);
	eData->isShock = Utils::string2Int(sceneElementMeta["isShock"]);

	int mid = Utils::string2Int(sceneElementMeta["model"]);

	META_LINE modelMeta = Template::getInstance()->getMetaById(TEMPLATE_ELEMENT_MODEL, mid);
	
	ElementModel* model = new ElementModel();
	model->id = mid;
	model->name = modelMeta["name"];
	model->res = modelMeta["res"];
	model->materialType = Utils::string2Int(modelMeta["res"]);
	model->shadowX = Utils::string2Int(modelMeta["shadowX"]);
	model->shadowY = Utils::string2Int(modelMeta["shadowY"]);
	model->showHpBar = Utils::string2Int(modelMeta["showHpBar"]);
	model->hpY = Utils::string2Int(modelMeta["hpY"]);
	model->selectX = Utils::string2Int(modelMeta["selectX"]);
	model->selectY = Utils::string2Int(modelMeta["selectY"]);
	model->touchCenterX = Utils::string2Int(modelMeta["touchCenterX"]);
	model->touchCenterY = Utils::string2Int(modelMeta["touchCenterY"]);
	model->touchWidth = Utils::string2Int(modelMeta["touchWidth"]);
	model->touchHeight = Utils::string2Int(modelMeta["touchHeight"]);
	
	
	model->depthSP = Utils::str_split_f(modelMeta["depthSP"], ",");
	model->depthEP = Utils::str_split_f(modelMeta["depthEP"], ",");

	model->sound = modelMeta["sound"];

	std::vector<std::string> blockPos = Utils::str_split(modelMeta["block"], "|");
	std::vector<float> startPointVec = Utils::str_split_f(blockPos[0], ",");
	std::vector<float> endPointVec = Utils::str_split_f(blockPos[1], ",");
	model->block = getPosStrVec2(eData->sceneX, eData->sceneY, startPointVec[0], startPointVec[1], endPointVec[0], endPointVec[1]);
	movePoint = getElementFollowTitle(eData->sceneX, eData->sceneY, startPointVec[0], startPointVec[1], endPointVec[0], endPointVec[1]);
	std::vector<std::string> collisonPos = Utils::str_split(modelMeta["collison"], "|");
	std::vector<float> startPointVec1 = Utils::str_split_f(collisonPos[0], ",");
	std::vector<float> endPointVec1 = Utils::str_split_f(collisonPos[1], ",");
	model->collison = getPosVec(eData->sceneX, eData->sceneY, startPointVec1[0], startPointVec1[1], endPointVec1[0], endPointVec1[1]);

	model->selfScale = Utils::string2Float(modelMeta["selfScale"]);
	model->bombScale = Utils::string2Float(modelMeta["bombScale"]);

	eData->model = model;
	this->data = eData;
	
	depthPoint = BattleLayer::getInstance()->positionForNode(Point(2 * (this->data->sceneX + this->data->model->depthEP[0]), 2 * (this->data->sceneY + this->data->model->depthEP[1])));



	if (groupType != 0)
	{
		group_type = groupType;
	}
	else
	{
		group_type = (sceneElementMeta["groupType"] == "" ? GROUP_TYPE_NEUTRAL : Utils::string2Int(sceneElementMeta["groupType"]));
	}
	/*
	std::string filename = model->res + ".plist";

	if (FileUtils::getInstance()->isFileExist(filename))
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(filename);
		normalVec = getFrameVector(model->res, "normal");
		dieVec = getFrameVector(model->res, "die");
	}
	*/

//	_normalBody->setPosition(this->data->model->shadowX, this->data->model->shadowY);
	_deadBody->setPosition(this->data->model->shadowX, this->data->model->shadowY);
	
	setState("normal");
	aimSprite->setPosition(this->aimOffset());

	if (!sceneElementMeta["arrowRes"].empty())
	{
		_tipPart = Sprite::createWithSpriteFrameName(sceneElementMeta["arrowRes"]);
		this->addChild(_tipPart);
		_tipPart->setPosition(atoi(modelMeta["shadowX"].c_str()), atoi(sceneElementMeta["arrowPosY"].c_str()));
		auto action = MoveBy::create(GUIDE_ARROW_TIME, Vec2(0, GUIDE_ARROW_DISTANCE));
		_tipPart->runAction(RepeatForever::create(Sequence::create(action, action->reverse(), NULL)));
	}
	
	if (model->showHpBar)
	{
		_hpBar = HpBarMonster::create();
		_hpBar->initialize(groupType);
		_hpBar->setPosition(model->shadowX, model->shadowY + model->hpY);
		_hpBar->setVisible(false);
		this->addChild(_hpBar, 10);
	}

#if DEBUG_MODE
	Point bp1 = BattleLayer::getInstance()->positionForTileCoord(Point(eData->sceneX, eData->sceneY + 1));
	Point bp2 = BattleLayer::getInstance()->positionForTileCoord(Point(eData->sceneX + 1, eData->sceneY + 1));
	auto position = Vec2(bp1.x, bp2.y);
	_drawNode = DrawNode::create();
	this->addChild(_drawNode);
	Vec2 p0 = model->collison.at(0) - position;
	Vec2 p1 = model->collison.at(1) - position;
	Vec2 p2 = model->collison.at(2) - position;
	Vec2 p3 = model->collison.at(3) - position;
	Vec2 points[] = { p0, p1, p2, p3 };
	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));

	if (eData->offline == ELEMENT_DEAD_AREA_DAMAGE)
	{
		int damageDist = Utils::string2Int(eData->offlineParam1);
		Vec2 center = Vec2(model->shadowX, model->shadowY);
		_drawNode->drawCircle(center, damageDist, 0, 360, false, Color4F(0, 0, 1, 1));
	}

	auto label = Label::createWithSystemFont(Utils::int2String(tid), "Arial", 24);
	label->setPosition(this->aimOffset());
	this->addChild(label);
	string str = Utils::int2String(position.x) + " :" + Utils::int2String(position.y) + "\n" + Utils::int2String(BattleLayer::getInstance()->tileCoordForPosition(position).x) + " :" + Utils::int2String(BattleLayer::getInstance()->tileCoordForPosition(position).y);
	auto label1 = Label::createWithSystemFont(str, "Arial", 24);
	label1->setPosition(this->aimOffset() * 2);
	this->addChild(label1);
#endif
}

int Element::getId()
{
	return _id;
}

string Element::getGroupId()
{
	return data->groupId;
}

Point Element::aimOffset()
{ 
	return Point(this->data->model->selectX, this->data->model->selectY);
};

void Element::hurt()
{

}

void Element::dead()
{
	if (nullptr != _tipPart)
	{
		this->removeChild(_tipPart, true);
		_tipPart = nullptr;
	}
	if (this->data->model->sound != "")
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((MUSIC_FIRE + this->data->model->sound).c_str());
	deadAction();
	setState("dead");
	//offline action
}

void Element::deadAction()
{
	if (data->offline == ELEMENT_DEAD_AREA_DAMAGE)
	{
		int damageDist = Utils::string2Int(data->offlineParam1);
		int gpType = Utils::string2Int(data->offlineParam2);
		int typeArr[] = { GROUP_TYPE_HERO ,GROUP_TYPE_JUST,GROUP_TYPE_EVIL,GROUP_TYPE_NEUTRAL};
		for (auto groupType : typeArr)
		{
			if ( !(gpType & groupType) )
			{
				continue;
			}
			std::vector<Base *> baseVec = BattleLayer::getInstance()->getBaseVectorByGroupType(groupType);

			for (size_t i = 0; i < baseVec.size(); i++)
			{
				float dist = Utils::getDistance(baseVec[i]->getShadowPoint(), this->getShadowPoint());
				if (dist <= damageDist)
				{
					HurtLogic::hurtHp(this, baseVec[i], true);
				}
			}
		}
	}
}

void Element::deadActionCall()
{
	BattleLayer::getInstance()->removeElement(this);
	if (data->modelId)
	{
		Point pt = Point(0, 0);
		Point &p = pt;
		BattleLayer::getInstance()->createOneElement(data->modelId,0,p);
	}

	if (this->data->dropPlan)
	{
		RandomLib::awardByPlanId(this->data->dropPlan, this);
	}

	if (this->getSlow())
	{
		this->setSlow(false);
		BattleLayer::getInstance()->resumePlaySpeed();
	}
}

void Element::setState(std::string state)
{
	_state = state;
	stopAllActions();

	if (state == "dead")
	{
		_normalBody->setVisible(false);
		_deadBody->setVisible(true);

		if (this->data->isSlow)
		{
			this->setSlow(true);
			BattleLayer::getInstance()->slowPlaySpeed();
		}

		if (this->data->isShock)
		{
			BattleLayer::getInstance()->startShake();
		}

		auto animation = AnimationCache::getInstance()->getAnimation("skill_explosion");
		if (animation)
		{
			Action* action = Animate::create(animation);
			auto endCall = CallFunc::create(CC_CALLBACK_0(Element::deadActionCall, this));
			action = Sequence::create(static_cast<FiniteTimeAction*>(action), endCall, NULL);
			_deadBody->runAction(action);
		}
		else{
			this->deadActionCall();
		}
		_deadBody->setScale(this->data->model->bombScale);
	}
	else
	{
		_normalBody->setVisible(true);
		_deadBody->setVisible(false);

		auto animation = AnimationCache::getInstance()->getAnimation(this->data->model->res + "_normal");// this->data->model->res + "_normal");
		if (animation)
		{
			Action* action = Animate::create(animation);
			action = RepeatForever::create(static_cast<ActionInterval*>(action));
			_normalBody->runAction(action);
		}
		
		/*
		auto drawnode = DrawNode::create();
		drawnode->drawPoint(Point::ZERO, 10, Color4F(1, 0, 0, 0.5));
		_normalBody->addChild(drawnode);
		*/
		_normalBody->setScale(this->data->model->selfScale);
		
	}
}

void Element::doAIAction(float time)
{

}

OBB2d* Element::getOBB2d()
{
	if (!obb2d)
	{
		obb2d = new OBB2d();
		for (size_t i = 0; i < 4; i++)
		{
			obb2d->setVertex(i, this->data->model->collison.at(i));
		}
	}

//	_drawNode->clear();
//	Vec2 p0 = this->data->model->collison.at(0) - this->getPosition();
//	Vec2 p1 = this->data->model->collison.at(1) - this->getPosition();
//	Vec2 p2 = this->data->model->collison.at(2) - this->getPosition();
//	Vec2 p3 = this->data->model->collison.at(3) - this->getPosition();
//	Vec2 points[] = { p0, p1, p2, p3 };
//	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));

	return obb2d;
}

int Element::getTemplateId()
{
	return _tid;
}

float Element::getCurrentHp()
{
	return data->hp;
}

void Element::setCurrentHp(const float &hp)
{
	if (!data->canDestroy)
	{
		return;
	}

	if (data->hp <= 0)
	{
		return;
	}

	data->hp = hp < 0 ? 0 : hp;
	
	if (_hpBar)
	{
		_hpBar->updateView(data->hp, data->maxHp);
	}

	if (data->hp <= 0)
	{
		dead();
	}
}

float Element::getFinalHp()
{
	return 0;
}

void Element::setFinalHp(const float &hp)
{
	
}

float Element::getAttack()
{
	return data->attack;
}

void Element::setAttack(const float &attack)
{
	data->attack = attack;
}

float Element::getDefense()
{
	return 0;
}

float Element::getShield()
{
	return 0;
}

void Element::setShield(const float &shp)
{

}

Rect Element::getTouchRect()
{
	/*
	Point depPoint0 = BattleLayer::getInstance()->positionForNode(Point(2 * (this->data->sceneX + this->data->model->depthSP[0]), 2 * (this->data->sceneY + this->data->model->depthSP[1])));
	Point depPoint1 = BattleLayer::getInstance()->positionForNode(Point(2 * (this->data->sceneX + this->data->model->depthEP[0]), 2 * (this->data->sceneY + this->data->model->depthSP[1])));
	Point depPoint2 = BattleLayer::getInstance()->positionForNode(Point(2 * (this->data->sceneX + this->data->model->depthEP[0]), 2 * (this->data->sceneY + this->data->model->depthEP[1])));
	Point depPoint3 = BattleLayer::getInstance()->positionForNode(Point(2 * (this->data->sceneX + this->data->model->depthSP[0]), 2 * (this->data->sceneY + this->data->model->depthEP[1])));
	Point thisPos = this->getPosition();

	_drawNode->clear();
	Vec2 p0 = depPoint2 - thisPos;
	Vec2 p1 = thisPos - thisPos;
	Vec2 p2 = BattleLayer::getInstance()->positionForTileCoord(Point(this->data->sceneX, this->data->sceneY)) - thisPos;
	Vec2 points[] = { p0, p1, p2 };

	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));
	*/

//	_drawNode->clear();

	float x0 = this->data->model->touchCenterX - this->data->model->touchWidth / 2;
	float y0 = this->data->model->touchCenterY - this->data->model->touchHeight / 2;
	/*
	float x1 = this->data->model->touchCenterX + this->data->model->touchWidth / 2;
	float y1 = this->data->model->touchCenterY + this->data->model->touchHeight / 2;

	Vec2 p0 = Vec2(x0, y0);
	Vec2 p1 = Vec2(x1, y0);
	Vec2 p2 = Vec2(x1, y1);
	Vec2 p3 = Vec2(x0, y1);
	Vec2 points[] = { p0, p1, p2, p3 };
	_drawNode->drawPolygon(points, sizeof(points) / sizeof(points[0]), Color4F(1, 0, 0, 0.5), 1, Color4F(0, 0, 1, 1));
	*/
	return Rect(x0, y0, this->data->model->touchWidth, this->data->model->touchHeight);
}

int Element::getMaterialType()
{
	return data->model->materialType;
}

bool Element::isQuest()
{
	return data->isQuest;
}

bool Element::isWarn()
{
	return data->isWarn;
}

cocos2d::Vec2 Element::getShadowPoint()
{
	auto position = this->getPosition();
	return Vec2(position.x + this->data->model->shadowX, position.y + this->data->model->shadowY);
}

void Element::releaseSkill()
{
	this->_skillCount--;
}

void Element::retainSkill()
{
	this->_skillCount++;
}

void Element::releaseBullet()
{
	this->_bulletCount--;
}

void Element::retainBullet()
{
	this->_bulletCount++;
}

void Element::releaseAttack()
{
	this->_attedCount--;
}

void Element::retainAttack()
{
	this->_attedCount++;
}

bool Element::canRecovey()
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

std::string& Element::getMiniSpriteNum()
{
	return data->miniSprite;
}