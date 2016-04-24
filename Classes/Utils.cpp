#include "Utils.h"

vector<string> Utils::str_split(string str, string pattern)
{
	string::size_type pos;
	vector<string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; ++i)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

vector<float> Utils::str_split_f(string str, string pattern)
{
	string::size_type pos;
	vector<float> result;
	str += pattern;
	int size = str.size();
	for (int i = 0; i < size; ++i)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			float f;
			sscanf(s.c_str(), "%f", &f);
			result.push_back(f);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}


void string_replace(string&s1, const string&s2, const string&s3)
{
	string::size_type pos = 0;
	string::size_type a = s2.size();
	string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}


string Utils::str_replace( string str, const string &pattern, const string &subStr)
{
	string::size_type pos = 0;
	string::size_type a = pattern.size();
	string::size_type b = subStr.size();
	while ((pos = str.find(pattern, pos)) != string::npos)
	{
		str.replace(pos, a, subStr);
		pos += b;
	}

	return str;
}

int Utils::getDirOct(const cocos2d::Vec2 &minVec)
{
	float xBy = minVec.x;
	float yBy = minVec.y;
	if (xBy == 0 && yBy == 0)
	{
		return 0;
	}
	if (xBy > 0)
	{
		if (yBy > 0)
		{
			if (xBy > yBy * 2.4)
			{
				return 2;
			}
			else if (yBy > xBy * 2.4)
			{
				return 4;
			}
			else
			{
				return 3;
			}
		}
		else
		{
			if (xBy > -yBy * 2.4)
			{
				return 2;
			}
			else if (-yBy > xBy * 2.4)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	else
	{
		if (yBy > 0)
		{
			if (-xBy > yBy * 2.4)
			{
				return 6;
			}
			else if (yBy > -xBy * 2.4)
			{
				return 4;
			}
			else
			{
				return 5;
			}
		}
		else
		{
			if (xBy < yBy * 2.4)
			{
				return 6;
			}
			else if (yBy < xBy * 2.4)
			{
				return 0;
			}
			else
			{
				return 7;
			}
		}
	}
}

int Utils::getDirHex(const cocos2d::Vec2 &minVec)
{
	int dir = 0;
	float dx = minVec.x;
	float dy = minVec.y;
	if (dx == 0)
	{
		return dy > 0 ? 8 : 0;
	}
	float tg = dy / dx;
	if (tg > -0.1989 && tg <= 0.1989)
	{
		dir = dx > 0 ? 4 : 12;
	}
	else if (tg > 0.1989 && tg <= 0.6682)
	{
		dir = dx > 0 ? 5 : 13;
	}
	else if (tg > 0.6682 && tg <= 1.4966)
	{
		dir = dx > 0 ? 6 : 14;
	}
	else if (tg > 1.4966 && tg <= 5.0273)
	{
		dir = dx > 0 ? 7 : 15;
	}
	else if (tg > 5.0273 || tg <= -5.0273)
	{
		dir = dy > 0 ? 8 : 0;
	}
	else if (tg > -5.0273 && tg <= -1.4966)
	{
		dir = dy > 0 ? 9 : 1;
	}
	else if (tg > -1.4966 && tg <= -0.6682)
	{
		dir = dy > 0 ? 10 : 2;
	}
	else if (tg > -0.6682 && tg <= -0.1989)
	{
		dir = dy > 0 ? 11 : 3;
	}
	return dir;
}

/**
* 获取两点相对于x轴的夹角弧度(360度)
* */
float Utils::vecAngleToX(const cocos2d::Vec2 vec)
{
	auto radian = vec.getAngle();
	if (radian < 0)
	{
		radian += 2 * M_PI;
	}
	return radian;
}

/**
*获取两点之间距离
**/
float Utils::getDistance(const cocos2d::Point sp, const cocos2d::Point ep)
{
	return  sqrt((sp.x - ep.x)*(sp.x - ep.x) + (sp.y - ep.y)*(sp.y - ep.y));
}

double Utils::getRotateAngle(const double &angle)
{
	return angle > 180 ? 360 - angle : -angle;
}

vector<cocos2d::Point> Utils::getDistancePoint(const cocos2d::Point point, int area)
{
	vector<cocos2d::Point> ptVec;
	//int offsetX[] = { -1, -2, -1, -2, -1, -2, -2, -1, 1, 2, 1, 2,  1,  1,  2,  2};
	//int offsetY[] = { -1, -2, -2, -1,  1,  1,  2,  2, 1, 2, 2, 1, -1, -2, -1, -2};
	/*int offsetX[] = { 1,  0,   0,  -1, -2, -2, -1, -2, 0, -1, 1,  0, 2, 2, 2, 1 };
	int offsetY[] = { -2, -1, -2, -2,  -1,  0,  0,  1, 2, 2,  2,  1, 0, -1, 1,0 };
	int index = (area -1)* 4;
	for (int i = 0; i < 4; i++)
	{
		ptVec.push_back(cocos2d::Point(point.x + offsetX[index + i], point.y + offsetY[index + i]));
	}*/
	int offsetX[] = { 0, 0,  -2, -1, 0, 0, 2, 1 };
	int offsetY[] = { -1,-2, 0,  0,  2, 1, 0, 0 };
	int index = (area - 1) * 2;
	for (int i = 0; i < 2; i++)
	{
		ptVec.push_back(cocos2d::Point(point.x + offsetX[index + i], point.y + offsetY[index + i]));
	}
	//随机设置下坐标
	random_shuffle(ptVec.begin(), ptVec.end());
	return ptVec;
}

vector<cocos2d::Point> Utils::getDistancePointBak(const cocos2d::Point point, int area)
{
	vector<cocos2d::Point> ptVec;
	//int offsetX[] = { -1, -2, -1, -2, -1, -2, -2, -1, 1, 2, 1, 2,  1,  1,  2,  2};
	//int offsetY[] = { -1, -2, -2, -1,  1,  1,  2,  2, 1, 2, 2, 1, -1, -2, -1, -2};
	int offsetX[] = { 0, 0, -1, -1, -1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 };
	int offsetY[] = { 0, -1, -1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, -1, -1, 0 };
	int index = (area - 1) * 4;
	int add[] = { 0, 1, 2, 3 };
	for (int i = 0; i < 4; i++)
	{
		ptVec.push_back(cocos2d::Point(point.x + offsetX[index + i], point.y + offsetY[index + i]));
	}
	//随机设置下坐标
	random_shuffle(ptVec.begin(), ptVec.end());
	return ptVec;
}

string Utils::int2String(const int & num)
{
	char c[15];
	sprintf(c, "%d", num);
	return c;
}

int Utils::string2Int(const string str)
{
	if (str == "")
	{
		return 0;
	}

	int i;
	sscanf(str.c_str(), "%d", &i);
	return i;
}

float Utils::string2Float(const string str)
{
	if (str == "")
	{
		return 0;
	}

	float f;
	sscanf(str.c_str(), "%f", &f);
	return f;
}

vector<cocos2d::Point> Utils::getPointVecByStr(const string str)
{
	vector<cocos2d::Point> pointVec;
	vector<string> strVec = str_split(str, "|");

	for (size_t i = 0; i < strVec.size(); i++)
	{
		vector<float> fltVec = str_split_f(strVec[i], ",");
		pointVec.push_back(cocos2d::Point(fltVec[0], fltVec[1]));
	}
	return pointVec;
}

bool Utils::checkDelayFrame(int &frame, const int &totalFrame)
{
	if (frame < totalFrame)
	{
		frame++;
		return false;
	}
	else
	{
		frame = 0;
		return true;
	}
}