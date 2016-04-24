#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__

#include "cocos2d.h"
#include "Config.h"
//角色数据管理类

class GameData
{
public:
	static GameData* getInstance();
	//设置gameData
	void setGameDataForKey(int key, std::string value, int type = GD);
	//获取gameData
	std::string getGameDataForKey(int key, int type = GD);
	//增加游戏币或者钻石
	int addMoney(int num);
	//减少游戏币或者钻石
	int minusMoney(int num);
	//获取钱
	int getMoney();
	int addItemById(const int &itemId, const unsigned int &num);
	int minusItemById(const int &itemId, const unsigned int &num);
	int getItemById(const int &itemId);
	//检测消耗激活功能是否已激活
	bool isActiveFunc(int key);

private:
	GameData();
	~GameData();
	GameData(const GameData&);
	GameData& operator=(const GameData&);
	static GameData* instance;

	void setGameDataXMLForKey(int key, std::string value, int type);
	std::string getGameDataXMLForKey(int key, int type);
	char encodeChar(char c, int key);
	char decodeChar(char c, int key);
	void decode(std::string &str, std::string &str_key);
	void encode(std::string &str, std::string &str_key);

private:
	std::unordered_map<std::string, std::string> _gameDataMap;
	std::string key_str = "WQWISSB@QEEDOO.COM~(&.&)";

};
#endif