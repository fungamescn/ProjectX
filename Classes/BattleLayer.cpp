#include "BattleLayer.h"
#include "Base.h"
#include "Utils.h"
#include "Template.h"
#include "Astar.h"
#include "BulletPool.h"
#include "SkillExecutor.h"
#include "StageLayer.h"
#include "MasterPool.h"
#include "SimpleAudioEngine.h"
#include "PopupLayer.h"
#include "Config.h"
#include "BattleItem.h"
#include "BuffLogic.h"
#include "StarLevel.h"
#include "BattleScene.h"
#include "GameData.h"
#include "PlatFormInterface.h"

USING_NS_CC;

BattleLayer* BattleLayer::_instance = nullptr;

BattleLayer::BattleLayer() :_stageId(0)
{
	//外部输入操作监听
	this->initListener();

	//战斗流程事件监听
	_eventManager = new EventManager();

	//ai管理
	_aiMagager = new AIManager();
};

BattleLayer::~BattleLayer(){
	delete _eventManager;
	delete _aiMagager;
};

BattleLayer* BattleLayer::getInstance()
{
	if (!_instance)
	{
		srand((unsigned)time(NULL));
		_instance = BattleLayer::create();
		CCASSERT(_instance, "FATAL: Not enough memory");
	}
	return _instance;
}

bool BattleLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	return true;
}

void BattleLayer::initStage(int level)
{
	//add background
	_stageId = level;

	auto origin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto winSize = Director::getInstance()->getWinSize();

	_diagonalAngle = 180 + (Point(0, 0)-Point(winSize.width / 2, winSize.height / 2)).getAngle() * 180 / M_PI;

	META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);

	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("map/map_res.plist");
	//加载瓦片地图
	//_tileMap = TMXTiledMap::create("map/" + stageMeta["mapRes"] + ".tmx");
	//this->addChild(_tileMap, 0);

	//createStageTiels();

	/*auto tileWidth = _tileMap->getContentSize().width;
	auto tileHeight = _tileMap->getContentSize().height;
	*/
	//背景图
//	std::vector<Point> pointVec = Utils::getPointVecByStr(stageMeta["bgPosition"]);
//	for (size_t i = 0; i < 4; i++)
//	{
//		auto bg = Sprite::create("mapBg/jungle/jungle_"+Utils::int2String(i+1)+".jpg");
//		bg->setAnchorPoint(Point(0, 0));
//		bg->setPosition(pointVec[i]);
//		this->addChild(bg, BACKGROUND_T+i);
//	}
    
    _tileMap = Sprite::create("map/map.jpg");
    _tileMap->setAnchorPoint(Point(0, 0));
    _tileMap->setPosition(Point(0,0));
    //this->addChild(_tileMap,);
    this->addChild(_tileMap, 0);
	
	//寻路设置
	//PathFind::getTheOnlyInstance()->setTiledMap(_tileMap);

	//生成物件 地表物件 by jgd
	//this->initElements(level);

//	this->createBattleItem(1, Point(40, 10));

	//battleEvent数据初始化
	_eventManager->initStageData(level);

	//初始化设置强制目标为空
	this->setForceRole(NULL);
	
	_shakeFactor = -1;

	_slowPlayCount = 0;

	//点击图标
	_touchSprite = WarnSprite::createWarnSprite(4);
	this->addChild(_touchSprite, ROLEZORDER+1);
	_touchSprite->setPosition(400, 500);
	_touchSprite->setVisible(false);
	PlatFormInterface::getInstance()->record("关卡", "进入次数", _stageId, 1);
}

void BattleLayer::gameStart()
{
	this->scheduleUpdate();
	_battleTime = 0;
	StageLayer::getInstance()->startClock();
//	schedule(schedule_selector(BattleLayer::areaChecking), 1.0f);
	StageLayer::getInstance()->initSkills();
	BuffLogic::getInstance()->scheduleBuffs(true);
	StarLevel::getInstance()->init(_stageId);
	_eventManager->trigger(EventManager::EVENT_TYPE_GAME_START);
    
    schedule(schedule_selector(BattleLayer::refreshEnemy), 1.0f);
//	META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
}


void BattleLayer::refreshEnemy(float dt)
{
//    { "id" : "90000","stageId" : "1","count" : "1","preEvent" : "0","type" : "1","actionType" : "9","actionParams1" : "9000","actionParams2" : "0","actionParams3" : "0","actionParams4" : "0","nextEvent" : "0" },
//    { "id" : "90001","stageId" : "1","count" : "1","preEvent" : "0","type" : "7","condPrams1" : "48,3|52,19","actionType" : "9","actionParams1" : "9002","actionParams2" : "0","actionParams3" : "0","actionParams4" : "0","nextEvent" : "0" },
//    { "id" : "90002","stageId" : "1","count" : "1","preEvent" : "0","type" : "7","condPrams1" : "42,1|48,19","actionType" : "1","actionParams1" : "90000","actionParams2" : "3","actionParams3" : "35,7|35,15|38,11","actionParams4" : "4","nextEvent" : "0" },
//    { "id" : "90003","stageId" : "1","count" : "1","preEvent" : "0","type" : "3","condPrams1" : "90000","condPrams2" : "2","actionType" : "9","actionParams1" : "9004","actionParams2" : "0","actionParams3" : "0","actionParams4" : "0","nextEvent" : "0" },
//    { "id" : "90004","stageId" : "1","count" : "1","preEvent" : "0","type" : "2","condPrams1" : "90000","condPrams2" : "3","actionType" : "9","actionParams1" : "9006","actionParams2" : "0","actionParams3" : "0","actionParams4" : "0","nextEvent" : "0" },
//    { "id" : "90005","stageId" : "1","count" : "1","preEvent" : "0","type" : "7","condPrams1" : "31,4|37,16","actionType" : "9","actionParams1" : "9007","actionParams2" : "0","actionParams3" : "0","actionParams4" : "0","nextEvent" : "0" },
    int tid = 90000;
    
    int count = 1;
    
    Point points = Point(CCRANDOM_0_1() * 2300,CCRANDOM_0_1() *1000);
    //points = Point(1300,1000);
    int groupType = 4;
    
    //int tid = Utils::string2Int(eventMeta["actionParams1"]);
    //int count = Utils::string2Int(eventMeta["actionParams2"]);
    //vector<Point> points = getPointsByStr(eventMeta["actionParams3"]);
    //int groupType = Utils::string2Int(eventMeta["actionParams4"]);
    
    
    //BattleLayer::getInstance()->createOneMonster(tid, groupType, points);
    MasterPool::getInstance()->addMonsterToCreateQuque(tid, groupType, points, true);
}

void BattleLayer::refreshHireHeros()
{
	for (int i = 0; i < heroVec.size(); i++)
	{
		if (heroVec.at(i)->getCurrentHp() == 0)
		{
			for (int j = GD_TEAM_1; j <= GD_TEAM_4; j++)
			{
				std::string s = GameData::getInstance()->getGameDataForKey(j);
				int sid = s == "" ? 0 : Utils::string2Int(s);
				if ((sid + 1000) == heroVec.at(i)->getTemplateId())
				{
					GameData::getInstance()->setGameDataForKey(i + 1, "0");
				}
			}
		}
	}
}

