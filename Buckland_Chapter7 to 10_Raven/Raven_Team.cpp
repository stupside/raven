#include "Raven_Team.h"

const Raven_Bot* Raven_Team::GetLeader() const
{
	if (m_pOwner->isDead()) {
		for (auto bot : GetMembers()) {
			if (CanLead(bot.second)) return bot.second;
		}

		return nullptr;
	}

	return m_pOwner;
}

void Raven_Team::SetTarget(Raven_Bot* target)
{
	auto From = m_pOwner->ID();

	for (auto Bot : GetMembers()) {

		auto To = Bot.first;

		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY, From, To, Msg_TeamTarget, target);
	}
}
