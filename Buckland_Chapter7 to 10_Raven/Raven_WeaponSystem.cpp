#include "Raven_WeaponSystem.h"
#include "armory/Weapon_RocketLauncher.h"
#include "Weapon_GrenadeLauncher.h"
#include "armory/Weapon_RailGun.h"
#include "armory/Weapon_ShotGun.h"
#include "armory/Weapon_Blaster.h"
#include "Raven_Bot.h"
#include "misc/utils.h"
#include "lua/Raven_Scriptor.h"
#include "Raven_Game.h"
#include "Raven_UserOptions.h"
#include "2D/transformations.h"


//------------------------- ctor ----------------------------------------------
//-----------------------------------------------------------------------------
Raven_WeaponSystem::Raven_WeaponSystem(Raven_Bot* owner,
	double ReactionTime,
	double AimAccuracy,
	double AimPersistance) :m_pOwner(owner),
	m_dReactionTime(ReactionTime),
	m_dAimAccuracy(AimAccuracy),
	m_dAimPersistance(AimPersistance)
{
	Initialize();
}

//------------------------- dtor ----------------------------------------------
//-----------------------------------------------------------------------------
Raven_WeaponSystem::~Raven_WeaponSystem()
{
	for (unsigned int w = 0; w < m_WeaponMap.size(); ++w)
	{
		delete m_WeaponMap[w];
	}
}

//------------------------------ Initialize -----------------------------------
//
//  initializes the weapons
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::Initialize()
{
	//delete any existing weapons
	WeaponMap::iterator curW;
	for (curW = m_WeaponMap.begin(); curW != m_WeaponMap.end(); ++curW)
	{
		delete curW->second;
	}

	m_WeaponMap.clear();

	//set up the container
	m_pCurrentWeapon = new Blaster(m_pOwner);

	m_WeaponMap[type_blaster] = m_pCurrentWeapon;
	m_WeaponMap[type_shotgun] = 0;
	m_WeaponMap[type_rail_gun] = 0;
	m_WeaponMap[type_rocket_launcher] = 0;
	m_WeaponMap[type_grenade_launcher] = 0;

	InitializeFuzzyPrecision();
}

