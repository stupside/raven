#include "Goal_FollowTeam.h"
#include "Raven_Team.h"
#include "Raven_SteeringBehaviors.h"

#include <Time/CrudeTimer.h>
#include "Goals/Raven_Goal_Types.h"
#include "Raven_Bot.h"

Goal_FollowTeam::Goal_FollowTeam(Raven_Bot* pE) : Goal<Raven_Bot>(pE, goal_follow_team) {
}

void Goal_FollowTeam::Activate()
{
	m_iStatus = active;

	auto* Bot = m_pOwner;
	auto* Team = Bot->GetTeam();

	if (Team) {
		if (Team->IsLeadingTeam(Bot)) {
			m_iStatus = failed;
		}
		else {
			Bot->GetSteering()->OffsetPursuitOn(Team->GetLeader(), Vector2D(20, 20));
		}
	}
}

int Goal_FollowTeam::Process()
{
	ActivateIfInactive();

	auto* Bot = m_pOwner;
	auto* Team = Bot->GetTeam();

	if (Team) {

		if (Team->IsLeadingTeam(Bot)) {
		}
		else {
			m_iStatus = completed;
		}
	}
	else {
		m_iStatus = completed;
	}

	return m_iStatus;
}

void Goal_FollowTeam::Terminate()
{
	auto* Bot = m_pOwner;

	Bot->GetSteering()->OffsetPursuitOff();

	m_iStatus = completed;
}

void Goal_FollowTeam::Render()
{
}
