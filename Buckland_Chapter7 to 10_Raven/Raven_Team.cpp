#include "Raven_Team.h"

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
}

void Raven_Team::RemoveMember(Raven_Bot* member)
{
	member->AssignTeam(nullptr);

	if (IsLeading(member))
		SetOwner(nullptr);

	m_pMembers.erase(member->ID());
}

void Raven_Team::SetTarget(Raven_Bot* target)
{
	auto From = GetOwner()->ID();

	if (IsLeading(target)) return;

	if (HasMember(target)) return;

	for (auto Bot : GetMembers()) {

		auto To = Bot.first;

		if (IsLeading(Bot.second))
			continue;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, From, To, Msg_TeamTarget, target);
	}
}

bool Raven_Team::CanLead(const Raven_Bot* bot) const
{
	if (bot)
		return bot->isAlive() && !bot->isSpawning();

	return false;
}

bool Raven_Team::IsLeading(const Raven_Bot* bot) const
{
	if (HasMember(bot))
		return GetOwner()->ID() == bot->ID();

	return false;
}
