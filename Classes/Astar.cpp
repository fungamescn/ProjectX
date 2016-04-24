#include "Astar.h"
#include "Utils.h"

USING_NS_CC;

PathFind::PathFind()
{
}

PathFind::~PathFind()
{
}

static PathFind* pathFind = nullptr;

PathFind* PathFind::getTheOnlyInstance()
{
	if (!pathFind)
	{
		pathFind = new PathFind();
	}

	return pathFind;
}

// A*初始化
Astar* PathFind::newAstar()
{
	Astar* astar = (Astar*)malloc(sizeof(Astar));
	astar->openHead = (AstartListNode*)malloc(sizeof(AstartListNode));
	astar->closedHead = (AstartListNode*)malloc(sizeof(AstartListNode));
	astar->openHead->next = NULL;
	astar->closedHead->next = NULL;
	return astar;
}

// A*释放
void PathFind::deleteAstar(Astar* astar)
{
	while (astar->openHead != NULL)
	{
		AstartListNode* t = astar->openHead;
		astar->openHead = astar->openHead->next;
		free(t);
		t = NULL;
	}

	while (astar->closedHead != NULL)
	{
		AstartListNode* t = astar->closedHead;
		astar->closedHead = astar->closedHead->next;
		free(t);
		t = NULL;
	}

	free(astar);
}

void PathFind::setTiledMap(Sprite* tiledMap)
{
	this->tiledMap = tiledMap;
}

// 结点初始化
void PathFind::AstarNodeInit(AstartListNode* current, AstartListNode* father, int x, int y, int endX, int endY)
{
	current->x = x;
	current->y = y;
	current->father = father;
	current->next = NULL;

	if (father != NULL)
	{
		//current->f = father->f + 1;
		
		if (dirwieght == 1 && ((father->x - x) == -(father->y - y)))
		{
			current->f = father->f + 0.5;
		}
		/*else if (dirwieght == 2 && (abs(father->x - x) != abs(father->y - y)))
		{
			current->f = father->f + 0.5;
		}*/
		else if (dirwieght == 3 && ((father->x - x) == (father->y - y)) )
		{
			current->f = father->f + 0.5;
		}
		else{
			current->f = father->f + 1;
		}
	}
	else
	{
		current->f = 0;
	}
	/*Point ep = nodeForPosition(Vec2(endX, endY));
	Point sp = nodeForPosition(Vec2(x, y));
	current->g = sqrt((ep.x - sp.x)*(ep.x - sp.x) + (ep.y - sp.y) * (ep.y - sp.y));*/
	current->g = abs(endX - x) + abs(endY - y);
	current->h = current->f + current->g;
}

// 取得成本最小的结点
AstartListNode* PathFind::AstarGetMinCostList(Astar* astar)
{
	AstartListNode* min = astar->openHead->next;
	AstartListNode* current = min->next;

	while (current != NULL)
	{
		if (current->h < min->h)
		{
			min = current;
		}

		current = current->next;
	}

	return min;
}

// 添加结点到链表
void PathFind::AstarAddNode(AstartListNode* head, AstartListNode* node)
{
	while (head->next != NULL)
	{
		head = head->next;
	}

	head->next = node;
}

// 从链表中删除结点
void PathFind::AstarRemoveNode(AstartListNode* head, AstartListNode* node)
{
	AstartListNode* current = head;
	head = head->next;

	while (head != NULL)
	{
		if (head == node)
		{
			current->next = head->next;
			head->next = NULL;
			break;
		}
		else
		{
			current = head;
			head = head->next;
		}
	}
}

// 检查Tile是否在链表中
bool PathFind::AstarCheckNodeInList(int x, int y, AstartListNode* head)
{
	bool result = false;
	head = head->next;

	while (head != NULL)
	{
		if (head->x == x && head->y == y)
		{
			result = true;
			break;
		}
		else
		{
			head = head->next;
		}
	}

	return result;
}

// 检查Tile是否是地图的路障
bool PathFind::AstarIsBlock(int x, int y)
{
//	if (x >= 0 && x < tiledMap->getMapSize().width * 2 && y >= 0 && y < tiledMap->getMapSize().height * 2)
//	{
//		if (this->hasDynamicBlock(x, y))
//		{
//			return true;
//		}
//
//		TMXLayer* mapLayer = tiledMap->getLayer("blacklayer");
//		if (!mapLayer) return false;
//		int tileGid = mapLayer->getTileGIDAt(ccp(ceil(x / 2), ceil(y / 2)));
//		if (tileGid) {
//			return true;
//		}
//	}
//	else
//	{
//		return false;
//	}

	return false;
}

void PathFind::addDynamicBlockArea(std::vector<int> posVec)
{
	string key = StringUtils::format("%03d%03d%03d%03d", posVec[0], posVec[1], posVec[2], posVec[3]);
	_blockAreaMap[key] = posVec;
}