void BattleLayer::gameOver(int result)
{
	moveFlag = true;
	this->unscheduleUpdate();
	StageLayer::getInstance()->stopClock();
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(MUSIC_BACKGROUD_FILE);
//	BuffLogic::getInstance()->scheduleBuffs(false);
//	BuffLogic::getInstance()->clearAll();
	

	if (result)
	{
		//评价
		StarLevel::getInstance()->trigger(STAR_TYPE_USE_TIME, 0, _battleTime);

		//胜利弹框
		PopupLayer::getInstance()->popup(BATTLE_POP_WIN);

		//保存数据 & 奖励
		StarLevel::getInstance()->save();

		//解锁新关卡
		BattleScene::getInstance()->unLockNextStage(_stageId);
		PlatFormInterface::getInstance()->record("关卡", "通关", _stageId, 1);
	}
	else
	{
		PopupLayer::getInstance()->popup(BATTLE_POP_LOSE);
		PlatFormInterface::getInstance()->adv_start();
	}
	
	if (_soundId >= 0)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(_soundId);
	}
	
}

void BattleLayer::cleanUp(int stage)
{
	SkillExecutor::getInstance()->clearAll();
	this->removeAllChildren();

	skillItemMap.clear();
	std::map<int, int>(skillItemMap).swap(skillItemMap);

	roleArray.clear();
	vector<Base *>(roleArray).swap(roleArray);

	MasterPool::getInstance()->clean();

	monsterMap.clear();
	std::map<string, std::vector<Base*>>(monsterMap).swap(monsterMap);

	elementMap.clear();
	std::map<string, std::vector<Base*>>(elementMap).swap(elementMap);

	elementZOrderList.clear();
	std::list<Base *>(elementZOrderList).swap(elementZOrderList);

	//阵营相关
	heroVec.clear();
	vector<Base *>(heroVec).swap(heroVec);

	justVec.clear();
	vector<Base *>(justVec).swap(justVec);
	
	evilVec.clear();
	vector<Base *>(evilVec).swap(evilVec);

	neutralVec.clear();
	vector<Base *>(neutralVec).swap(neutralVec);

	otherVec.clear();
	vector<Base *>(otherVec).swap(otherVec);

	_eventManager->clearLevelData();

	if (_stageId != stage && _stageId)
	{
		META_LINE keys = { { "stageId", Utils::int2String(_stageId) } };
		std::vector<META_LINE>  stageTiles = Template::getInstance()->getMetaVectorByKeys(TEMPLATE_STAGE_TILES, keys);
		for (auto stageTile : stageTiles)
		{
			Director::getInstance()->getTextureCache()->removeTextureForKey("map/" + stageTile["tiledSprite"] + ".png");

		}
	}
	META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
	if (stageMeta["plistStr"] != "")
	{
		vector<string> plistVec = Utils::str_split(stageMeta["plistStr"], "|");
		//_loadMaxNum = _loadMaxNum + plistVec.size();
		for (auto plist : plistVec)
		{
			vector<string> plistInfo = Utils::str_split(plist, "-");
			int type = plistInfo[0] == "m" ? TEMPLATE_MONSTER : TEMPLATE_ELEMENT_MODEL;
			META_LINE model = Template::getInstance()->getMetaById(type, atoi(plistInfo[1].c_str()));
			if (model["sound"] != "")
				CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(model["sound"].c_str());
			if (type == TEMPLATE_MONSTER)
			{
				Director::getInstance()->getTextureCache()->removeTextureForKey("monster_" + model["resId"] + ".png");
				int bulletId = atoi(model["bulletId"].c_str());
				META_LINE bulletMeta = Template::getInstance()->getMetaById(TEMPLATE_BULLET, bulletId);
				if (bulletMeta["sound"] != "")
					CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(bulletMeta["sound"].c_str());
			}
		}
	}
	_stageId = 0;

	BulletPool::getInstance()->clearAll();
	StageLayer::getInstance()->clearAll();

	PathFind::getTheOnlyInstance()->cleanDynamicBlock();

}

void BattleLayer::initElements(int stage)
{
	string stg = Utils::int2String(stage);
	META_TABLE metaTable = Template::getInstance()->getMetaTable(TEMPLATE_SCENE_ELEMENT);
	for (META_TABLE::iterator it = metaTable.begin(); it != metaTable.end(); ++it)
	{
		META_LINE metaLine = it->second;
		if (metaLine["stage"] == stg && metaLine["init"] == "1")
		{
			Point pt = cocos2d::Point(0, 0);
			Point &p = pt;
			createOneElement(Utils::string2Int(metaLine["id"]), Utils::string2Int(metaLine["groupType"]), p);
		}
	}
}

void BattleLayer::initTeam(std::vector<int> vec)
{
	if (_stageId<0)
	{
		return;
	}

	META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
	std::vector<Point> bornVec = Utils::getPointVecByStr(stageMeta["born"]);

	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] == 0)
		{
			continue;
		}

		Hero* player = Hero::create(1000+vec[i]);

		//Point pos = positionForTileCoord(bornVec[i]);
		player->setRolePosition(Point(1333,1100));
		player->setDirection(1);
		player->stand();
		this->addBase(player);
	}
//	this->adjustTeamPos(900, 700);
	
	moveFlag = true;
//	this->updateViewPointCenter(true);
	updateViewPointCenter2(true);
	//this->createMonsters(5);
}

void BattleLayer::adjustTeamPos(int x, int y)
{
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		int tx = x + i * 5;
		int ty = y + i * 5;
		Role* role = (Role*)heroVec.at(i);
		role->setRolePosition(Point(tx, ty));
	}
}

Sprite* BattleLayer::getTiledMap()
{
	return this->_tileMap;
}

void BattleLayer::addBase(Base * base)
{
	auto baseParent = base->getParent();
	if (!baseParent)
	{
		this->addChild(base);
	}
	else if (baseParent != this)
	{
		base->removeFromParentAndCleanup(false);
		this->addChild(base);
	}
	if (base->getBaseType() == BASE_TYPE_MONSTER)
	{
		string tid = Utils::int2String(base->getTemplateId());
		monsterMap[tid].push_back(base);
	}
	else if (base->getBaseType() == BASE_TYPE_ELEMENT)
	{
		string groupId = static_cast<Element*>(base)->getGroupId();
//		string tid = Utils::int2String(base->getTemplateId());
		elementMap[groupId].push_back(base);

		Point baseDep = base->getDepthPoint();
		Point itDep;
		std::list<Base *>::iterator it;
		for (it = elementZOrderList.begin(); it != elementZOrderList.end(); ++it)
		{
			itDep = (*it)->getDepthPoint();
			if (baseDep.y > itDep.y)
			{
				elementZOrderList.insert(it, base);
				break;
			}
		}

		if (it == elementZOrderList.end())
		{
			elementZOrderList.push_back(base);
		}
	}
	getBaseVectorByGroupType(base->getGroupType()).push_back(base);
	
	if (base->getBaseType()!=BASE_TYPE_ELEMENT)
		roleArray.push_back(base);
}

