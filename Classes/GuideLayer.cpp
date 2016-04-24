#include "GuideLayer.h"
#include "BattleScene.h"
#include "Template.h"
#include "Utils.h"
#include "BattleLayer.h"
#include "cocostudio/CocoStudio.h"
#include "GameData.h"

GuideLayer * GuideLayer::_instance = nullptr;

bool GuideLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	_clipper = ClippingNode::create();
	_clipper->setInverted(true);
	_clipper->setAnchorPoint(Vec2::ZERO);
	_clipper->setPosition(0, 0);

	auto winSize = CCDirector::getInstance()->getWinSize();
	_content = LayerColor::create(Color4B(0, 0, 0, 180), winSize.width, winSize.height);
	_content->setAnchorPoint(Vec2::ZERO);
	_content->setPosition(0, 0);
	_clipper->addChild(_content);

	_stencil = DrawNode::create();
	Vec2 rect[4];
	rect[0] = Vec2(0, 0);
	rect[1] = Vec2(0, 100);
	rect[2] = Vec2(100, 100);
	rect[3] = Vec2(100, 0);
	Color4F green(0, 1, 0, 1);
	_stencil->drawPolygon(rect, 4, green, 0, green);
	_stencil->setAnchorPoint(Vec2::ZERO);
	_stencil->setPosition(0, 0);
	_clipper->setStencil(_stencil);
	this->addChild(_clipper);

	_fingerImg = Sprite::create();
	_fingerImg->setAnchorPoint(Vec2::ZERO);
	this->addChild(_fingerImg);

	_descLabel = Label::create();
	_descLabel->setColor(Color3B::YELLOW);
	_descLabel->enableShadow(Color4B::BLACK);
	_descLabel->setSystemFontSize(32);
	_descLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(_descLabel);

	_dialogLayer = CSLoader::createNode("ui/Guide.csb");
	this->addChild(_dialogLayer);
	_dialogText = static_cast<ui::Text *>(_dialogLayer->getChildByName("Guide_text"));

	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(GuideLayer::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, -130);
	_listener->setEnabled(false);
	return true;
}

GuideLayer * GuideLayer::getInstance()
{
	if (!_instance)
	{
		_instance = GuideLayer::create();
		_instance->retain();
	}
	return _instance;
}

void GuideLayer::setStencilPos(Vec2 pos)
{
	_stencil->setPosition(pos);
}

void GuideLayer::setStencilSize(Size size)
{
	_stencil->clear();
	Vec2 rect[4];
	rect[0] = Vec2(0, 0);
	rect[1] = Vec2(0, size.height);
	rect[2] = Vec2(size.width, size.height);
	rect[3] = Vec2(size.width, 0);
	Color4F green(0, 1, 0, 1);
	_stencil->setContentSize(size);
	_stencil->drawPolygon(rect, 4, green, 0, green);
}

void GuideLayer::setFingerPos(Vec2 pos)
{
	_fingerImg->setPosition(pos);
}

void GuideLayer::setDescContent(std::string content, float px, float py)
{
	if (content == "")
	{
		_descLabel->setVisible(false);
	}
	else
	{
		_descLabel->setVisible(true);
		_descLabel->setString(content);
		_descLabel->setPosition(Vec2(px, py));
	}
	
}

void GuideLayer::show(float time, int returnId, int nextId, std::string fingerName)
{
	if (!this->getParent())
	{
		BattleScene::getInstance()->addChild(this, 10000);
	}
	
	_listener->setEnabled(true);

	if (time > 0)
	{
		_returnId = returnId;
		scheduleOnce(schedule_selector(GuideLayer::timeOverHander), time);	
	}

	_nextId = nextId;

	_fingerImg->stopAllActions();
	if (fingerName == "")
	{
		_fingerImg->setVisible(false);
	}
	else
	{
		_fingerImg->setVisible(true);
		auto animation = AnimationCache::getInstance()->getAnimation(fingerName);
		Action* action = Animate::create(animation);
		action = RepeatForever::create(static_cast<ActionInterval*>(action));
		_fingerImg->runAction(action);
	}

	auto heroVec = BattleLayer::getInstance()->heroVec;
	for (size_t i = 0; i < heroVec.size(); i++)
	{
		auto hero = static_cast<Role*>(heroVec.at(i));
		hero->clearFindPathList();
	}
}

void GuideLayer::timeOverHander(float at)
{
	showById(_returnId);
}

void GuideLayer::hide()
{
	BattleScene::getInstance()->removeChild(this);
	_listener->setEnabled(false);
	unschedule(schedule_selector(GuideLayer::timeOverHander));
}

