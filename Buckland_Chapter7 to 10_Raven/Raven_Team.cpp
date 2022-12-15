#include "Raven_Team.h"
#include "Goals/Goal_Think.h"
#include <Debug/DebugConsole.h>

 Raven_Bot* Raven_Team::GetLeader() const
{
	Raven_Bot* Leader = nullptr;

	for (auto Member : GetMembers()) {

		if (CanLead(Member.second)) {

			if (Member.second->isPossessed()) {
				return Member.second;
			}
			else if (Leader == nullptr)
			{
				Leader = Member.second;
			}
		}
	}

	return Leader;
}


void Raven_Team::AddMember(Raven_Bot* Member)
{
	Member->AssignTeam(this);

	Member->GetBrain()->RemoveAllSubgoals();

	Member->GetBrain()->AddGoal_FollowTeam();

	m_pMembers.insert_or_assign(Member->ID(), Member);

	debug_con << "Added bot " << Member->ID() << " to team" << ID() << "";
}

void Raven_Team::RemoveMember(Raven_Bot* Member)
{
	Member->AssignTeam(nullptr);

	Member->GetBrain()->RemoveAllSubgoals();

	m_pMembers.erase(Member->ID());

	debug_con << "Removed bot " << Member->ID() << " from team" << ID() << "";
}

bool Raven_Team::TrySetTeamTarget(Raven_Bot* Target)
{
	if (Target == nullptr) return false;

	if (HasMember(Target))
		return false;

	auto TargetID = Target->ID();

	auto DismatchFrom = GetLeader()->ID();
	auto DispatchExtraInfos = (void*)&TargetID;

	for (auto Bot : GetMembers()) {

		auto DispatchTo = Bot.first;

		if (IsLeadingTeam(Bot.second)) continue;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, DismatchFrom, DispatchTo, Msg_TeamTarget, DispatchExtraInfos);
	}

	debug_con << "Set target " << TargetID << " for team " << ID() << "";
}

bool Raven_Team::CanLead(const Raven_Bot* Bot) const
{
	if (Bot)
		return Bot->isAlive() && !Bot->isSpawning();

	return false;
}

bool Raven_Team::IsLeadingTeam(const Raven_Bot* Bot) const
{
	if (Bot == nullptr) return false;

	return GetLeader()->ID() == Bot->ID();
}
