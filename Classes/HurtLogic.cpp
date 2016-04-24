#include "HurtLogic.h"
#include "Config.h"
#include "Hero.h"

void HurtLogic::hurtHp(Base* attacker, Base* injurer, bool useShield)
{
	if (injurer->getGroupType() == GROUP_TYPE_OTHER)
	{
		return;
	}
	int restrict = 0;
	if (attacker->getGroupType() == GROUP_TYPE_HERO)
	{
		restrict = static_cast<Hero*>(attacker)->getRestrict(injurer->getMaterialType());
	}
	int minusHp = HURTHP(attacker->getAttack(), injurer->getDefense(), restrict);
	if (useShield && injurer->getShield() > 0)
	{
		if (minusHp >= injurer->getShield())
		{
			minusHp -= injurer->getShield();
			injurer->setShield(0);
		}
		else
		{
			injurer->setShield(injurer->getShield() - minusHp);
			minusHp = 0;
		}
	}
	injurer->setCurrentHp(injurer->getCurrentHp() - minusHp);
}

void HurtLogic::skillHurtHp(META_LINE& skillMeta, Base* injurer)
{
	int injurerGroup = injurer->getGroupType();
	if (injurerGroup == GROUP_TYPE_OTHER)
	{
		return;
	}
	int damage = atoi(skillMeta["damage"].c_str());
	int useShield = atoi(skillMeta["useShield"].c_str());
	int minusHp = SKILL_HURTHP(damage, injurer->getDefense());
	if (useShield && injurer->getShield() > 0)
	{
		if (minusHp >= injurer->getShield())
		{
			minusHp -= injurer->getShield();
			injurer->setShield(0);
		}
		else
		{
			injurer->setShield(injurer->getShield() - minusHp);
			minusHp = 0;
		}
	}
	injurer->setCurrentHp(injurer->getCurrentHp() - minusHp);
}