void Raven_WeaponSystem::InitializeFuzzyPrecision()
{
	FuzzyVariable& DistanceToTarget = m_FuzzyModule.CreateFLV("Distance_To_Target");

	FzSet& DistanceToTarget_Close = DistanceToTarget.AddLeftShoulderSet("Target_Close", 0, 25, 150);
	FzSet& DistanceToTarget_Medium = DistanceToTarget.AddTriangularSet("Target_Medium", 25, 150, 300);
	FzSet& DistanceToTarget_Far = DistanceToTarget.AddRightShoulderSet("Target_Far", 150, 300, 1000);

	FuzzyVariable& Velocity = m_FuzzyModule.CreateFLV("Velocity");

	auto MaxSpeed = m_pOwner->MaxSpeed();

	FzSet& Velocity_Slow = Velocity.AddLeftShoulderSet("Velocity_Slow", 0, MaxSpeed * 1 / 3, MaxSpeed * 2 / 3);
	FzSet& Velocity_Normal = Velocity.AddTriangularSet("Velocity_Normal", MaxSpeed * 1 / 3, MaxSpeed * 2 / 3, MaxSpeed);
	FzSet& Velocity_Fast = Velocity.AddRightShoulderSet("Velocity_Fast", MaxSpeed * 2 / 3, MaxSpeed, MaxSpeed * 2);

	FuzzyVariable& VisibleTimeSpanTarget = m_FuzzyModule.CreateFLV("Visible_Time_Span_Target");

	auto ReactionTime = m_dReactionTime;

	FzSet& VisibleTimeSpanTarget_Short = VisibleTimeSpanTarget.AddLeftShoulderSet("TimeTargetHasBeenVisible_Short", 0, ReactionTime * 2, ReactionTime * 4);
	FzSet& VisibleTimeSpanTarget_Normal = VisibleTimeSpanTarget.AddTriangularSet("TimeTargetHasBeenVisible_Normal", ReactionTime * 2, ReactionTime * 4, ReactionTime * 10);
	FzSet& VisibleTimeSpanTarget_Long = VisibleTimeSpanTarget.AddLeftShoulderSet("TimeTargetHasBeenVisible_Long", ReactionTime * 4, ReactionTime * 10, ReactionTime * 100);

	FuzzyVariable& Accuracy = m_FuzzyModule.CreateFLV("Accuracy");

	FzSet& AccuracyUnaccurate = Accuracy.AddLeftShoulderSet("Accuracy_Unaccurate", 0 / 100, 10 / 100, 25 / 100);
	FzSet& AccuracyAccurate = Accuracy.AddTriangularSet("Accuracy_Accurate", 10 / 100, 25 / 100, 80 / 100);
	FzSet& AccuracyPerfectAccurate = Accuracy.AddRightShoulderSet("Accuracy_Perfect_Accurate", 25 / 100, 80 / 100, 100 / 100);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Slow, DistanceToTarget_Close)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Normal, DistanceToTarget_Close)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Fast, DistanceToTarget_Close)), AccuracyUnaccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Slow, DistanceToTarget_Medium)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Normal, DistanceToTarget_Medium)), AccuracyUnaccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Fast, DistanceToTarget_Medium)), AccuracyUnaccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Slow, DistanceToTarget_Far)), AccuracyUnaccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Normal, DistanceToTarget_Far)), AccuracyUnaccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Short, FzAND(Velocity_Fast, DistanceToTarget_Far)), AccuracyUnaccurate);


	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Slow, DistanceToTarget_Close)), AccuracyPerfectAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Normal, DistanceToTarget_Close)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Fast, DistanceToTarget_Close)), AccuracyAccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Slow, DistanceToTarget_Medium)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Normal, DistanceToTarget_Medium)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Fast, DistanceToTarget_Medium)), AccuracyUnaccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Slow, DistanceToTarget_Far)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Normal, DistanceToTarget_Far)), AccuracyUnaccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Normal, FzAND(Velocity_Fast, DistanceToTarget_Far)), AccuracyUnaccurate);


	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Slow, DistanceToTarget_Close)), AccuracyPerfectAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Normal, DistanceToTarget_Close)), AccuracyPerfectAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Fast, DistanceToTarget_Close)), AccuracyAccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Slow, DistanceToTarget_Medium)), AccuracyPerfectAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Normal, DistanceToTarget_Medium)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Fast, DistanceToTarget_Medium)), AccuracyAccurate);

	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Slow, DistanceToTarget_Far)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Normal, DistanceToTarget_Far)), AccuracyAccurate);
	m_FuzzyModule.AddRule(FzAND(VisibleTimeSpanTarget_Long, FzAND(Velocity_Fast, DistanceToTarget_Far)), AccuracyUnaccurate);
}

//-------------------------------- SelectWeapon -------------------------------
//
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::SelectWeapon()
{
	//if a target is present use fuzzy logic to determine the most desirable 
	//weapon.
	if (m_pOwner->GetTargetSys()->isTargetPresent())
	{
		//calculate the distance to the target
		double DistToTarget = Vec2DDistance(m_pOwner->Pos(), m_pOwner->GetTargetSys()->GetTarget()->Pos());

		//for each weapon in the inventory calculate its desirability given the 
		//current situation. The most desirable weapon is selected
		double BestSoFar = MinDouble;

		WeaponMap::const_iterator curWeap;
		for (curWeap = m_WeaponMap.begin(); curWeap != m_WeaponMap.end(); ++curWeap)
		{
			//grab the desirability of this weapon (desirability is based upon
			//distance to target and ammo remaining)
			if (curWeap->second)
			{
				double score = curWeap->second->GetDesirability(DistToTarget);

				//if it is the most desirable so far select it
				if (score > BestSoFar)
				{
					BestSoFar = score;

					//place the weapon in the bot's hand.
					m_pCurrentWeapon = curWeap->second;
				}
			}
		}
	}

	else
	{
		m_pCurrentWeapon = m_WeaponMap[type_blaster];
	}
}