void GuideLayer::showById(int id)
{
	auto completeFlag = GameData::getInstance()->getGameDataForKey(id / 100, GD_GUIDE);
	if (completeFlag == "1")
	{
		return;
	}

	BattleLayer::getInstance()->setRoleEnable(false);

	
	auto meta = Template::getInstance()->getMetaById(TEMPLATE_GUIDE, id);

	_id = id;
	_guideEnd = Utils::string2Int(meta["guideEnd"]);

	if (meta["guideType"]=="1")//蒙版+开口
	{
		auto locatePoint = getLocatePoint(meta["locateType"], meta["locateId"]);
		
		auto stencilX = Utils::string2Float(meta["stencilX"]);
		auto stencilY = Utils::string2Float(meta["stencilY"]);
		setStencilPos(locatePoint + Vec2(stencilX, stencilY));

		auto stencilW = Utils::string2Float(meta["stencilW"]);
		auto stencilH = Utils::string2Float(meta["stencilH"]);
		setStencilSize(Size(stencilW, stencilH));

		auto fingerX = Utils::string2Float(meta["fingerX"]);
		auto fingerY = Utils::string2Float(meta["fingerY"]);
		setFingerPos(locatePoint + Vec2(fingerX, fingerY));

		auto descContent = meta["desc"];
		auto descX = Utils::string2Float(meta["descX"]);
		auto descY = Utils::string2Float(meta["descY"]);
		setDescContent(descContent, locatePoint.x + descX, locatePoint.y + descY);

		_stencil->setVisible(true);
		_fingerImg->setVisible(true);
		_descLabel->setVisible(true);
		_dialogLayer->setVisible(false);
	}
	else//面板
	{
		_stencil->setVisible(false);
		_fingerImg->setVisible(false);
		_descLabel->setVisible(false);
		_dialogLayer->setVisible(true);

		_dialogIndex = 0;
		_dialogVec = Utils::str_split(meta["desc"], "|");
		_dialogText->setString(_dialogVec.at(_dialogIndex));
	}

	auto time = Utils::string2Float(meta["time"]);
	auto returnId = Utils::string2Int(meta["returnId"]);
	auto nextId = Utils::string2Int(meta["nextId"]);
	auto fingerName = meta["fingerName"];
	show(time, returnId, nextId, fingerName);
}

bool GuideLayer::onTouchBegan(Touch* touch, Event* event)
{
	if (!_dialogVec.empty())
	{
		_dialogIndex++;
		if (_dialogVec.size() > _dialogIndex)
		{
			_dialogText->setString(_dialogVec.at(_dialogIndex));
		}
		else
		{
			_dialogVec.clear();
			_dialogIndex = 0;
			BattleLayer::getInstance()->setRoleEnable(true);

			if (_guideEnd)
			{
				GameData::getInstance()->setGameDataForKey(_id / 100, "1", GD_GUIDE);
			}

			this->hide();

			if (_nextId > 0)
			{
				this->showById(_nextId);
			}
//			else
//			{
//				this->hide();
//			}
		}
		_listener->setSwallowTouches(true);
		return true;
	}

	auto point = Director::getInstance()->convertToGL(touch->getLocationInView());//获得当前触摸的坐标  
	auto rect = Rect(_stencil->getPositionX(), _stencil->getPositionY(), _stencil->getContentSize().width, _stencil->getContentSize().height);
	if (rect.containsPoint(point))//如果触点处于rect中  
	{
		if (_guideEnd)
		{
			GameData::getInstance()->setGameDataForKey(_id / 100, "1", GD_GUIDE);
		}

		BattleLayer::getInstance()->setRoleEnable(true);
		_listener->setSwallowTouches(false);
		this->hide();
		if (_nextId > 0)
		{
			this->showById(_nextId);
		}
//		else
//		{
//			this->hide();
//		}
	}
	else
	{
		_listener->setSwallowTouches(true);
	}
	return true;
}

Point GuideLayer::getLocatePoint(string locateType, string locateId)
{
	if (locateType == "1")
	{
		auto elements =  BattleLayer::getInstance()->getElementsByGroupId(locateId);
		if (!elements.empty())
		{
			return elements[0]->getShadowPoint() + BattleLayer::getInstance()->getPosition();
		}
	}
	else if (locateType == "2")
	{
		auto monsters = BattleLayer::getInstance()->getMonstersByTid(locateId);
		if (!monsters.empty())
		{
			return monsters[0]->getShadowPoint() + BattleLayer::getInstance()->getPosition();
		}
	}
	return Point::ZERO;
}