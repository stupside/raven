#pragma once
#include "armory/Raven_Weapon.h"



class  Raven_Bot;

class GrenadeLauncher : public Raven_Weapon
{
private:

	void     InitializeFuzzyModule();

public:

	GrenadeLauncher(Raven_Bot* owner);

	 
	void Render();

	void ShootAt(Vector2D pos);

	double GetDesirability(double DistToTarget);
};