void BattleLayer::removeBase(Base* base)
{
	if (base->getBaseType() == BASE_TYPE_ELEMENT)
	{
		MasterPool::getInstance()->deadElementPtrArray.push_back((Element *)base);
		base->retain();
		this->removeChild(base, false);
	}
	else if (base->getBaseType() == BASE_TYPE_MONSTER || base->getBaseType() == BASE_TYPE_HERO)
	{
		MasterPool::getInstance()->deadMonsterPtrArray.push_back((Role *)base);
		base->retain();
		this->removeChild(base, false);
		//MasterPool::getInstance()->releaseDeadMonsterPtrArray();
	}
}

void BattleLayer::onBaseDead(Base* base)
{
	//清除强制目标
	if (this->getForceRole() == base)
	{
		base->aimSprite->setVisible(false);
		this->setForceRole(NULL);
		//_aimSprite->setVisible(false);
	}

	//从活着的role列表移除 （除英雄）
	if (!(base->getGroupType() & GROUP_TYPE_HERO))
	{
		for (std::vector<Base*>::iterator it = roleArray.begin(); it != roleArray.end(); ++it)
		{
			if (*it == base)
			{
				roleArray.erase(it);
				break;
			}
		}

		std::vector<Base *> &baseVec = getBaseVectorByGroupType(base->getGroupType());
		for (std::vector<Base*>::iterator it = baseVec.begin(); it != baseVec.end(); ++it)
		{
			if (*it == base)
			{
				//CCLOG(" getBaseVectorByGroupType(base->getGroupType()).size: %d", getBaseVectorByGroupType(base->getGroupType()).size());
				baseVec.erase(it);
				//CCLOG(" getBaseVectorByGroupType(base->getGroupType()).size1: %d", getBaseVectorByGroupType(base->getGroupType()).size());
				break;
			}
		}

		//其他维护的列表
		if (base->getBaseType() == BASE_TYPE_MONSTER)
		{
			string tid = Utils::int2String(base->getTemplateId());
			for (std::vector<Base*>::iterator it = monsterMap[tid].begin(); it != monsterMap[tid].end(); ++it)
			{
				if (*it == base)
				{
					monsterMap[tid].erase(it);
					break;
				}
			}

			base->setLocalZOrder(ROLEZORDER);
			if (static_cast<Role *>(base)->getAttribute()->getIsDeath())
			{
				MasterPool::getInstance()->deadMonsterArray.push_back((Role *)base);
				if (MasterPool::getInstance()->deadMonsterArray.size() > MAX_DEAD_BODY_NUM)
				{
					this->removeBase(MasterPool::getInstance()->deadMonsterArray.front());
					MasterPool::getInstance()->deadMonsterArray.pop_front();
				}
			}
		}
		else if (base->getBaseType() == BASE_TYPE_ELEMENT)
		{
			string groupId = static_cast<Element*>(base)->getGroupId();
//			string tid = Utils::int2String(base->getTemplateId());
			for (std::vector<Base*>::iterator it = elementMap[groupId].begin(); it != elementMap[groupId].end(); ++it)
			{
				if (*it == base)
				{
					elementMap[groupId].erase(it);
					break;
				}
			}

			for (std::list<Base *>::iterator it = elementZOrderList.begin(); it != elementZOrderList.end(); ++it)
			{
				if (*it == base)
				{
					elementZOrderList.erase(it);
					break;
				}
			}
		}

	}

	if (base->getBaseType() == BASE_TYPE_MONSTER)
	{
		StarLevel::getInstance()->trigger(STAR_TYPE_KILL_MONSTER, base->getTemplateId());
	}
	else if (base->getBaseType() == BASE_TYPE_ELEMENT)
	{
		StarLevel::getInstance()->trigger(STAR_TYPE_KILL_ELEMENT, base->getTemplateId());
	}
	else if (base->getBaseType() == BASE_TYPE_HERO)
	{
		StarLevel::getInstance()->trigger(STAR_TYPE_NO_DIE);
	}

	_eventManager->trigger(EventManager::EVENT_TYPE_BASE_DEAD, base);
}

std::vector<Base *> BattleLayer::getBasesList()
{
	return roleArray;
}

void BattleLayer::update(float t)
{
	if (isHeroAllDied())
	{
//		gameOver(0);
		return;
	}

	_battleTime += t;

	touchMove(t);
	updateViewZOrder();
	_aiMagager->exeAIOrders(t);
	updateTouchPointVisible();
//	updateViewPointCenter(false);
	updateViewPointCenter2();
	updateWarnSprite();
	updateCeateMonsterFromQueue();
	areaChecking(t);
	shakeUpdate();
}

void BattleLayer::updateViewZOrder()
{
	if (!Utils::checkDelayFrame(_zoderUpdate, 10)) return;
	Point elementDep;
	Point baseDep;

	for (size_t i = 0; i < roleArray.size(); i++)
	{
		auto &base = roleArray[i];

		baseDep = this->nodeForPosition( base->getDepthPoint() );
		base->setZOrder(MAX_ELEMENT_ZORDER);
		for (std::list<Base *>::iterator it = elementZOrderList.begin(); it != elementZOrderList.end(); ++it)
		{
			elementDep = this->nodeForPosition((*it)->getDepthPoint());
			if (baseDep.x <= elementDep.x && baseDep.y <= elementDep.y )
			{
				base->setZOrder((*it)->getZOrder() - 1);
				break;
			}
		}
	}
}

void BattleLayer::updateMonsterAction(float t)
{

}

Point BattleLayer::tileCoordForPosition(Point pt)
{
//	Size _mapTileSize = _tileMap->getTileSize();
//	int nodex = (pt.x * 2 / _mapTileSize.width - _tileMap->getMapSize().width + 1) / 2 - (pt.y * 2 / _mapTileSize.height - _tileMap->getMapSize().height * 2 + 2) / 2;
//	int nodey = -(pt.x * 2 / _mapTileSize.width - _tileMap->getMapSize().width + 1) / 2 - (pt.y * 2 / _mapTileSize.height - _tileMap->getMapSize().height * 2 + 2) / 2;
	return CC_POINT_POINTS_TO_PIXELS(pt);
    //return pt;
}

Point BattleLayer::positionForTileCoord(Point pos)
{
//	Size _mapTileSize = _tileMap->getTileSize();
//	Vec2 ret = Vec2(_mapTileSize.width / 2 * (_tileMap->getMapSize().width + pos.x - pos.y - 1),
//		_mapTileSize.height / 2 * ((_tileMap->getMapSize().height * 2 - pos.x - pos.y) - 2));
	return CC_POINT_PIXELS_TO_POINTS(pos);
    //return pos;
}

Point BattleLayer::nodeForPosition(Point pt)
{
    //return pt;
//	Size _mapTileSize = _tileMap->getTileSize();
//	int nodex = (pt.x * 4 / _mapTileSize.width - _tileMap->getMapSize().width * 2 + 1) / 2 - (pt.y * 4 / _mapTileSize.height - _tileMap->getMapSize().height * 4 + 2) / 2;
//	int nodey = -(pt.x * 4 / _mapTileSize.width - _tileMap->getMapSize().width * 2 + 1) / 2 - (pt.y * 4 / _mapTileSize.height - _tileMap->getMapSize().height * 4 + 2) / 2;
//
	return CC_POINT_POINTS_TO_PIXELS(pt);
}

