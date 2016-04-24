#include "PlatFormInterface.h"
#include "GameData.h"
#include "BattleScene.h"
#include "Utils.h"
#include "BattleLayer.h"
//头文件包含，判断平台
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif
USING_NS_CC;


const ST_PAY st_PayList[] =
{
	{ "1", "200" },	
	{ "2", "1000" },
	{ "3", "200" },
	{ "4", "400" },
	{ "5", "600" },
	{ "6", "600" },
	{ "7", "800" },
	{ "8", "1000" },
	{ "9", "200" },
	{ "10", "200" },
	{ "11", "200" },
	{ "12", "200" },
	{ "13", "400" },
	{ "14", "400" },
	{ "15", "400" },
	{ "16", "400" },
	{ "17", "200" },
	{ "18", "400" },
	{ "19", "200" },
	{ "20", "200" },
	{ "21", "400" },
	{ "22", "800" },
	{ "23", "400" },
	{ "24", "400" },
	{ "25", "1000" },
	{ "26", "200" },
	{ "27", "400" }
};

void payCallBack(int payPointNum)
{
	int refreshRoleId = 0;
	int refreshBuff = 0;
	switch (payPointNum)
	{
		case gift_index_0:
		{
			GameData::getInstance()->addMoney(50000);
			break;
		}
		case gift_index_1:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 15);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 15);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 15);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 15);
			break;
		}
		case gift_index_2:
		{
			GameData::getInstance()->setGameDataForKey(1002, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_3:
		{
			GameData::getInstance()->setGameDataForKey(1003, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_4:
		{
			GameData::getInstance()->setGameDataForKey(1004, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_5:
		{
			GameData::getInstance()->addMoney(150000);
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			GameData::getInstance()->setGameDataForKey(1002, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_6:
		{
			GameData::getInstance()->addMoney(150000);
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			GameData::getInstance()->setGameDataForKey(1003, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_7:
		{
			GameData::getInstance()->addMoney(150000);
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			GameData::getInstance()->setGameDataForKey(1004, "1", GD_TEAM_LOCK);
			break;
		}
		case gift_index_8:
		{
			GameData::getInstance()->setGameDataForKey(1001, Utils::int2String(MAX_LEVEL), GD_TEAM_LEVEL);
			PlatFormInterface::getInstance()->record("雇佣兵", "等级", 1001, MAX_LEVEL);
			refreshRoleId = 1001;
			break;
		}
		case gift_index_9:
		{
			GameData::getInstance()->setGameDataForKey(1002, Utils::int2String(MAX_LEVEL), GD_TEAM_LEVEL);
			PlatFormInterface::getInstance()->record("雇佣兵", "等级", 1002, MAX_LEVEL);
			refreshRoleId = 1002;
			break;
		}
		case gift_index_10:
		{
			GameData::getInstance()->setGameDataForKey(1003, Utils::int2String(MAX_LEVEL), GD_TEAM_LEVEL);
			PlatFormInterface::getInstance()->record("雇佣兵", "等级", 1003, MAX_LEVEL);
			refreshRoleId = 1003;
			break;
		}
		case gift_index_11:
		{
			GameData::getInstance()->setGameDataForKey(1004, Utils::int2String(MAX_LEVEL), GD_TEAM_LEVEL);
			PlatFormInterface::getInstance()->record("雇佣兵", "等级", 1004, MAX_LEVEL);
			refreshRoleId = 1004;
			break;
		}
		case gift_index_12:
		{
			GameData::getInstance()->setGameDataForKey(1, Utils::int2String(PROPERTY_MAX_LVL), GD_PROPERTY);
			PlatFormInterface::getInstance()->record("技能", "等级", 1, PROPERTY_MAX_LVL);
			refreshBuff = 1;
			break;
		}
		case gift_index_13:
		{
			GameData::getInstance()->setGameDataForKey(2, Utils::int2String(PROPERTY_MAX_LVL), GD_PROPERTY);
			PlatFormInterface::getInstance()->record("技能", "等级", 2, PROPERTY_MAX_LVL);
			refreshBuff = 1;
			break;
		}
		case gift_index_14:
		{
			GameData::getInstance()->setGameDataForKey(3, Utils::int2String(PROPERTY_MAX_LVL), GD_PROPERTY);
			PlatFormInterface::getInstance()->record("技能", "等级", 3, PROPERTY_MAX_LVL);
			refreshBuff = 1;
			break;
		}
		case gift_index_15:
		{
			GameData::getInstance()->setGameDataForKey(4, Utils::int2String(PROPERTY_MAX_LVL), GD_PROPERTY);
			PlatFormInterface::getInstance()->record("技能", "等级", 4, PROPERTY_MAX_LVL);
			refreshBuff = 1;
			break;
		}
		case gift_index_16:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			break;
		}
		case gift_index_17:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			break;
		}
		case gift_index_18:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			break;
		}
		case gift_index_19:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			break;
		}
		case gift_index_20:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 15);
			break;
		}
		case gift_index_21:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 15);
			break;
		}
		case gift_index_22:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 15);
			break;
		}
		case gift_index_23:
		{
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 15);
			break;
		}
		case gift_index_24:
		{
			GameData::getInstance()->addMoney(1000000);
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			GameData::getInstance()->setGameDataForKey(GD_SUCTION_GOLD, "1");
			break;
		}
		case gift_index_25:
		{
			BattleScene::getInstance()->allResurrection();
			break;
		}
		case gift_index_26:
		{
			GameData::getInstance()->addMoney(50000);
			GameData::getInstance()->addItemById(SKILL_ITEM_HEAL, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_NUCLEAR, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_GATLIN, 5);
			GameData::getInstance()->addItemById(SKILL_ITEM_ROCKET, 5);
			break;
		}
	}
	BattleScene::getInstance()->updateAll();
	if (refreshBuff != 0)
	{
		BattleLayer::getInstance()->refreshHeroProp(0);
	}
	else if (refreshRoleId != 0)
	{
		BattleLayer::getInstance()->refreshHeroProp(refreshRoleId);
	}
}


