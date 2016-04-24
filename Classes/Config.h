#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__

//配置文件Config
#define ROLEZORDER  1000
#define AIMZORDER  10000
#define BACKGROUND_T -20000
#define BACKGROUND_B 20000
#define MAX_ELEMENT_ZORDER 5000
#define BULLET_ZORDER 25000
#define SKILL_ZORDER 30000
#define MIN_NUM 0
#define MAX_NUM 9999999
#define ROLE_STATE_CHECK_RATE 10

#define MUSIC_BACKGROUD_FILE "music/sound_battleMusic.mp3"
#define MUSIC_FIRE_FILE "music/firebatI.wav"

#define MUSIC_FIRE "music/"
#define MUSIC_EXPLOSION "music/sound_explosion.ogg"

#define MAX_DEAD_MONSTER_PTR_NUM 15
#define MAX_DEAD_BODY_NUM 25
#define MIN_DEAD_BODY_NUM 15

#define BATCH_NUM 5
#define SACLE_XY 2

#define GUIDE_ARROW_DISTANCE 20
#define GUIDE_ARROW_TIME 0.3

#define MONEY_ITEM_ID 5

#define MAX_STAGE 3010

#define BATTLE_ITEM_MOVE_SPEED 10
#define BATTLE_ITEM_FLASH_RATE 8

#define MAX_LEVEL 20
#define PROPERTY_MAX_LVL 10
enum
{
	UI_GAME_START = 0,
	UI_LEVEL_SELECTION,
	UI_LEVEL_INFO,
	UI_TEAM_HIRE,
	UI_LOADING,
	UI_STAGE_LAYER,
};

enum
{
	BATTLE_POP_WIN,
	BATTLE_POP_LOSE,
	BATTLE_POP_STOP
};

#define DEBUG_MODE 0
#define DEBUG_LOG_LINE 20
#define DEBUG_DISPLAY_STATES 0

enum
{
	GROUP_TYPE_HERO = 1,
	GROUP_TYPE_JUST = 2,
	GROUP_TYPE_EVIL = 4,
	GROUP_TYPE_NEUTRAL = 8,
	GROUP_TYPE_OTHER = 16
};
static const int GROUP_TYPE[] = { GROUP_TYPE_HERO, GROUP_TYPE_JUST, GROUP_TYPE_EVIL, GROUP_TYPE_NEUTRAL, GROUP_TYPE_OTHER };

#define GD_PRE 10000

#define INVALID_FLAG -1
#define GD_MONEY_FUNC_ERROR -1

//key 前缀 type
enum GD_PREFIX
{
	GD = 0,
	GD_ITEM = 1,
	GD_SKILL = 2,
	GD_LEVEL = 3,
	GD_TEAM_LOCK = 4,
	GD_TEAM_LEVEL = 5,
	GD_STAGE_STAR = 6,
	GD_PROPERTY = 7,
	GD_GUIDE = 8
};

const int GD_TEAM_1 = 1;
const int GD_TEAM_2 = 2;
const int GD_TEAM_3 = 3;
const int GD_TEAM_4 = 4;
const int GD_MAX_STAGE = 5;
const int GD_SUCTION_GOLD = 6;
const int GD_GUIDE_UI = 7;
//1000表示游戏币编号
const int GD_MONEY = 1000;


const unsigned int kZoomActionTag = 0xc0c05002;
const int MAX_HERO_VEC_LENGTH = 4;
const int HERO_TYPE_NUM = 4;
const float BTN_SCALE_UP_TIME = 0.05f;
const float BTN_SCALE_DOWN_TIME = 0.1f;

enum
{
	SKILL_ID_HEAL = 1,
	SKILL_ID_NUCLEAR = 4,
	SKILL_ID_GATLIN = 5,
	SKILL_ID_ROCKET = 3
};

enum
{
	SKILL_ITEM_HEAL = 1,
	SKILL_ITEM_NUCLEAR,
	SKILL_ITEM_GATLIN,
	SKILL_ITEM_ROCKET
};

#endif

