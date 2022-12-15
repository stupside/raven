#include "FollowTeamGoal_Evaluator.h"

#include "goals/Goal_Think.h"
#include "goals/Raven_Feature.h"
#include "Raven_Team.h"

double FollowTeamGoald_Evaluator::CalculateDesirability(Raven_Bot* pBot)
{
    double Desirability = 0.0;

    auto Team = pBot->GetTeam();

    //only do the calculation if there is a target present
    if (Team)
    {
        const double Tweaker = 1.0;

        auto Leader = Team->GetLeader();

        Desirability = Tweaker * Raven_Feature::Health(pBot) * Raven_Feature::TotalWeaponStrength(pBot);
        
        //bias the value according to the personality of the bot
        Desirability *= m_dCharacterBias;
    }

    return Desirability;
}

void FollowTeamGoald_Evaluator::SetGoal(Raven_Bot* pEnt)
{
	pEnt->GetBrain()->AddGoal_FollowTeam();
}

void FollowTeamGoald_Evaluator::RenderInfo(Vector2D Position, Raven_Bot* pBot)
{
}
