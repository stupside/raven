#pragma once

class Raven_ShootHitContext
{
public:
    const bool Hit;
    const float Speed;
    const unsigned int WeaponType;

    Raven_ShootHitContext(const bool Hit, const float Speed, const unsigned int WeaponType) : Hit(Hit), Speed(Speed), WeaponType(WeaponType) {

    }
};