Point BattleLayer::positionForNode(Point node)
{
//	Size _mapTileSize = _tileMap->getTileSize();
//	Vec2 ret = Vec2(_mapTileSize.width / 4 * (_tileMap->getMapSize().width * 2 + node.x - node.y - 1),
//		_mapTileSize.height / 4 * ((_tileMap->getMapSize().height * 4 - node.x - node.y) - 2));
	return CC_POINT_PIXELS_TO_POINTS(node);
    //return node;
 
}

void BattleLayer::updateViewPointCenter2(bool soon)
{
	if (!moveFlag) return;
	Point position;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		if (heroVec[i]->getCurrentHp() > 0)
		{
			position = heroVec[i]->getPosition();
			break;
		}
	}

	if (lastHeroPos == position)
	{
		return;
	}
	lastHeroPos = position;

	auto winSize = Director::getInstance()->getWinSize();
	auto mapPos = Point(winSize.width / 2, winSize.height / 2) - position;
	
	mapPos.x = MIN(0, mapPos.x);
	//mapPos.x = MAX(winSize.width - _tileMap->getTileSize().width * _tileMap->getMapSize().width, mapPos.x);
	mapPos.y = MIN(0, mapPos.y);
	//mapPos.y = MAX(winSize.height - _tileMap->getTileSize().height * _tileMap->getMapSize().height, mapPos.y);

	if (soon || Utils::getDistance(mapPos,this->getPosition()) > 500)
	{
		this->setPosition(mapPos);
	}
	else
	{
		auto thisPos = this->getPosition();
		auto offsetPoint = this->getPosition() - mapPos;
		if (offsetPoint.isZero())
		{
			return;
		}
		moveFlag = false;
		this->runAction(Sequence::create(MoveTo::create(0.4, mapPos), CallFunc::create(CC_CALLBACK_0(BattleLayer::updateViewPointCenterCallBack, this)), NULL));
	}
}

void BattleLayer::updateViewPointCenter(bool soon) {
	if (!moveFlag) return;
	auto winSize = Director::getInstance()->getWinSize();
	Point position = heroVec.at(0)->getPosition();
	int index = 0;
	if (heroVec.size() > 1 && static_cast<Role *>(heroVec.at(0))->getCurrentHp() <= 0)
	{
		for (unsigned int i = 1; i < heroVec.size(); i++)
		{
			if (static_cast<Role *>(heroVec.at(i))->getCurrentHp() > 0)
			{
				position = heroVec.at(i)->getPosition();
				index = i;
				break;
			}
		}
	}
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	int tileWidth = this->_tileMap->getContentSize().width;
	int tileHeight = this->_tileMap->getContentSize().height;
	x = MIN(x, (_tileMap->getContentSize().width * tileWidth) - winSize.width / 2);
	y = MIN(y, (_tileMap->getContentSize().height * tileHeight) - winSize.height / 2);
	auto actualPosition = Point(x, y);
	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - actualPosition;
	float speed = static_cast<Role *>(heroVec.at(index))->getSpeed();

	if (soon)
	{
		this->setPosition(viewPoint);
	}
	else if (abs(this->getPosition().x - viewPoint.x) >= tileWidth * 1)
	{
		moveFlag = false;
		float time = abs(viewPoint.x - this->getPosition().x) / (2 * speed * 60);
		time = time < 0.5 ? 0.5 : time;
		this->runAction(Sequence::create(MoveTo::create(time, viewPoint - Point(viewPoint.x, this->getPosition().y)), CallFunc::create(CC_CALLBACK_0(BattleLayer::updateViewPointCenterCallBack, this)), NULL));
	}
	else if (abs(this->getPosition().y - viewPoint.y) >= tileHeight * 1)
	{
		moveFlag = false;
		float time = abs(viewPoint.y - this->getPosition().y) / (2 * speed * 60);
		time = time < 0.5 ? 0.5 : time;
		this->runAction(Sequence::create(MoveTo::create(time, Point(this->getPosition().x, viewPoint.y)), CallFunc::create(CC_CALLBACK_0(BattleLayer::updateViewPointCenterCallBack, this)), NULL));
	}
}

void BattleLayer::updateViewPointCenterCallBack()
{
	moveFlag = true;
}

void BattleLayer::calcTilePointBoundary(cocos2d::Point &p)
{
	p.x = p.x < 0 ? 0 : p.x;
	p.x = p.x >= _tileMap->getContentSize().width ? _tileMap->getContentSize().width - 1 : p.x;
	p.y = p.y < 0 ? 0 : p.y;
	p.y = p.y >= _tileMap->getContentSize().height ? _tileMap->getContentSize().height - 1 : p.y;
}

int BattleLayer::getWeightDir(Point position, Point touchLocation)
{
	//Vec2 pt = nodeForPosition(position);
	//Vec2 endPt = nodeForPosition(touchLocation);
	float angle = Utils::vecAngleToX(touchLocation - position);
	if ( angle > M_PI ) angle = angle - M_PI;
	/*if ( angle <= M_PI /6 )
	{
		return 1;
	}
	else if (angle <= M_PI / 3)
	{
		return 4;
	}
	else if (angle <= M_PI * 2 /3)
	{
		return 3;
	}
	else if (angle <= M_PI * 5 /6)
	{
		return 4;
	}
	return 4;*/
	if ( angle == 0 )
	{
		return 1;
	}
	else if (angle == M_PI  /2)
	{
		return 3;
	}
	return 4;
}


