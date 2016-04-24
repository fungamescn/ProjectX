#include "OBB2d.h"

USING_NS_CC;

void Projection::initialize(const float &min, const float &max)
{
	_min = min;
	_max = max;
}

const float& Projection::getMax()
{
	return _max;
}

const float& Projection::getMin()
{
	return _min;
}

bool Projection::overlap(Projection* project)
{
	if (_min > project->getMax() || _max < project->getMin())
	{
		return false;
	}

	return true;
}

OBB2d::OBB2d() : _project(nullptr)
{
	
}

OBB2d::~OBB2d()
{
	delete _project;
	_project = nullptr;
}

cocos2d::Vec2* OBB2d::getAxles()
{
	for (int i = 0; i < 2; i++)
	{
		auto minVec = _vertex[i] - _vertex[i + 1];
		minVec.normalize();
		_axles[i].x = -minVec.y;
		_axles[i].y = minVec.x;
	}
	return _axles;
}

Projection* OBB2d::getProjection(const cocos2d::Vec2 &axle)
{
	float min = _vertex[0].dot(axle);
	float max = min;

	for (int i = 1; i < 4; i++)
	{
		float eachOne = _vertex[i].dot(axle);
		if (eachOne > max)
		{
			max = eachOne;
		}
		else if (eachOne < min)
		{
			min = eachOne;
		}
	}
	if (!_project)
	{
		_project = new Projection();
	}
	_project->initialize(min, max);
	return _project;
}

void OBB2d::setVertex(const int &index, const cocos2d::Vec2 &vec)
{
	assert(0 <= index && index <= 3 && "The index must be from 0 to 3");
	_vertex[index].x = vec.x;
	_vertex[index].y = vec.y;
}

bool OBB2d::isCollidWithOBB(OBB2d* obb2d)
{
	auto axleSelf = this->getAxles();
	auto axleObb = obb2d->getAxles();

	for (int i = 0; i < 2; i++)
	{
		auto projectSS = this->getProjection(axleSelf[i]);
		auto projectOS = obb2d->getProjection(axleSelf[i]);
		if (!projectSS->overlap(projectOS))
		{
			return false;
		}
		auto projectSO = this->getProjection(axleObb[i]);
		auto projectOO = obb2d->getProjection(axleObb[i]);
		if (!projectSO->overlap(projectOO))
		{
			return false;
		}
	}

	return true;
}