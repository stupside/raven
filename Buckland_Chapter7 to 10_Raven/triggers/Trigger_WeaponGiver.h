#ifndef WEAPON_GIVER_H
#define WEAPON_GIVER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_WeaponGiver.h
//
//  Author:   Mat Buckland
//
//  Desc:     This trigger 'gives' the triggering bot a weapon of the
//            specified type 
//
//-----------------------------------------------------------------------------
#include "Triggers/Trigger_Respawning.h"
#include "../Raven_Bot.h"
#include <iosfwd>
#include "../Raven_Map.h"

class Trigger_WeaponGiver : public Trigger_Respawning<Raven_Bot>
{
private:

  //vrtex buffers for rocket shape
  std::vector<Vector2D>         m_vecRLVB;
  std::vector<Vector2D>         m_vecRLVBTrans;
  
public:

  //this type of trigger is created when reading a map file
  Trigger_WeaponGiver(std::ifstream& datafile);
  Trigger_WeaponGiver(const int WeaponType, int radius, const Raven_Map::GraphNode* GraphNode);

  //if triggered, this trigger will call the PickupWeapon method of the
  //bot. PickupWeapon will instantiate a weapon of the appropriate type.
  void Try(Raven_Bot* Bot);
  
  //draws a symbol representing the weapon type at the trigger's location
  void Render();

  void RenderWeapon(int type);

  void Read (std::ifstream& is);
};

#endif