void BattleLayer::setHeroDistancePoint(Point touchLocation)
{
	//CCLOG("my dis:%f, %f", tileCoordForPosition(touchLocation).x, tileCoordForPosition(touchLocation).y);
	int index = 1;
	bool firstHero = false;
	std::vector<Point> pvec;
	for (auto it : heroVec){
		Base * role = it;
		if (role->getBaseType() == BASE_TYPE_HERO)
		{
			vector<Point> ptVec = Utils::getDistancePoint(tileCoordForPosition(touchLocation), index);
			int blackNum = 0;
			if (!firstHero && role->getCurrentHp() > 0)
			{
				firstHero = true;
				int dirWeight = getWeightDir(role->getPosition(), touchLocation);
				PathFind::getTheOnlyInstance()->updateDirWieght(dirWeight);
				Point nodePoint = nodeForPosition(touchLocation);
				static_cast<Hero*>(role)->doFindPath(touchLocation);
			}else{
				for (unsigned int i = 0; i < ptVec.size(); i++)
				{
				Point tiledPoint = ptVec.at(i);
				BattleLayer::calcTilePointBoundary(tiledPoint);
				Point nodePoint = Point(tiledPoint.x * 2 + 1, tiledPoint.y * 2 + 1);
				if (PathFind::getTheOnlyInstance()->AstarIsBlock(nodePoint.x, nodePoint.y))
				{
					blackNum++;
					continue;
				}
				bool radX = rand() * 100 > 50;
				nodePoint = Point(tiledPoint.x * 2 + (radX ? 1 : 0), tiledPoint.y * 2 + (rand() * 100 > 50 ? 1 : 0));
				static_cast<Hero*>(role)->doFindPath(positionForNode(nodePoint));
				break;
				}
			}
			if (blackNum == ptVec.size())
			{
				ptVec = Utils::getDistancePointBak(tileCoordForPosition(touchLocation), index);
				for (unsigned int i = 0; i < ptVec.size(); i++)
				{
					Point tiledPoint = ptVec.at(i);
					BattleLayer::calcTilePointBoundary(tiledPoint);
					Point nodePoint = Point(tiledPoint.x * 2 + 1, tiledPoint.y * 2 + 1);
					if (PathFind::getTheOnlyInstance()->AstarIsBlock(nodePoint.x, nodePoint.y)) continue;
					bool radX = rand() * 100 > 50;
					nodePoint = Point(tiledPoint.x * 2 + (radX ? 1 : 0), tiledPoint.y * 2 + (rand() * 100 > 50 ? 1 : 0));
					if (pvec.size() > 0)
					{
						for (auto pt : pvec)
						{
							if (pt == nodePoint)
							{
								nodePoint = Point(tiledPoint.x * 2 + (!radX ? 1 : 0), tiledPoint.y * 2 + (rand() * 100 > 50 ? 1 : 0));
								break;
							}
						}
					}
					pvec.push_back(nodePoint);
					static_cast<Hero*>(role)->doFindPath(positionForNode(nodePoint));
					break;
				}
			}
			index++;
		}
	}
}

void BattleLayer::createOneElement(int tid, int groupType, Point &point)
{
	//return;
	Element* element = Element::create();
	element->setElementData(tid, groupType, point);

	element->setAnchorPoint(Point(0, 0));

	this->addBase(element);

	Point p1 = BattleLayer::getInstance()->positionForTileCoord(Point(element->data->sceneX, element->data->sceneY + 1));
	Point p2 = BattleLayer::getInstance()->positionForTileCoord(Point(element->data->sceneX + 1, element->data->sceneY + 1));
	element->setPosition(p1.x, p2.y);
	Point dp = element->getDepthPoint();
	element->setLocalZOrder(MAX_ELEMENT_ZORDER - dp.y);

	PathFind::getTheOnlyInstance()->addDynamicBlockArea(element->data->model->block);
}

void BattleLayer::createElements(int tid, int count, int groupType, vector<Point> &points)
{
	if (points.size() != count)
	{
		CCLOG("createElements error:params wrong!");
		return;
	}

	for (size_t i = 0; i < count; i++)
	{
		createOneElement(tid, groupType, points[i]);
	}

}

void BattleLayer::createElements(int tid, int count, int groupType, int randomId)
{
	META_LINE randomPoints = Template::getInstance()->getMetaById(TEMPLATE_RANDOM_POINTS, randomId);
	vector<cocos2d::Point> points = Utils::getPointVecByStr(randomPoints["main"]);
	int length = points.size() - 1;

	for (size_t i = 0; i < count; i++)
	{
		int index = random(0, length);
		createOneElement(tid, groupType, points[index]);
	}
}

void BattleLayer::removeElement(Element * element)
{
	this->onBaseDead(element);
	PathFind::getTheOnlyInstance()->removeDynamicBlockArea(element->data->model->block);
	this->removeBase(element);
}

Role* BattleLayer::createOneMonster(int tid, int groupType, cocos2d::Point point, bool isTiledPos /* = true */)
{
	Role* monster = MasterPool::getInstance()->getMonster();
	if (monster == NULL)
	{
		monster = Role::create(tid, groupType );
		_updateCreateMonsterMax = 3; //新建Role延迟3帧
		if (isTiledPos)
		{
			Point p = this->positionForTileCoord(point);
			monster->setRolePosition(p);
		}
		else
		{
			monster->setRolePosition(point);
		}
	}
	else{
		_updateCreateMonsterMax = 1;//修改Role延迟1帧
		monster->createRole(tid, groupType, point, isTiledPos /* = true */);
	}
	monster->stand();
	this->addBase(monster);
	return monster;
}

void BattleLayer::createMonsters(int tid, int count, int groupType, vector<Point> &points)
{

	//	vector<string> subPosStrVec = Utils::str_split(posStr, "|");

	if (points.size() != count)
	{
		CCLOG("createMonsters error:params wrong!");
		return;
	}

	//刷怪
	for (size_t i = 0; i < count; i++)
	{
		MasterPool::getInstance()->addMonsterToCreateQuque(tid, groupType, points[i], true);
		//createOneMonster(tid, groupType, points[i]);
	}
}

void BattleLayer::createMonsters(int tid, int count, int groupType, int randomId)
{
	META_LINE randomPoints = Template::getInstance()->getMetaById(TEMPLATE_RANDOM_POINTS, randomId);
	vector<cocos2d::Point> points = Utils::getPointVecByStr(randomPoints["main"]);
	int length = points.size() - 1;

	for (size_t i = 0; i < count; i++)
	{
		int index = random(0, length);
		MasterPool::getInstance()->addMonsterToCreateQuque(tid, groupType, points[index], true);
		//createOneMonster(tid, groupType, points[index]);
	}
}

void BattleLayer::createBattleItem(int tid, cocos2d::Point point,bool isTile/*=false*/)
{
	if (!BattleItem::canDrop(tid))
	{
		return;
	}

	auto battleItem = BattleItem::create();
	battleItem->setDataById(tid);
	
	if (isTile)
		battleItem->setPosition(this->positionForTileCoord(point));
	else
		battleItem->setPosition(point);
	
	this->addChild(battleItem);
	battleItem->setLocalZOrder(MAX_ELEMENT_ZORDER + 1);
	/*
	auto drawNode = DrawNode::create();
	drawNode->drawCircle(Point(0, 0), 10, 0, 360, false, Color4F(0, 0, 1, 1));
	drawNode->setPosition(this->positionForTileCoord(point));
	this->addChild(drawNode);
	*/
}

void BattleLayer::removeBattleItem(BattleItem* battleItem)
{
	int tid = battleItem->getTemplateId();
	_eventManager->trigger(EventManager::EVENT_TYPE_ITEM_DEAD, tid);
	this->removeChild(battleItem);
}

