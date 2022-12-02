#pragma once

#include <set>
#include <list>

#include <Messaging/MessageDispatcher.h>

#include "Raven_Bot.h"

#include "Raven_Messages.h"

class Raven_Team
{
private:
	const Raven_Bot* m_pOwner;

	std::map<int, Raven_Bot*> m_pMembers;

public:

	explicit Raven_Team(const Raven_Bot* owner) : m_pOwner(owner), m_pMembers(std::map<int, Raven_Bot*>()) {

	}

	const Raven_Bot* GetLeader() const;

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	inline bool HasMembers() const {
		return GetMembers().empty();
	}

	inline void AddMember(Raven_Bot* member) {
		m_pMembers.insert_or_assign(member->ID(), member);
	}

	inline void RemoveMember(Raven_Bot* member) {
		m_pMembers.erase(member->ID());
	}

	void SetTarget(Raven_Bot* target);

	inline bool CanLead(const Raven_Bot* bot) const {
		return bot->isAlive() && !bot->isSpawning();
	}

	inline bool IsLeading(const Raven_Bot* bot) const {
		return GetLeader()->ID() == bot->ID();
	}
};

