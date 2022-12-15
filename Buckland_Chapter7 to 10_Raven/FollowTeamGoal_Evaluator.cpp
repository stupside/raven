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
        const double Tweaker = 1.5;

        auto Leader = Team->GetLeader();

        if (Leader == pBot) return 0;

        Desirability = Tweaker * (1 - Raven_Feature::Health(Leader));
        
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