void BattleLayer::updateWarnSprite()
{
	if (!Utils::checkDelayFrame(_updateWarnSpriteCount, 30)) return;
	auto winSize = Director::getInstance()->getWinSize();
	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - this->getPosition();
	int index = -1;
	int index1 = -1;
	StageLayer::getInstance()->getDeadWarnSprite()->setVisible(false);
	for (unsigned int i = 0; i < heroVec.size(); i++)
	{
		if (static_cast<Role *>(heroVec.at(i))->getCurrentHp() > 0)
		{
			index = i;
		}
		else if (static_cast<Role *>(heroVec.at(i))->getCurrentHp() <= 0)
		{
			Point roleWorldPoint = convertToWorldSpace(static_cast<Role *>(heroVec.at(i))->getPosition());
			if (roleWorldPoint.x < 0 || roleWorldPoint.x > winSize.width || roleWorldPoint.y < 0 || roleWorldPoint.y > winSize.height)
			{
				index1 = i;
			}
		}
	}
	if (index1 != -1 && index != -1)
	{
		StageLayer::getInstance()->getDeadWarnSprite()->setVisible(true);
		int angle = (heroVec.at(index)->getPosition() - heroVec.at(index1)->getPosition()).getAngle() * 180 / M_PI;
		StageLayer::getInstance()->getDeadWarnSprite()->setRotation(angle <= 0 ? -180 - angle : 180 - angle);// >= 0 ? rotation : 360 + rotation);
		Vec2 vec = getWarnSpriteDev(angle);
		StageLayer::getInstance()->getDeadWarnSprite()->setPosition(Point(winSize.width / 2 + vec.x, winSize.height / 2 + vec.y));
	}
	if (index == -1)
	{
		StageLayer::getInstance()->getEnemyWarnSprite()->setVisible(false);
		StageLayer::getInstance()->getQuestWarnSprite()->setVisible(false);
		return;
	}
	//敌方
	StageLayer::getInstance()->getEnemyWarnSprite()->setVisible(true);
	Base * enemy = NULL;
	int typeArr[] = { GROUP_TYPE_EVIL, GROUP_TYPE_NEUTRAL };
	bool hasEvil = false;
	for (auto groupType : typeArr)
	{
		std::vector<Base *> baseVec = this->getBaseVectorByGroupType(groupType);
		for (std::vector<Base *>::iterator iter = baseVec.begin(); iter != baseVec.end(); ++iter)
		{
			Base * role = *iter;
			int cp = role->getCurrentHp();
			if ( role->getCurrentHp() > 0)
			{
				if(role->isWarn())hasEvil = true;
				Point roleWorldPoint = convertToWorldSpace(role->getPosition());
				if (roleWorldPoint.x > 0 && roleWorldPoint.x < winSize.width / 2 && roleWorldPoint.y > 0 && roleWorldPoint.y < winSize.height / 2)
				{
					StageLayer::getInstance()->getEnemyWarnSprite()->setVisible(false);
					break;
				}
				else if (role->isWarn() && StageLayer::getInstance()->getEnemyWarnSprite()->isVisible()){
					enemy = role;
				}
			}
		}
	}
	if (hasEvil && enemy != NULL && StageLayer::getInstance()->getEnemyWarnSprite()->isVisible())
	{
		int angle = (heroVec.at(index)->getPosition()- enemy->getPosition()).getAngle() * 180 / M_PI;
		StageLayer::getInstance()->getEnemyWarnSprite()->setRotation(angle <= 0 ? -180 - angle : 180 - angle);// >= 0 ? rotation : 360 + rotation);
		Vec2 vec = getWarnSpriteDev(angle);
		StageLayer::getInstance()->getEnemyWarnSprite()->setPosition(Point(winSize.width / 2 + vec.x, winSize.height / 2 + vec.y));
	}
	else if(!hasEvil)
	{
		StageLayer::getInstance()->getEnemyWarnSprite()->setVisible(false);
	}
	//任务
	StageLayer::getInstance()->getQuestWarnSprite()->setVisible(true);
	enemy = NULL;
	int typeArr1[] = { GROUP_TYPE_JUST, GROUP_TYPE_EVIL, GROUP_TYPE_NEUTRAL };
	hasEvil = false;
	for (auto groupType : typeArr1)
	{
		std::vector<Base *> baseVec = this->getBaseVectorByGroupType(groupType);
		for (std::vector<Base *>::iterator iter = baseVec.begin(); iter != baseVec.end(); ++iter)
		{
			Base * role = *iter;
			if (role->getCurrentHp() > 0)
			{
				if (role->isQuest())hasEvil = true;
				Point roleWorldPoint = convertToWorldSpace(role->getPosition());
				if (roleWorldPoint.x >= 0 && roleWorldPoint.x <= winSize.width / 2 && roleWorldPoint.y >= 0 && roleWorldPoint.y <= winSize.height / 2)
				{
					StageLayer::getInstance()->getQuestWarnSprite()->setVisible(false);
					break;
				}
				else if (role->isQuest() && StageLayer::getInstance()->getQuestWarnSprite()->isVisible()){
					enemy = role;
				}
			}
		}
	}
	if (hasEvil && enemy != NULL && StageLayer::getInstance()->getQuestWarnSprite()->isVisible())
	{
		int angle = (heroVec.at(index)->getPosition() - enemy->getPosition()).getAngle() * 180 / M_PI;
		StageLayer::getInstance()->getQuestWarnSprite()->setRotation(angle <= 0 ? -180 - angle : 180 - angle);// >= 0 ? rotation : 360 + rotation);
		Vec2 vec = getWarnSpriteDev(angle);
		StageLayer::getInstance()->getQuestWarnSprite()->setPosition(Point(winSize.width / 2 + vec.x, winSize.height / 2 + vec.y));
	}
	else if (!hasEvil)
	{
		StageLayer::getInstance()->getQuestWarnSprite()->setVisible(false);
	}
}

cocos2d::Vec2 BattleLayer::getWarnSpriteDev(int& angle)
{
	auto winSize = Director::getInstance()->getWinSize();
	Vec2 vec;
	if (angle <= 0)
	{
		if (angle < -_diagonalAngle && angle >= -(180 - _diagonalAngle))
		{
			vec.y = winSize.height / 2 - 50;
			vec.x = tan(M_PI*abs(90 - abs(angle)) / 180) * vec.y;
			if (angle >= -90)
			{
				vec.x = -vec.x;
			}
		}
		else if (angle >= -_diagonalAngle)
		{
			vec.x = winSize.width / 2 - 50;
			vec.y = vec.x  *  tan(M_PI*abs(angle) / 180);
			vec.x = -vec.x;
		}
		else
		{
			vec.x = winSize.width / 2 - 50;
			vec.y = vec.x * tan(M_PI*abs(abs(angle) - 180) / 180);
		}
	}
	else{
		if (angle >_diagonalAngle && angle <= 180 - _diagonalAngle)
		{
			vec.y = winSize.height / 2 - 50;
			vec.x = tan(M_PI*abs(90 - abs(angle)) / 180) * vec.y;
			if (angle <= 90)
			{
				vec.x = -vec.x;
			}
			vec.y = -vec.y;
		}
		else if (angle <= _diagonalAngle)
		{
			vec.x = winSize.width / 2 - 50;
			vec.y = -vec.x * tan(M_PI* angle / 180);
			vec.x = -vec.x;
		}
		else
		{
			vec.x = winSize.width / 2 - 50;
			vec.y = -vec.x * tan(M_PI*abs(angle - 180) / 180);
		}
	}
	return vec;
}

void BattleLayer::onSingleCLick(cocos2d::Point touchPoint)
{
	auto touchLocation = touchPoint;
	auto touchLocation2 = this->convertToNodeSpace(touchLocation);
	if (this->getForceRole())this->getForceRole()->aimSprite->setVisible(false);
	//_aimSprite->setVisible(false);
	this->setForceRole(NULL);
	this->setHeroDistancePoint(touchLocation2);
	_touchSprite->setPosition(touchLocation2 + Vec2(20, 10));
	_touchSprite->setVisible(true);
}

