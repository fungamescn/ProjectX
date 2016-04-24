#ifndef __OBB2D_H__
#define __OBB2D_H__

#include "cocos2d.h"

class Projection
{
public:
	Projection() = default;
	~Projection() = default;
	void initialize(const float &min, const float &max);
	bool overlap(Projection* project);
	const float& getMin();
	const float& getMax();
private:
	float _min;
	float _max;
};

class OBB2d
{
public:
	OBB2d();
	~OBB2d();

	cocos2d::Vec2* getAxles();
	Projection* getProjection(const cocos2d::Vec2 &axle);
	void setVertex(const int &index, const cocos2d::Vec2 &vec);
	bool isCollidWithOBB(OBB2d* obb2d);
private:
	cocos2d::Vec2 _vertex[4];
	cocos2d::Vec2 _axles[2];
	Projection* _project;
};

#endif