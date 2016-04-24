#include "GameData.h"
#include "Utils.h"
#include "BattleScene.h"
#include "PlatFormInterface.h"
USING_NS_CC;

GameData::GameData(){};
GameData::~GameData(){};
GameData::GameData(const GameData&){};


GameData* GameData::instance = nullptr;

GameData* GameData::getInstance()
{
	if (instance == nullptr)
	{
		instance = new GameData();
	}
	return instance;
}

void GameData::setGameDataForKey(int key, std::string value, int type)
{
	std::string _key = Utils::int2String(type * GD_PRE + key);
	std::unordered_map<std::string, std::string>::iterator itera = _gameDataMap.find(_key);
	if (itera != _gameDataMap.end())
	{
		_gameDataMap.erase(itera);
	}
	_gameDataMap.insert(make_pair(_key, value));
	setGameDataXMLForKey(key, value, type);
}

std::string GameData::getGameDataForKey(int key, int type)
{
	std::string _key = Utils::int2String(type * GD_PRE + key);
	std::unordered_map<std::string, std::string>::iterator itera = _gameDataMap.find(_key);
	if (itera == _gameDataMap.end())
	{
		return getGameDataXMLForKey(key, type);
	}
	return _gameDataMap.at(_key);
}

void GameData::setGameDataXMLForKey(int key, std::string value, int type)
{
	encode(value, key_str);
	std::string _key = Utils::int2String(type * GD_PRE + key);
	UserDefault::getInstance()->setStringForKey(_key.c_str(), value.c_str());//value.c_str() 
}

std::string GameData::getGameDataXMLForKey(int key, int type)
{
	std::string _key = Utils::int2String(type * GD_PRE + key);
	std::string value = UserDefault::getInstance()->getStringForKey(_key.c_str());
	decode(value, key_str);
	_gameDataMap.insert(make_pair(_key, value));
	return value;
}

char GameData::encodeChar(char c, int key)
{
	return c^key;
}

char GameData::decodeChar(char c, int key)
{
	return c^key;
}

void GameData::encode(std::string &str, std::string &str_key)
{
	int len = str.size();
	int keyLength = str_key.size();
	keyLength = len > keyLength ? len : keyLength;
	for (int i = 0; i<len; i++)
	{
		str[i] = encodeChar(str[i], str_key[i%keyLength]);
	}
}

void GameData::decode(std::string &str, std::string &str_key)
{
	int len = str.size();
	int keyLength = str_key.size();
	keyLength = len > keyLength ? len : keyLength;
	for (int i = 0; i<len; i++)
	{
		str[i] = decodeChar(str[i], str_key[i%keyLength]);
	}
}

int GameData::getMoney()
{
	std::string moneyStr = this->getGameDataForKey(GD_MONEY);
	if (moneyStr == "") return 0;
	return Utils::string2Int(moneyStr);
}

int GameData::addMoney(int num)
{
	if (num <= 0) return GD_MONEY_FUNC_ERROR;
	int money = getMoney();
	money = money + num;
	this->setGameDataForKey(GD_MONEY, Utils::int2String(money));
	return money;
}

int GameData::minusMoney(int num)
{
	if (num <= 0) return GD_MONEY_FUNC_ERROR;
	int money = getMoney();
	if (money < num)
	{
		PlatFormInterface::getInstance()->showPayLayout(gift_index_0);
		return GD_MONEY_FUNC_ERROR;
	}
	int leftMoney = money - num;
	this->setGameDataForKey(GD_MONEY, Utils::int2String(leftMoney));
	return leftMoney;
}

int GameData::addItemById(const int &itemId, const unsigned int &num)
{
	if (itemId == MONEY_ITEM_ID)
	{
		return addMoney(num);
	}
	else
	{
		int itemNum = this->getItemById(itemId);
		itemNum += num;
		GameData::getInstance()->setGameDataForKey(itemId, Utils::int2String(itemNum), GD_ITEM);
		return itemNum;
	}
}

int GameData::minusItemById(const int &itemId, const unsigned int &num)
{
	int itemNum = this->getItemById(itemId);
	if (itemNum < num)
	{
		return INVALID_FLAG;
	}
	itemNum -= num;
	GameData::getInstance()->setGameDataForKey(itemId, Utils::int2String(itemNum), GD_ITEM);
	return itemNum;
}

int GameData::getItemById(const int &itemId)
{
	auto itemStr = this->getGameDataForKey(itemId, GD_ITEM);
	return itemStr.empty() ? 0 : Utils::string2Int(itemStr);
}

bool GameData::isActiveFunc(int key)
{
	if (key < 0 || key >GD_MONEY) return false;
	return Utils::string2Int(this->getGameDataForKey(key)) != 0;
}