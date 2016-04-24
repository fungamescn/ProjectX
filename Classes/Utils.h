#ifndef __UTILS_H__
#define __UTILS_H__

#include "cocos2d.h"

using namespace std;

class Utils
{
public:
	Utils() = default;
	~Utils() = default;
	static vector<string> str_split(string str, string pattern);
	static vector<float> str_split_f(string str, string pattern);
	static string str_replace(string str, const string &pattern, const string &subStr);
	static int getDirOct(const cocos2d::Vec2 &minVec);
	static int getDirHex(const cocos2d::Vec2 &minVec);
	static float vecAngleToX(const cocos2d::Vec2 vec);
	static float getDistance(const cocos2d::Point sp, const cocos2d::Point ep);
	static double getRotateAngle(const double &angle);
	static vector<cocos2d::Point> getDistancePoint(const cocos2d::Point point, int area);
	static vector<cocos2d::Point> getDistancePointBak(const cocos2d::Point point, int area);
	static string int2String(const int & num);
	static int string2Int(const string str);
	static float string2Float(const string str);
	static vector<cocos2d::Point> getPointVecByStr(const string str);
	static bool checkDelayFrame(int &frame, const int &totalFrame);
};
#endif