//--------------------  AddWeapon ------------------------------------------
//
//  this is called by a weapon affector and will add a weapon of the specified
//  type to the bot's inventory.
//
//  if the bot already has a weapon of this type then only the ammo is added
//-----------------------------------------------------------------------------
void  Raven_WeaponSystem::AddWeapon(unsigned int weapon_type)
{
	//create an instance of this weapon
	Raven_Weapon* w = 0;

	switch (weapon_type)
	{
	case type_rail_gun:

		w = new RailGun(m_pOwner); break;

	case type_shotgun:

		w = new ShotGun(m_pOwner); break;

	case type_rocket_launcher:

		w = new RocketLauncher(m_pOwner); break;

	case type_grenade_launcher:

		w = new GrenadeLauncher(m_pOwner); break;

	case type_blaster:

		return;
	}//end switch


	//if the bot already holds a weapon of this type, just add its ammo
	Raven_Weapon* present = GetWeaponFromInventory(weapon_type);

	if (present)
	{
		if (w)
			present->IncrementRounds(w->NumRoundsRemaining());

		delete w;
	}

	//if not already holding, add to inventory
	else
	{
		m_WeaponMap[weapon_type] = w;
	}
}


//------------------------- GetWeaponFromInventory -------------------------------
//
//  returns a pointer to any matching weapon.
//
//  returns a null pointer if the weapon is not present
//-----------------------------------------------------------------------------
Raven_Weapon* Raven_WeaponSystem::GetWeaponFromInventory(int weapon_type)
{
	return m_WeaponMap[weapon_type];
}

//----------------------- ChangeWeapon ----------------------------------------
void Raven_WeaponSystem::ChangeWeapon(unsigned int type)
{
	Raven_Weapon* w = GetWeaponFromInventory(type);

	if (w) m_pCurrentWeapon = w;
}

//--------------------------- TryShoot ---------------------------------
//
//  this method aims the bots current weapon at the target (if there is a
//  target) and, if aimed correctly, fires a round
//-----------------------------------------------------------------------------
bool Raven_WeaponSystem::TryShoot()
{
	//aim the weapon only if the current target is shootable or if it has only
	//very recently gone out of view (this latter condition is to ensure the 
	//weapon is aimed at the target even if it temporarily dodges behind a wall
	//or other cover)
	if (m_pOwner->GetTargetSys()->isTargetShootable() ||
		(m_pOwner->GetTargetSys()->GetTimeTargetHasBeenOutOfView() <
			m_dAimPersistance))
	{
		//the position the weapon will be aimed at
		Vector2D AimingPos = m_pOwner->GetTargetBot()->Pos();

		auto velocity = m_pOwner->Velocity();
		auto distanceToTarget = Vector2D();

		auto visibleTargetDuration = m_pOwner->GetTargetSys()->GetTimeTargetHasBeenVisible();

		//if the current weapon is not an instant hit type gun the target position
		//must be adjusted to take into account the predicted movement of the 
		//target
		if (GetCurrentWeapon()->GetType() == type_rocket_launcher ||
			GetCurrentWeapon()->GetType() == type_blaster || GetCurrentWeapon()->GetType() == type_grenade_launcher)
		{
			AimingPos = PredictFuturePositionOfTarget();
			//if the weapon is aimed correctly, there is line of sight between the
			//bot and the aiming position and it has been in view for a period longer
			//than the bot's reaction time, shoot the weapon
			if (m_pOwner->RotateFacingTowardPosition(AimingPos) &&
				(m_pOwner->GetTargetSys()->GetTimeTargetHasBeenVisible() >
					m_dReactionTime) &&
				m_pOwner->hasLOSto(AimingPos))
			{
				AddNoiseToAim(AimingPos);
				GetCurrentWeapon()->ShootAt(AimingPos);
				return true;
			}
		}
		//no need to predict movement, aim directly at target
		else
		{
			//if the weapon is aimed correctly and it has been in view for a period
			//longer than the bot's reaction time, shoot the weapon
			if (m_pOwner->RotateFacingTowardPosition(AimingPos) &&
				(m_pOwner->GetTargetSys()->GetTimeTargetHasBeenVisible() >
					m_dReactionTime))
			{
				AddNoiseToAim(AimingPos);

				GetCurrentWeapon()->ShootAt(AimingPos);
				return true;
			}
		}
	}

	//no target to shoot at so rotate facing to be parallel with the bot's
	//heading direction
	else
	{
		m_pOwner->RotateFacingTowardPosition(m_pOwner->Pos() + m_pOwner->Heading());
	}

	return false;
}

