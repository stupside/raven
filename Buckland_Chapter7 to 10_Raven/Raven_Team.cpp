#include "Raven_Team.h"
#include <Debug/DebugConsole.h>

const Raven_Bot* Raven_Team::GetOwner() const
{
	if (CanLead(m_pOwner))
		return m_pOwner;

	Raven_Bot* owner = nullptr;

	for (auto bot : GetMembers()) {

		if (CanLead(bot.second)) {

			if (bot.second->isPossessed()) {
				return bot.second;
			}
			else if (owner == nullptr)
			{
				owner = bot.second;
			}
		}
	}

	return owner;
}

void Raven_Team::SetOwner(Raven_Bot* owner) {
	m_pOwner = owner;

	debug_con << "Set " << owner->ID() << " Owner of team" << ID() << "";

	if (owner)
	{
		if (HasMember(owner)) return;

		AddMember(owner);
	}
}


void Raven_Team::AddMember(Raven_Bot* member)
{
	member->AssignTeam(this);

	m_pMembers.insert_or_assign(member->ID(), member);

	debug_con << "Added bot " << member->ID() << " to team" << ID() << "";
}

void Raven_Team::RemoveMember(Raven_Bot* member)
{
	member->AssignTeam(nullptr);

	if (IsLeading(member))
		SetOwner(nullptr);

	m_pMembers.erase(member->ID());

	debug_con << "Removed bot " << member->ID() << " from team" << ID() << "";
}

void Raven_Team::SetTarget(Raven_Bot* target)
{
	if (target == nullptr) return;

	if (IsLeading(target)) return;

	if (HasMember(target)) return;

	auto TargetID = target->ID();

	auto DismatchFrom = GetOwner()->ID();
	auto DispatchExtraInfos = (void*)&TargetID;

	for (auto Bot : GetMembers()) {

		auto DispatchTo = Bot.first;

		if (IsLeading(Bot.second))
			continue;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, DismatchFrom, DispatchTo, Msg_TeamTarget, DispatchExtraInfos);
	}

	debug_con << "Target bot " << TargetID << " for team " << ID() << "";
}

bool Raven_Team::CanLead(const Raven_Bot* bot) const
{
	if (bot)
		return bot->isAlive() && !bot->isSpawning();

	return false;
}

bool Raven_Team::IsLeading(const Raven_Bot* bot) const
{
	if (bot == nullptr) return false;

	if (HasMember(bot))
		return GetOwner()->ID() == bot->ID();

	return false;
}
