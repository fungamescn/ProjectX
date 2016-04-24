#ifndef __RANDOM_LIB_H__
#define __RANDOM_LIB_H__

class Base;
class RandomLib
{
public:
	RandomLib(){};
	~RandomLib(){};
	static int getAwardId(int planId);
	static void award(int awardId, Base* base = nullptr);
	static void awardByPlanId(int planId, Base* base = nullptr);
private:
	
};

#endif