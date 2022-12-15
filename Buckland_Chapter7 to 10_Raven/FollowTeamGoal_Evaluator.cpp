#include "FollowTeamGoal_Evaluator.h"

#include "goals/Goal_Think.h"
#include "goals/Raven_Feature.h"
#include "Raven_Team.h"
#include "Raven_Game.h"

double FollowTeamGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
	double Desirability = 0.0;

	auto Team = pBot->GetTeam();

	if (Team)
	{
		const double Tweaker = 1.5;

		if (Team->IsLeadingTeam(pBot)) return 0;

		auto Leader = Team->GetLeader();

		auto DistanceToLeader = pBot->Pos() - Leader->Pos();

		auto MapSizeX = pBot->GetWorld()->GetMap()->GetSizeX();
		auto MapSizeY = pBot->GetWorld()->GetMap()->GetSizeY();

		auto DistanceTweaker = (DistanceToLeader.x + DistanceToLeader.y) / (MapSizeX + MapSizeY);

		Desirability = Tweaker * (1 - Raven_Feature::Health(Leader)) * DistanceTweaker;

		Clamp(Desirability, 0, 1);

		Desirability *= m_dCharacterBias;
	}

	return Desirability;
}

void FollowTeamGoal_Evaluator::SetGoal(Raven_Bot* pEnt)
{
	pEnt->GetBrain()->AddGoal_FollowTeam();
}

void FollowTeamGoal_Evaluator::RenderInfo(Vector2D Position, Raven_Bot* pBot)
{
}
