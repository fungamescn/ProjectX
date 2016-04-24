#ifndef __PLATFORMINTERFACE_H__
#define __PLATFORMINTERFACE_H__


#include "cocos2d.h"

typedef struct{
	std::string payPoint; //编号
	std::string payPrice; //单位为分
}ST_PAY;

enum GIFT_INDEX
{
	gift_index_0 = 0, // 金币大礼包
	gift_index_1, // 豪华大礼包
	gift_index_2, // 战队大礼包	解锁火焰兵
	gift_index_3, // 战队大礼包	解锁狙击兵
	gift_index_4, // 战队大礼包	解锁榴弹兵
	gift_index_5, // 火焰兵大礼包
	gift_index_6, // 狙击兵大礼包
	gift_index_7, // 榴弹兵大礼包
	gift_index_8, // 升级大礼包	步枪兵一键满级
	gift_index_9, // 升级大礼包	火焰兵一键满级
	gift_index_10, // 升级大礼包	狙击兵一键满级
	gift_index_11, // 升级大礼包	榴弹兵一键满级
	gift_index_12, // 升级大礼包	技能（生命%）一键满级
	gift_index_13, // 升级大礼包	技能（伤害%）一键满级
	gift_index_14, // 升级大礼包	技能（防御%）一键满级
	gift_index_15, // 升级大礼包	技能（速度%）一键满级
	gift_index_16, // 道具大礼包	医疗包*1
	gift_index_17, // 道具大礼包	核弹*1
	gift_index_18, // 道具大礼包	机枪塔*1
	gift_index_19, // 道具大礼包	空袭导弹*1
	gift_index_20, // 道具大礼包	医疗包*5
	gift_index_21, // 道具大礼包	核弹*5
	gift_index_22, // 道具大礼包	机枪塔*5
	gift_index_23, // 道具大礼包	空袭导弹*5
	gift_index_24, // 至尊大礼包
	gift_index_25, // 战斗大礼包
	gift_index_26, // 新手大礼包
	COST_INDEX_MAX
};

#define PAY_DEBUG 0 //1表示无需sdk即可充值
#define CP_NAME_ID 1  //1代表sk

class PlatFormInterface
{
public:
	static PlatFormInterface* getInstance();
	//思科sdk相关调用
#if(CP_NAME_ID == 1)
	void showPayLayout(int gift_index);//, const char * type = "sms");
	void record(const char* str1, const char* str2, const int &int1, const int &int2);
	void adv_start();
#endif

private:
	static PlatFormInterface* instance;

private:
	PlatFormInterface();
	~PlatFormInterface();
	PlatFormInterface(const PlatFormInterface&);
	PlatFormInterface& operator = (const PlatFormInterface&);

};
#endif