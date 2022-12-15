#include "Goal_FollowTeam.h"
#include "Raven_Team.h"
#include "Raven_SteeringBehaviors.h"

#include "Goals/Goal_Explore.h"
#include "Goals/Goal_MoveToPosition.h"

#include <Time/CrudeTimer.h>
#include "Goals/Raven_Goal_Types.h"
#include "Raven_Bot.h"

Goal_FollowTeam::Goal_FollowTeam(Raven_Bot* pE) : Goal_Composite<Raven_Bot>(pE, goal_follow_team) {
}

void Goal_FollowTeam::Activate()
{
	m_iStatus = active;

	auto* Bot = m_pOwner;
	auto* Team = Bot->GetTeam();

	if (Team) {
		if (Team->IsLeadingTeam(Bot)) {
			m_iStatus = completed;
		}
		else {

			auto Members = Team->GetMembers();

			m_target.Zero();

			for (auto Member : Members) {
				m_target += m_target;
			}

			m_target /= Members.size();

			if (!Bot->canWalkTo(m_target)) {
				m_target = Team->GetLeader()->Pos();
			}

			AddSubgoal(new Goal_MoveToPosition(m_pOwner, m_target));
		}
	}
	else {
		m_iStatus = completed;
	}
}

int Goal_FollowTeam::Process()
{
	ActivateIfInactive();

	if (m_pOwner->isAtPosition(m_target))
	{
		m_iStatus = completed;
	}
	else {
		m_iStatus = ProcessSubgoals();
	}

	return m_iStatus;
}

void Goal_FollowTeam::Terminate()
{
}

void Goal_FollowTeam::Render()
{
	Goal_Composite<Raven_Bot>::Render();
}