void PathFind::removeDynamicBlockArea(std::vector<int> posVec)
{
	string key = StringUtils::format("%03d%03d%03d%03d", posVec[0], posVec[1], posVec[2], posVec[3]);
	auto it = _blockAreaMap.find(key);
	if (it != _blockAreaMap.end())
	{
		_blockAreaMap.erase(it);
		return;
	}
}

bool PathFind::hasDynamicBlock(int x, int y)
{
	for (std::map< std::string, std::vector<int> >::iterator it = _blockAreaMap.begin(); it != _blockAreaMap.end();++it)
	{
		if (x >= it->second.at(0) && x < it->second.at(2) && y >= it->second.at(1) && y < it->second.at(3))
		{
			return true;
		}
	}
	return false;
}

list<Point> PathFind::AStarSearch(int nodeStartX, int nodeStartY, int nodeEndX, int nodeEndY, bool limitNode)
{
	astarPathCount = 0;
	// 路经表
	list<Point> astarPathList;

	astarPathList.clear();

	this->startX = nodeStartX;
	this->startY = nodeStartY;
	this->endX = nodeEndX;
	this->endY = nodeEndY;

	//if (AstarIsBlock(endX, endY))
	//{
	//	return astarPathList;
	//}

	int offsetX[8] = { 0, 0, -1, 1, -1, -1, 1, 1 };
	int offsetY[8] = { 1, -1, 0, 0, 1, -1, 1, -1 };

	int offsetX1[4] = { 0, -1, 0, 1 };
	int offsetY1[4] = { 1, 0, -1, 0 };


	Astar* astar = newAstar();

	AstartListNode* currentNode = NULL;
	AstartListNode* startNode = (AstartListNode*)malloc(sizeof(AstartListNode));
	AstarNodeInit(startNode, NULL, startX, startY, endX, endY);
	// 把起始结点加入OpenList
	AstarAddNode(astar->openHead, startNode);

	// 如查OpenList不为空
	while (astar->openHead->next != NULL)
	{
		// 取得成本最小的结点
		currentNode = AstarGetMinCostList(astar);

		// 如果当前结点是目标结点
		if (currentNode->x == endX && currentNode->y == endY)
		{
			break;
		}
		else if (astarPathCount > (limitNode ? MAX_ASTAR_COUNT : DEFINE_MAX_ASTAR_COUNT))
		{
			break;
		}
		else
		{
			// 把当前结点添加到Closed表中
			AstarAddNode(astar->closedHead, currentNode);
			// 把当前结点从Open表中删除
			AstarRemoveNode(astar->openHead, currentNode);
			int len = !limitNode ? 4 : 8;
			for (int i = 0; i < len; i++)
			{
				int x = currentNode->x;// +offsetX[i];
				if (!limitNode)
				{
					x = x + offsetX1[i];
				}
				else{
					x = x + offsetX[i];
				}
				int y = currentNode->y;// +offsetY[i];
				if (!limitNode)
				{
					y = y + offsetY1[i];
				}
				else{
					y = y + offsetY[i];
				}

				if (x < 0 || x >= 2668 * 2 || y < 0 || y >= 2230 * 2)
				{
					continue;
				}
				else
				{
					if (!AstarCheckNodeInList(x, y, astar->openHead)
						&& !AstarCheckNodeInList(x, y, astar->closedHead)
						&& !AstarIsBlock(x, y)
						)
					{
						AstartListNode* endNode = (AstartListNode*)malloc(sizeof(AstartListNode));
						AstarNodeInit(endNode, currentNode, x, y, endX, endY);
						AstarAddNode(astar->openHead, endNode);
					}
				}
			}
		}
		astarPathCount++;
	}
//	CCLOG("astarPathCount %d", astarPathCount);
	if (astar->openHead->next == NULL && (currentNode->x != endX || currentNode->y != endY))
	{
		astarPathCount = 0;
		astarPathList.clear();
		return astarPathList;
	}
	else
	{
		while (currentNode != NULL)
		{
			Point tilePoint;
			tilePoint.x = currentNode->x;
			tilePoint.y = currentNode->y;
			/*
			TMXLayer* mapLayer = tiledMap->getLayer("Background");
			if (!mapLayer) return false;
			Sprite* tile = mapLayer->tileAt(ccp(ceil(tilePoint.x / 2), ceil(tilePoint.y / 2)));
			*/
			astarPathList.push_front(tilePoint);
			currentNode = currentNode->father;
		}

		return astarPathList;
	}

	deleteAstar(astar);
	astarPathList.clear();
	return astarPathList;
}

void PathFind::updateDirWieght(int wieght)
{
	dirwieght = wieght;
}

void PathFind::cleanDynamicBlock()
{
	_blockAreaMap.clear();
}