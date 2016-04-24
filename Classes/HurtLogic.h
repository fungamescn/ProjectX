#ifndef __HURTLOGIC_H__
#define __HURTLOGIC_H__

#include "Base.h"
#include "Template.h"

#define HURTHP(att, def, add) MAX(1, att - def) * (1 + add / 100)
#define SKILL_HURTHP(att, def) MAX(1, att - def)

class HurtLogic
{
public:
	HurtLogic() = default;
	~HurtLogic() = default;
	static void hurtHp(Base* attacker, Base* injurer, bool useShield);
	static void skillHurtHp(META_LINE& skillMeta, Base* injurer);
};

#endif