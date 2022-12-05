#include "Raven_Learner.h"

#include "Raven_WeaponSystem.h"
#include "Raven_TargetingSystem.h"

#include "armory/Raven_Weapon.h"

#include <iostream>
#include <fstream>

#include <thread>

#define GET_VARIABLE_NAME(variable) (#variable)
#define MAX_TRAINING_CDATA_SIZE 200
#define MIN_TRAINING_BOT_SCORE 1

#define SHOOT 1
#define DONT_SHOOT 0

Raven_Learner::Datas Raven_Learner::Compute(bool shoot)
{
	auto* Bot = m_pBot;
	auto* BotTargetingSystem = Bot->GetTargetSys();

#pragma region Data

	auto Velocity = Bot->Velocity(); // save
	auto Speed = Bot->Speed(); // save

	auto* BotWeaponSystem = Bot->GetWeaponSys();
	auto* Weapon = BotWeaponSystem->GetCurrentWeapon();

	auto WeaponType = Weapon->GetType(); // save

#pragma endregion

#pragma region Target

	auto* Target = BotTargetingSystem->GetTarget();

	auto TargetDistance = Vec2DDistance(Bot->Pos(), Target->Pos()); // save
	auto TargetVisibleTimeSpan = BotTargetingSystem->GetTimeTargetHasBeenVisible(); // save
	auto TargetIsShootable = BotTargetingSystem->isTargetShootable(); // save
	auto TargetInFOV = BotTargetingSystem->isTargetWithinFOV(); // save
	auto TargetHealth = Target->Health(); // save

#pragma endregion

	Raven_Learner::Datas Datas
	{
		Speed,
		(double)WeaponType,
	};

	Raven_Learner::Datas Targets
	{
		TargetDistance,
		TargetVisibleTimeSpan,
		(double)TargetIsShootable,
		(double)TargetInFOV,
		(double)TargetHealth,
	};

	Targets.push_back(shoot); // decision

	auto* Neural = Raven_Learner::Neural::Instance();

	if (m_pBot->Score() <= MIN_TRAINING_BOT_SCORE) return Raven_Learner::Datas();

	Neural->AddData(Datas, Targets);

	auto* CNeuralNetwork = Neural->GetCNeuralNet();

	auto InputsReserver = Datas.size() + Targets.size();

	Raven_Learner::Datas Inputs(InputsReserver);

	Inputs.insert(Datas.end(), Targets.begin(), Targets.end());

	auto outputs = CNeuralNetwork->Update(Inputs);

	return outputs;
}

void Raven_Learner::Neural::Train()
{
	m_bTraining = GetCNeuralNet()->Train(m_pCData);
}

bool Raven_Learner::Neural::TryStartTrainThread()
{
	if (m_bTraining) return false;

	auto thread = std::thread(&Raven_Learner::Neural::Train, this);

	thread.detach();

	return true;
}

Raven_Learner::Neural* Raven_Learner::Neural::Instance()
{
	static Raven_Learner::Neural* instance = new Raven_Learner::Neural();

	return instance;
}

CNeuralNet* Raven_Learner::Neural::GetCNeuralNet()
{
	if (m_pCData == nullptr) return nullptr;

	static CNeuralNet* instance = new CNeuralNet(m_pCData->GetInputNb(), m_pCData->GetTargetsNb(), NUM_HIDDEN_NEURONS, LEARNING_RATE);

	TryStartTrainThread();

	return instance;
}

void Raven_Learner::Neural::LoadDatas(std::string path)
{
	Raven_Learner::Datas Datas;

	std::ifstream m_ifstream;

	m_ifstream.open(path);

	if (m_ifstream.good()) {

		std::string Value;

		while (std::getline(m_ifstream, Value)) {
			Datas.push_back(std::stod(Value));
		}
	}

	m_ifstream.close();
}

void Raven_Learner::Neural::AddData(Datas datas, Datas targets)
{
	if (datas.size() && targets.size()) {

		if (m_pCData == nullptr) {
			m_pCData = new CData(datas.size(), targets.size());
		}
		else {
			if (m_pCData->GetInputNb()) {
			}
			else {
				m_pCData = new CData(datas.size(), targets.size());
			}
		}

		if (datas.size() == m_pCData->GetInputNb() && targets.size() == m_pCData->GetTargetsNb()) {
			if (m_pCData->GetInputSet().size() < MAX_TRAINING_CDATA_SIZE) {
				m_pCData->AddData(datas, targets);
			}
		}
	}
}
