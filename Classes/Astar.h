#ifndef __PATH_FIND_H__
#define __PATH_FIND_H__

#include "cocos2d.h"

#define  MAX_ASTAR_COUNT 30
#define  DEFINE_MAX_ASTAR_COUNT 300

// 链表结点结构
typedef struct tagAstartListNode
{
	// 结点 x 坐标
	short x;

	//结点 y 坐标
	short y;

	// 当前结点到起始结点的代价
	short f;

	// 当前结点到目标结点的代价
	short g;

	// 总代价
	short h;

	// 当前结点父结点
	struct tagAstartListNode* father;

	// 当前结点子结点
	struct tagAstartListNode* next;
} AstartListNode;

// A*算法结构
typedef struct tagAstar
{
	// open 表
	AstartListNode* openHead;

	// closed 表
	AstartListNode* closedHead;
} Astar;

class PathFind
{
public:

	PathFind();
	~PathFind();

	// 路径长度
	int astarPathCount;

	cocos2d::Sprite* tiledMap;

	int startX;
	int startY;
	int endX;
	int endY;

	static PathFind* getTheOnlyInstance();

	void setTiledMap(cocos2d::Sprite* tiledMap);

	// A*初始化
	Astar* newAstar();

	// A*释放
	void deleteAstar(Astar* Astar);

	// 结点初始化
	void AstarNodeInit(AstartListNode* current, AstartListNode* father, int x, int y, int endX, int endY);

	// 取得成本最小的结点
	AstartListNode* AstarGetMinCostList(Astar* Astar);

	// 添加结点到链表
	void AstarAddNode(AstartListNode* head, AstartListNode* node);

	// 从链表中删除结点
	void AstarRemoveNode(AstartListNode* head, AstartListNode* node);

	// 检查Tile是否在链表中
	bool AstarCheckNodeInList(int x, int y, AstartListNode* head);

	// 检查Tile是否是地图的路障
	bool AstarIsBlock(int x, int y);

	//添加动态路障区域
	void addDynamicBlockArea(std::vector<int> posVec);

	//删除动态路障区域
	void removeDynamicBlockArea(std::vector<int> posVec);

	//是否有动态路障
	bool hasDynamicBlock(int x, int y);

	//list<Point> AStarSearch(TMXTiledMap* tiledMap, int startX, int startY, int endX, int endY);
	std::list<cocos2d::Point> AStarSearch(int startX, int startY, int endX, int endY, bool limitNode = false);

	void updateDirWieght(int wieght);

	void cleanDynamicBlock();

private:
	std::map<std::string,int> _blockMap;
	std::map< std::string, std::vector<int> > _blockAreaMap;
	std::map<std::string,int> _collisonMap;

	int dirwieght;
};

#endif