PlatFormInterface::PlatFormInterface(){};
PlatFormInterface::~PlatFormInterface(){
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}

};

PlatFormInterface* PlatFormInterface::instance = nullptr;

PlatFormInterface* PlatFormInterface::getInstance()
{
	if (instance == nullptr)
	{
		instance = new PlatFormInterface();
	}
	return instance;
}

#if(CP_NAME_ID == 1)
void PlatFormInterface::showPayLayout(int gift_index)//, const char * type)
{
	if ( gift_index < 0 || gift_index >= COST_INDEX_MAX ) return;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && !PAY_DEBUG)
	JniMethodInfo minfo;//定义Jni函数信息结构体
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数
	const char * jniClass = "org/cocos2dx/cpp/AppActivity";//这里写你所要调用的java代码的类名
	bool isHave = JniHelper::getStaticMethodInfo(minfo, jniClass, "showPayLayout", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave) {
		CCLog("jni->%s/callJni:此函数不存在", jniClass);
	}
	else{
		std::string payPoint = st_PayList[gift_index].payPoint;
		std::string payPrice = st_PayList[gift_index].payPrice;
		CCLog("jni->%s/callJni:此函数存在", jniClass);
		jstring jpayPoint = minfo.env->NewStringUTF(payPoint.c_str());
		jstring jpayPrice = minfo.env->NewStringUTF(payPrice.c_str());
		jstring jtype = minfo.env->NewStringUTF("sms");
		//调用此函数
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jpayPoint, jpayPrice, jtype);
	}
	CCLog("jni-java函数执行完毕");
#endif
#if(PAY_DEBUG)
	payCallBack(gift_index);
#endif
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
extern "C"
{
	//方法名与java类中的包名+方法名，以下划线连接
	void Java_org_cocos2dx_cpp_AppActivity_SendInfo(JNIEnv *env, jobject thiz, jint code)
	{
		int payPointNum  = code;
		payCallBack(payPointNum -1);
	}

	void Java_org_cocos2dx_cpp_AppActivity_SendInfoS(JNIEnv *env, jobject thiz, jstring code)
	{
	}
}
#endif

void PlatFormInterface::record(const char* str1, const char* str2, const int &int1, const int &int2)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && !PAY_DEBUG)
	JniMethodInfo minfo;//定义Jni函数信息结构体
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数
	const char * jniClass = "org/cocos2dx/cpp/AppActivity";//这里写你所要调用的java代码的类名
	bool isHave = JniHelper::getStaticMethodInfo(minfo, jniClass, "record", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/int;Ljava/lang/int;)V");
	if (!isHave) {
		CCLog("jni->%s/callJni:此函数不存在", jniClass);
	}
	else{
		jstring jstr1 = minfo.env->NewStringUTF(str1);
		jstring jstr2 = minfo.env->NewStringUTF(str2);
		jint jint1 = int1;
		jint jint2 = int2;
		//调用此函数
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jstr1, jstr2, jint1, jint2);
	}
	CCLog("jni-java函数执行完毕");
#endif
}

void PlatFormInterface::adv_start()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && !PAY_DEBUG)
	JniMethodInfo minfo;//定义Jni函数信息结构体
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数
	const char * jniClass = "org/cocos2dx/cpp/AppActivity";//这里写你所要调用的java代码的类名
	bool isHave = JniHelper::getStaticMethodInfo(minfo, jniClass, "adv_start", "()V");
	if (!isHave) {
		CCLog("jni->%s/callJni:此函数不存在", jniClass);
	}
	else{
		//调用此函数
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	}
	CCLog("jni-java函数执行完毕");
#endif
}

#endif