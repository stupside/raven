#include "FollowTeamGoal_Evaluator.h"

#include "goals/Goal_Think.h"
#include "goals/Raven_Feature.h"
#include "Raven_Team.h"

double FollowTeamGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
    double Desirability = 0.0;

    auto Team = pBot->GetTeam();

    //only do the calculation if there is a target present
    if (Team)
    {
        const double Tweaker = 1.0;

        auto Leader = Team->GetLeader();

        Desirability = Tweaker * (1 - Raven_Feature::Health(Leader));
        
        Desirability *= m_dCharacterBias;

        Clamp(Desirability, 0, 1);
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
