#include "Raven_Team.h"

const Raven_Bot* Raven_Team::GetOwner() const
{
	if (m_pOwner == nullptr || m_pOwner->isDead()) {
		for (auto bot : GetMembers()) {
			if (CanLead(bot.second)) return bot.second;
		}

		return nullptr;
	}

	return m_pOwner;
}

const Raven_TargetingSystem* Raven_Team::GetOwnerTargetSystem()
{
	return GetOwner()->GetTargetSys();
}

void Raven_Team::SetTarget(Raven_Bot* target)
{
	auto From = GetOwner()->ID();

	for (auto Bot : GetMembers()) {

		auto To = Bot.first;

		if (IsLeading(Bot.second))
			continue;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, From, To, Msg_TeamTarget, target);
	}
}