void BattleLayer::onDoubleClick(cocos2d::Point touchPoint)
{
	auto touchLocation = touchPoint;
	auto touchLocation2 = this->convertToNodeSpace(touchLocation);
	if (this->getForceRole())this->getForceRole()->aimSprite->setVisible(false);
	//_aimSprite->setVisible(false);
	this->setForceRole(NULL);
	this->setHeroDistancePoint(touchLocation2);
	_touchSprite->setPosition(touchLocation2 + Vec2(20, 10));
	_touchSprite->setVisible(true);
}

void BattleLayer::callBackPlayer(Hero * hero)
{
	auto p1t = hero->getPosition();
	auto winSize = Director::getInstance()->getWinSize();
	Point pt = convertToNodeSpace(Point(winSize.width / 2, winSize.height / 2));
	Point disNodePoint = nodeForPosition(pt);
	/*if (PathFind::getTheOnlyInstance()->AstarIsBlock(disNodePoint.x, disNodePoint.y))
	{
		bool follow = false;
		for (auto member : heroVec)
		{
			if (member != hero && member->getCurrentHp() > 0)
			{
				pt = member->getPosition();
				follow = true;
				break;
			}
		}
		if (follow)
		{
			hero->doFindPath(pt);
		}
	}
	else
	{
		hero->doFindPath(pt);
	}*/
	for (auto member : heroVec)
	{
		if (member != hero && member->getCurrentHp() > 0)
		{
			pt = member->getPosition();
			break;
		}
	}
	if (p1t.getDistanceSq(pt) > 22500)
	{
		hero->setRolePosition(pt);
	}
}

void BattleLayer::areaChecking(float dt)
{
	if (!Utils::checkDelayFrame(_updateAreaChecking, 10)) return;
	_eventManager->areaChecking();
}

bool BattleLayer::isHeroAllDied()
{
	if (heroVec.size() == 0)
	{
		return false;
	}

	for (size_t i = 0; i < heroVec.size(); i++)
	{
		if (heroVec[i]->getCurrentHp() > 0)
		{
			return false;
		}
	}
	return true;
}

std::vector<Base *>& BattleLayer::getBaseVectorByGroupType(int groupType)
{
	if (groupType == GROUP_TYPE_HERO) return heroVec;
	if (groupType == GROUP_TYPE_JUST) return justVec;
	if (groupType == GROUP_TYPE_EVIL) return evilVec;
	if (groupType == GROUP_TYPE_NEUTRAL) return neutralVec;
	if (groupType == GROUP_TYPE_OTHER) return otherVec;
	return heroVec;
}

bool BattleLayer::onClickBlack(cocos2d::Point touchPoint)
{
	auto touchLocation = touchPoint;
	auto touchLocation2 = this->convertToNodeSpace(touchLocation);
	Point disNodePoint = nodeForPosition(touchLocation2);
	Point mapPosition = this->getPosition();
	bool hasFroceRole = false;
	int typeArr[] = { GROUP_TYPE_EVIL, GROUP_TYPE_NEUTRAL };
	for (auto groupType : typeArr)
	{
		if (hasFroceRole) break;
		std::vector<Base *> baseVec = this->getBaseVectorByGroupType(groupType);
		for (std::vector<Base *>::iterator iter = baseVec.begin(); iter != baseVec.end(); ++iter)
		{
			Base * role = *iter;
			if (!role->canAim())
			{
				continue;
			}
			if (role->getCurrentHp() > 0)
			{
				Point rolePosition = role->getPosition();
				Point roleLocation = mapPosition + rolePosition;
				Point rectPoint = touchLocation - roleLocation;

				Rect touchRect = role->getTouchRect();
				if (touchRect.containsPoint(rectPoint))
				{
					if (this->getForceRole()) this->getForceRole()->aimSprite->setVisible(false);
					this->setForceRole(role);
					role->aimSprite->setVisible(true);
					hasFroceRole = true;
					_touchSprite->setVisible(false);
					break;
				}
			}
		}
	}
	if (hasFroceRole)
	{
		for (auto it = roleArray.begin(); it != roleArray.end(); it++){
			Base * role = *it;
			if (role->getBaseType() == BASE_TYPE_HERO)
			{
				static_cast<Role*>(role)->clearFindPathList();
			}
		}
		return true;
	}
	//else if (PathFind::getTheOnlyInstance()->AstarIsBlock(disNodePoint.x, disNodePoint.y))
	//{
	//	_touchSprite->setVisible(false);
	//	return true;
	//}
	/*else{
		for (auto it = roleArray.begin(); it != roleArray.end(); it++){
			Base * role = *it;
			if (role->getBaseType() == BASE_TYPE_HERO)
			{
				static_cast<Role*>(role)->clearFindPathList();
				Point ps = role->getPosition();
				if (role->getCurrentHp() >0 && PathFind::getTheOnlyInstance()->AstarIsBlock(nodeForPosition(ps).x, nodeForPosition(ps).y))
				{
					for (auto role1 : roleArray)
					{
						if (!PathFind::getTheOnlyInstance()->AstarIsBlock(nodeForPosition(role1->getPosition()).x, nodeForPosition(role1->getPosition()).y))
						{
							static_cast<Role*>(role)->setRolePosition(role1->getPosition());
						}
					}
				}
			}
		}
	}*/
	return false;
}

bool BattleLayer::cleanOldChild(int level)
{
	if (!_stageId) return true;
	if (level != _stageId)
	{
		META_LINE stageMeta = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
		std::vector<Point> pointVec = Utils::getPointVecByStr(stageMeta["bgPosition"]);
		for (size_t i = 0; i < 4; i++)
		{
			//this->removeChild(this->getChildByTag(BACKGROUND_T + i));
			TextureCache::getInstance()->removeTextureForKey("mapBg/jungle/jungle_" + Utils::int2String(i + 1) + ".jpg");
		}
		//this->removeChild(_tileMap);
		return true;
	}
	return false;
}

void BattleLayer::updateTouchPointVisible()
{
	if (!Utils::checkDelayFrame(_updateTouchWarn, 20)) return;
	if (_touchSprite->isVisible())
	{
		int count = 0;
		for (auto hero : heroVec)
		{
			if (static_cast<Hero *>(hero)->getFindPathListNum() <= 0)
			{
				count++;
			}
		}
		if (count == heroVec.size()) _touchSprite->setVisible(false);
	}
}

void BattleLayer::updateCeateMonsterFromQueue()
{
	if (!Utils::checkDelayFrame(_updateCreateMonster, _updateCreateMonsterMax)) return;
	if (MasterPool::getInstance()->getCreateMonsterLen() > 0)
	{
		MonsterCreateData mData = MasterPool::getInstance()->getNextCreateMonster();
		createOneMonster(mData.tid, mData.gType, Point(mData.pointX, mData.pointY), mData.isTiledPos);
	}
}

