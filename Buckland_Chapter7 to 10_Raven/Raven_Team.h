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

	inline void SetOwner(Raven_Bot* owner) {
		m_pOwner = owner;

		if (owner)
		{
			if (HasMember(owner)) return;

			AddMember(owner);
		}
	}

	const Raven_Bot* GetOwner() const;

	const Raven_TargetingSystem* GetOwnerTargetSystem();

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	inline bool HasMembers() const {
		return GetMembers().empty();
	}

	inline bool HasMember(const Raven_Bot* bot) const {
		return GetMembers().at(bot->ID());
	}

	inline void AddMember(Raven_Bot* member) {
		member->AssignTeam(this);

		m_pMembers.insert_or_assign(member->ID(), member);
	}

	inline void RemoveMember(Raven_Bot* member) {

		member->AssignTeam(nullptr);

		if (m_pOwner->ID() == member->ID())
			SetOwner(nullptr);

		m_pMembers.erase(member->ID());
	}

	void SetTarget(Raven_Bot* target);

	inline bool CanLead(const Raven_Bot* bot) const {
		if(bot)
			return bot->isAlive() && !bot->isSpawning();

		return false;
	}

	inline bool IsLeading(const Raven_Bot* bot) const {
		if(HasMember(bot))
			return GetOwner()->ID() == bot->ID();

		return false;
	}
};

