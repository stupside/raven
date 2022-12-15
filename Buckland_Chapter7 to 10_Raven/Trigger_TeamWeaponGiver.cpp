#include "Trigger_TeamWeaponGiver.h"

#include "Raven_Bot.h"
#include "Game/BaseGameEntity.h"

#include "lua/Raven_Scriptor.h"
#include "triggers/Trigger_Respawning.h"
#include "constants.h"
#include "Raven_Team.h"

Trigger_TeamWeaponGiver::Trigger_TeamWeaponGiver(const int WeaponType, int radius, const Raven_Map::GraphNode* GraphNode, const Raven_Team* Team) :
    Trigger_WeaponGiver(WeaponType, radius, GraphNode)
{
    m_pTeam = Team;
}

void Trigger_TeamWeaponGiver::Try(Raven_Bot* Bot)
{
    const auto* Team = Bot->GetTeam();

    if (Team) {
        const bool IsMember = Team->HasMember(Bot);

        if (IsMember) {
            Trigger_WeaponGiver::Try(Bot);
        }
    }
}

void Trigger_TeamWeaponGiver::Render()
{
    Trigger_WeaponGiver::Render();

    gdi->TextAtPos(Pos().x - 40, Pos().y - 20, "Team W: " + std::to_string(m_pTeam->ID()));
}