cocos2d::Vec2 BattleLayer::getNearVec2(cocos2d::Vec2 position, cocos2d::Vec2 position2, vector<cocos2d::Vec2> vecs)
{
	int minLen = MAX_NUM;
	cocos2d::Vec2 nearVec;
	if (Vec2::isSegmentIntersect(position, position2, vecs[0], vecs[1]))
	{
		nearVec = Vec2::getIntersectPoint(position, position2, vecs[0], vecs[1]);
	}
	int index = 1;
	while (index <= 3)
	{
		if (Vec2::isSegmentIntersect(position, position2, vecs[index], vecs[index + 1]))
		{
			cocos2d::Vec2 nearVec1 = Vec2::getIntersectPoint(position, position2, vecs[index], vecs[index + 1]);
			if (nearVec.isZero() || nearVec1.getDistanceSq(position) < nearVec.getDistanceSq(position))
			{
				nearVec = nearVec1;
			}

		}
		index++;
	}
	if (nearVec.isZero()) return position;
	return nearVec;
}

int compare(META_LINE p1, META_LINE  p2)
{
	return Utils::string2Int(p1["id"].c_str()) <= Utils::string2Int(p2["id"].c_str());
}

void BattleLayer::createStageTiels()
{
	int childIndex = 1;
	META_LINE keys = { { "stageId", Utils::int2String(_stageId) } };
	std::vector<META_LINE>  stageTiles = Template::getInstance()->getMetaVectorByKeys(TEMPLATE_STAGE_TILES, keys);
	sort(stageTiles.begin(), stageTiles.end(), compare);
	for (META_LINE stageTile : stageTiles)
	{
		int allTiles = atoi(stageTile["allTiles"].c_str());
		if (allTiles == 1)
		{
			auto batchNode1 = SpriteBatchNode::create("map/" + stageTile["tiledSprite"] + ".png");
			this->addChild(batchNode1);
			for (int i = 0; i < _tileMap->getContentSize().width;)
			{
				for (int j = 1; j <= _tileMap->getContentSize().height;)
				{
					Vec2 pt = this->positionForTileCoord(Vec2(i + 4, j + 6));
					Sprite *tileSprite = Sprite::create("map/" + stageTile["tiledSprite"] + ".png");
					tileSprite->setAnchorPoint(Vec2(0, 0));
					batchNode1->addChild(tileSprite);
					tileSprite->setPosition(pt);
					tileSprite->setScale(SACLE_XY);
					j = j + 6;
				}
				i = i + 6;
			}
		}
		else{
			vector<string> posStrVec = Utils::str_split(stageTile["tilePosStr"].c_str(), "|");
			if (posStrVec.size() >= BATCH_NUM)
			{
				auto batchNode1 = SpriteBatchNode::create("map/" + stageTile["tiledSprite"] + ".png");
				this->addChild(batchNode1);
				for (auto posStr : posStrVec)
				{
					vector<float> posVec = Utils::str_split_f(posStr, ",");
					Vec2 pt = this->positionForTileCoord(Vec2(posVec[0], posVec[1]+1));
					Sprite *tileSprite = Sprite::create("map/" + stageTile["tiledSprite"] + ".png");
					tileSprite->setAnchorPoint(Vec2(0, 0));
					batchNode1->addChild(tileSprite);
					tileSprite->setPosition(pt);
					tileSprite->setScale(SACLE_XY);
				}
			}
			else
			{
				for (auto posStr : posStrVec)
				{
					vector<float> posVec = Utils::str_split_f(posStr, ",");
					Vec2 pt = this->positionForTileCoord(Vec2(posVec[0], posVec[1]+1));
					Sprite *tileSprite = Sprite::create("map/" + stageTile["tiledSprite"] + ".png");
					tileSprite->setAnchorPoint(Vec2(0, 0));
					this->addChild(tileSprite);
					tileSprite->setPosition(pt);
					tileSprite->setScale(SACLE_XY);
				}
			}
		}
	}
}

void BattleLayer::slowPlaySpeed()
{
	_slowPlayCount++;
	updateTimeScale();
}

void BattleLayer::resumePlaySpeed()
{
	_slowPlayCount--;

	if (_slowPlayCount < 0)
	{
		_slowPlayCount = 0;
	}

	updateTimeScale();
}

void BattleLayer::updateTimeScale()
{
	if (_slowPlayCount > 0)
	{
		CCDirector::getInstance()->getScheduler()->setTimeScale(0.2);
	}
	else
	{
		CCDirector::getInstance()->getScheduler()->setTimeScale(1);
	}
}

void BattleLayer::shakeUpdate()
{
	if (_shakeFactor < 0)
	{
		return;
	}

	if (_shakeFactor == 0)
	{
		this->setPosition(_shakeOriginalPos);
		_shakeFactor--;
		return;
	}

	if (_shakeDelay * CCDirector::getInstance()->getScheduler()->getTimeScale() < 1)
	{
		_shakeDelay++;
		return;
	}

	this->setPositionX(_shakeOriginalPos.x + (rand() % _shakeFactor) / 5 - _shakeFactor / 10);
	this->setPositionY(_shakeOriginalPos.y + (rand() % _shakeFactor) / 5 - _shakeFactor / 10);
	_shakeFactor = _shakeFactor - 5;
}

void BattleLayer::startShake()
{
	_shakeFactor = 100;
	_shakeOriginalPos = this->getPosition();
	_shakeDelay = 1;
}

void BattleLayer::upMoneySprite(int num)
{
	for (auto hero : heroVec)
	{
		if (hero->getCurrentHp() > 0)
		{
			static_cast<Hero *>(hero)->upMoneySprite(num);
			break;
		}
	}
}

void BattleLayer::addSkillItemById(int itemId, int num)
{
	if (!skillItemMap[itemId])
	{
		skillItemMap[itemId] = 0;
	}
	skillItemMap[itemId] += num;
}

int BattleLayer::minusSkillItem(int itemId,int num)
{
	if (!skillItemMap[itemId])
	{
		skillItemMap[itemId] = 0;
	}

	auto itemNum = GameData::getInstance()->getItemById(itemId);
	auto result = skillItemMap[itemId] + itemNum - num;
	if (result < 0)
	{
		return INVALID_FLAG;
	}
	
	if (skillItemMap[itemId] >= num)
	{
		skillItemMap[itemId] -= num;
	}
	else
	{
		skillItemMap[itemId] = 0;
		auto leftNum = num - skillItemMap[itemId];
		GameData::getInstance()->minusItemById(itemId, leftNum);
	}

	return result;
}

int BattleLayer::getSkillItemNum(int itemId)
{
	if (!skillItemMap[itemId])
	{
		skillItemMap[itemId] = 0;
	}

	auto itemNum = GameData::getInstance()->getItemById(itemId);
	return itemNum + skillItemMap[itemId];
}

void BattleLayer::refreshHeroProp(int roleId)
{
	for (auto hero : heroVec)
	{
		static_cast<Hero *>(hero)->refreshProp(roleId);
	}
}

std::string BattleLayer::getMiniMap()
{
	META_LINE stage = Template::getInstance()->getMetaById(TEMPLATE_STAGE, _stageId);
	return stage["miniMap"];
}