//---------------------------- AddNoiseToAim ----------------------------------
//
//  adds a random deviation to the firing angle not greater than m_dAimAccuracy 
//  rads
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::AddNoiseToAim(Vector2D& Hit)
{
	auto TargetSystem = m_pOwner->GetTargetSys();

	auto TargetPosition = TargetSystem->GetTarget()->Pos();
	auto DistanceToTarget = Vec2DDistance(m_pOwner->Pos(), TargetPosition);

	auto CurrentSpeed = m_pOwner->Speed();
	auto VisibleTimeSpanTarget = TargetSystem->GetTimeTargetHasBeenVisible();

	m_FuzzyModule.Fuzzify("Distance_To_Target", DistanceToTarget);
	m_FuzzyModule.Fuzzify("Velocity", CurrentSpeed);
	m_FuzzyModule.Fuzzify("Visible_Time_Span_Target", VisibleTimeSpanTarget);

	auto Accuracy = m_FuzzyModule.DeFuzzify("Accuracy", FuzzyModule::max_av) * m_dAimAccuracy;

	auto RandomAccuracy = RandInRange(-Accuracy, Accuracy);

	auto CurrentPosition = m_pOwner->Pos();

	Hit -= CurrentPosition;

	Vec2DRotateAroundOrigin(Hit, RandomAccuracy);

	Hit += CurrentPosition;
}

//-------------------------- PredictFuturePositionOfTarget --------------------
//
//  predicts where the target will be located in the time it takes for a
//  projectile to reach it. This uses a similar logic to the Pursuit steering
//  behavior.
//-----------------------------------------------------------------------------
Vector2D Raven_WeaponSystem::PredictFuturePositionOfTarget()const
{
	double MaxSpeed = GetCurrentWeapon()->GetMaxProjectileSpeed();

	//if the target is ahead and facing the agent shoot at its current pos
	Vector2D ToEnemy = m_pOwner->GetTargetBot()->Pos() - m_pOwner->Pos();

	//the lookahead time is proportional to the distance between the enemy
	//and the pursuer; and is inversely proportional to the sum of the
	//agent's velocities
	double LookAheadTime = ToEnemy.Length() /
		(MaxSpeed + m_pOwner->GetTargetBot()->MaxSpeed());

	//return the predicted future position of the enemy
	return m_pOwner->GetTargetBot()->Pos() +
		m_pOwner->GetTargetBot()->Velocity() * LookAheadTime;
}


//------------------ GetAmmoRemainingForWeapon --------------------------------
//
//  returns the amount of ammo remaining for the specified weapon. Return zero
//  if the weapon is not present
//-----------------------------------------------------------------------------
int Raven_WeaponSystem::GetAmmoRemainingForWeapon(unsigned int weapon_type)
{
	if (m_WeaponMap[weapon_type])
	{
		return m_WeaponMap[weapon_type]->NumRoundsRemaining();
	}


	return 0;
}

//---------------------------- ShootAt ----------------------------------------
//
//  shoots the current weapon at the given position
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::ShootAt(Vector2D pos)const
{
	GetCurrentWeapon()->ShootAt(pos);
}

//-------------------------- RenderCurrentWeapon ------------------------------
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::RenderCurrentWeapon()const
{
	GetCurrentWeapon()->Render();
}

void Raven_WeaponSystem::RenderDesirabilities()const
{
	Vector2D p = m_pOwner->Pos();

	int num = 0;

	WeaponMap::const_iterator curWeap;
	for (curWeap = m_WeaponMap.begin(); curWeap != m_WeaponMap.end(); ++curWeap)
	{
		if (curWeap->second) num++;
	}

	int offset = 15 * num;

	for (curWeap = m_WeaponMap.begin(); curWeap != m_WeaponMap.end(); ++curWeap)
	{
		if (curWeap->second)
		{
			double score = curWeap->second->GetLastDesirabilityScore();
			std::string type = GetNameOfType(curWeap->second->GetType());

			gdi->TextAtPos(p.x + 10.0, p.y - offset, std::to_string(score) + " " + type);

			offset += 15;
		}
	}
}
