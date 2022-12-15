#pragma once

#include "triggers/Trigger_WeaponGiver.h"

class Trigger_TeamWeaponGiver : public Trigger_WeaponGiver
{
private:
	const Raven_Team* m_pTeam;
public:
	Trigger_TeamWeaponGiver(const int WeaponType, int radius, const Raven_Map::GraphNode* GraphNode, const Raven_Team* Team);

	void Try(Raven_Bot* Bot) override;

	void Render() override;
};