#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"

#include "Raven_Team.h"



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner) :m_pOwner(owner),
m_pCurrentTarget(0)
{}

//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::Update()
{
	double SmallestDistToTarget = MaxDouble;

	Raven_Bot* Target = nullptr;

	const std::list<Raven_Bot*> SensedBots = m_pOwner->GetSensoryMem()->GetListOfRecentlySensedOpponents();

	std::list<Raven_Bot*>::const_iterator CurrentBot = SensedBots.begin();

	for (CurrentBot; CurrentBot != SensedBots.end(); ++CurrentBot)
	{
		if (*CurrentBot == m_pOwner) continue;

		//make sure the bot is alive and that it is not the owner
		if ((*CurrentBot)->isAlive())
		{
			double DistToTarget = Vec2DDistanceSq((*CurrentBot)->Pos(), m_pOwner->Pos());

			if (DistToTarget < SmallestDistToTarget)
			{
				SmallestDistToTarget = DistToTarget;

				Target = *CurrentBot;
			}
		}
	}

	auto* OwnerTeam = m_pOwner->GetTeam();

	if (OwnerTeam) {
		const auto IsLeader = OwnerTeam->IsLeading(m_pOwner);

		if (IsLeader)
		{
			m_pCurrentTarget = Target;

			OwnerTeam->SetTarget(Target);
		}
	}
	else {
		m_pCurrentTarget = Target;
	}
}




bool Raven_TargetingSystem::isTargetWithinFOV()const
{
	return m_pOwner->GetSensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool Raven_TargetingSystem::isTargetShootable()const
{
	return m_pOwner->GetSensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D Raven_TargetingSystem::GetLastRecordedPosition()const
{
	return m_pOwner->GetSensoryMem()->GetLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenVisible()const
{
	return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenOutOfView()const
{
	return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}
