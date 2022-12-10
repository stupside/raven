#include "Raven_Learner.h"

#include "Raven_WeaponSystem.h"
#include "Raven_TargetingSystem.h"

#include "Messaging/Telegram.h"
#include "Raven_Messages.h"

#include "armory/Raven_Weapon.h"

#include <iostream>
#include <fstream>

#include <thread>

#define GET_VARIABLE_NAME(variable) (#variable)
#define MAX_TRAINING_CDATA_SIZE 200
#define MIN_TRAINING_BOT_SCORE 1

#define SHOOT 1
#define DONT_SHOOT 0

Raven_Learner::Datas Raven_Learner::ComputeShoot(bool HasShooted)
{
	auto* Bot = this;

	const auto Datas = GetDataSet();

	auto Targets = GetTargetSet();

	Targets.push_back(HasShooted); // decision

	auto* Neural = Raven_Learner::Neural::Instance();

	if (Bot->Score() <= MIN_TRAINING_BOT_SCORE) return Raven_Learner::Datas();

	Neural->AddData(Datas, Targets);

	auto* CNeuralNetwork = Neural->GetCNeuralNet();

	auto InputsReserver = Datas.size() + Targets.size();

	Raven_Learner::Datas Inputs(InputsReserver);

	Inputs.insert(Datas.end(), Targets.begin(), Targets.end());

	auto outputs = CNeuralNetwork->Update(Inputs);

	return outputs;
}

Raven_Learner::Datas Raven_Learner::GetTargetSet() const
{
	auto* Bot = this;
	auto* BotTargetingSystem = Bot->GetTargetSys();

	auto* Target = BotTargetingSystem->GetTarget();

	auto TargetDistance = Vec2DDistance(Bot->Pos(), Target->Pos()); // save
	auto TargetVisibleTimeSpan = BotTargetingSystem->GetTimeTargetHasBeenVisible(); // save
	auto TargetIsShootable = BotTargetingSystem->isTargetShootable(); // save
	auto TargetInFOV = BotTargetingSystem->isTargetWithinFOV(); // save
	auto TargetHealth = Target->Health(); // save

	Raven_Learner::Datas Datas
	{
		TargetDistance,
		TargetVisibleTimeSpan,
		(double)TargetIsShootable,
		(double)TargetInFOV,
		(double)TargetHealth,
	};

	return Datas;
}

Raven_Learner::Datas Raven_Learner::GetDataSet() const
{
	auto* Bot = this;
	auto* BotTargetingSystem = Bot->GetTargetSys();

	auto Velocity = Bot->Velocity(); // save
	auto Speed = Bot->Speed(); // save

	auto* BotWeaponSystem = Bot->GetWeaponSys();
	auto* Weapon = BotWeaponSystem->GetCurrentWeapon();

	auto WeaponType = Weapon->GetType(); // save

	Raven_Learner::Datas Datas
	{
		Velocity.LengthSq(),
		Speed,
		(double)WeaponType,
	};

	return Datas;
}

bool Raven_Learner::MayShoot()
{
	auto Shooted = Raven_Bot::MayShoot();

	if (GetTargetSys()->isTargetPresent()) {
		ComputeShoot(Shooted);
	}

	return Shooted;
}

bool Raven_Learner::HandleMessage(const Telegram& msg)
{
	auto Handled = Raven_Bot::HandleMessage(msg);

	return Handled;
}

void Raven_Learner::Update()
{
	Raven_Bot::Update();
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
