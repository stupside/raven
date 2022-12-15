#pragma once

#include <set>
#include <list>

#include <Messaging/MessageDispatcher.h>

#include "Raven_Bot.h"

#include "Raven_Messages.h"

class Raven_Team
{
private:

	unsigned int m_ID;

	std::map<int, Raven_Bot*> m_pMembers;

	inline bool HasMembers() const {
		return !GetMembers().empty();
	}

	inline bool HasMember(const Raven_Bot* Bot) const {

		if (Bot == nullptr) return false;

		if (HasMembers())
		{
			auto id = Bot->ID();

			auto members = GetMembers();

			return members.count(id);
		}

		return false;
	}

public:

	explicit Raven_Team(unsigned int Id) : m_ID(Id), m_pMembers(std::map<int, Raven_Bot*>()) {
	}

	inline unsigned int ID() const {
		return m_ID;
	}

	Raven_Bot* GetLeader() const;

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	void AddMember(Raven_Bot* Member);
	void RemoveMember(Raven_Bot* Member);

	bool TrySetTeamTarget(Raven_Bot* Target);

	bool CanLead(const Raven_Bot* Bot) const;

	bool IsLeadingTeam(const Raven_Bot* Bot) const;
};

