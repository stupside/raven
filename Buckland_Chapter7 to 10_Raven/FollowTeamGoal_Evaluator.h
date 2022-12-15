#pragma once

// #pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   ExploreGoal_Evaluator.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:  class to calculate how desirable the goal of exploring is
//-----------------------------------------------------------------------------

#include "goals/Goal_Evaluator.h"

#include "Raven_Bot.h"

class FollowTeamGoald_Evaluator : public Goal_Evaluator
{
public:

	FollowTeamGoald_Evaluator(double bias) :Goal_Evaluator(bias) {}

	double CalculateDesirability(Raven_Bot* pBot) override;

	void  SetGoal(Raven_Bot* pEnt) override;

	void RenderInfo(Vector2D Position, Raven_Bot* pBot) override;
};


