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

	const Raven_Bot* m_pOwner;

	std::map<int, Raven_Bot*> m_pMembers;

	inline bool HasMembers() const {
		return !GetMembers().empty();
	}

	inline bool HasMember(const Raven_Bot* bot) const {

		if (bot == nullptr) return false;

		if (HasMembers())
		{
			auto id = bot->ID();

			auto members = GetMembers();

			return members.count(id);
		}

		return false;
	}

public:

	explicit Raven_Team(unsigned int id) : m_ID(id), m_pMembers(std::map<int, Raven_Bot*>()), m_pOwner(nullptr) {
	}

	inline unsigned int ID() const {
		return m_ID;
	}

	void SetOwner(Raven_Bot* owner);

	const Raven_Bot* GetOwner() const;

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	void AddMember(Raven_Bot* member);
	void RemoveMember(Raven_Bot* member);

	bool TrySetTeamTarget(Raven_Bot* target);

	bool CanLead(const Raven_Bot* bot) const;

	bool IsLeading(const Raven_Bot* bot) const;
};

