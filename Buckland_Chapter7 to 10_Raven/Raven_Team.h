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

	explicit Raven_Team() : m_pMembers(std::map<int, Raven_Bot*>()), m_pOwner(nullptr) {
	}

	inline void SetOwner(const Raven_Bot* owner) {
		m_pOwner = owner;
	}

	const Raven_Bot* GetOwner() const;

	const Raven_TargetingSystem* GetOwnerTargetSystem();

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	inline bool HasMembers() const {
		return GetMembers().empty();
	}

	inline void AddMember(Raven_Bot* member, bool owner = false) {

		m_pMembers.insert_or_assign(member->ID(), member);

		member->AssignTeam(this);

		if (owner) {
			SetOwner(member);
		}
	}

	inline void RemoveMember(Raven_Bot* member) {
		m_pMembers.erase(member->ID());

		member->AssignTeam(nullptr);
	}

	void SetTarget(Raven_Bot* target);

	inline bool CanLead(const Raven_Bot* bot) const {
		return bot->isAlive() && !bot->isSpawning();
	}

	inline bool IsLeading(const Raven_Bot* bot) const {
		return GetOwner()->ID() == bot->ID();
	}
};

