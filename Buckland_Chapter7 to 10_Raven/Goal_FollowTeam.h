#include "Raven_Bot.h"

#include "Goals/Goal.h"

#pragma once
class Goal_FollowTeam : public Goal<Raven_Bot>
{
private:
public:

	Goal_FollowTeam(Raven_Bot* pE);

	void Activate() override;
	int  Process() override;
	void Terminate() override;

	void Render() override;
};

