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

const Raven_TargetingSystem* Raven_Team::GetOwnerTargetSystem()
{
	return GetOwner()->GetTargetSys();
}

void Raven_Team::SetTarget(Raven_Bot* target)
{
	auto From = GetOwner()->ID();

	if (target->ID() == From) return;
	if (GetMembers().at(target->ID())) return;

	for (auto Bot : GetMembers()) {

		auto To = Bot.first;

		if (IsLeading(Bot.second))
			continue;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, From, To, Msg_TeamTarget, target);
	}
}
