#pragma once

#include <set>
#include <list>

#include <Messaging/MessageDispatcher.h>

#include "Raven_Bot.h"

#include "Raven_Messages.h"
#include "Raven_Map.h"

class Raven_Team
{
private:

	unsigned int m_ID;
	const Raven_Map::GraphNode* m_WeaponSpawn;

	std::map<int, Raven_Bot*> m_pMembers;

public:

	explicit Raven_Team(unsigned int Id, const Raven_Map::GraphNode* WeaponSpawn) : m_ID(Id), m_WeaponSpawn(WeaponSpawn), m_pMembers(std::map<int, Raven_Bot*>()) {
	}

	inline unsigned int ID() const {
		return m_ID;
	}

	inline bool HasMember(const Raven_Bot* Bot) const {

		if (Bot == nullptr) return false;

		return Bot->GetTeam() == this;
	}

	Raven_Bot* GetLeader() const;

	inline const Raven_Map::GraphNode* GetWeaponSpawn() const { return m_WeaponSpawn; }

	inline const std::map<int, Raven_Bot*> GetMembers() const {
		return m_pMembers;
	}

	inline const std::set<int> GetMemberIds() const {

		std::set<int> Ids;

		const auto Members = GetMembers();

		for (auto Member : Members) {
			Ids.insert(Member.first);
		}

		return Ids;
	}

	void AddMember(Raven_Bot* Member);
	void RemoveMember(Raven_Bot* Member);

	bool TrySetTeamTarget(Raven_Bot* Target);

	bool CanLead(const Raven_Bot* Bot) const;

	bool IsLeadingTeam(const Raven_Bot* Bot) const;
};

