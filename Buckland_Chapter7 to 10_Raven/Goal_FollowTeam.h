#include "Raven_Bot.h"

#include "Goals/Goal.h"

#include "Goals/Goal_Composite.h"

#pragma once
class Goal_FollowTeam : public Goal_Composite<Raven_Bot>
{
private:
	Vector2D m_target;
public:

	Goal_FollowTeam(Raven_Bot* pE);

	void Activate() override;
	int  Process() override;
	void Terminate() override;

	void Render() override;
};

