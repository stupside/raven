#include "Raven_Learner.h"

#include "Raven_WeaponSystem.h"
#include "Raven_TargetingSystem.h"

#include "Messaging/Telegram.h"
#include "Raven_Messages.h"

#include "armory/Raven_Weapon.h"
#include "Raven_SensoryMemory.h"

#include <iostream>
#include <fstream>

#include <thread>
#include <Debug/DebugConsole.h>
#include <misc/Cgdi.h>

#define GET_VARIABLE_NAME(variable) (#variable)
#define MAX_TRAINING_CDATA_SIZE 200
#define MIN_TRAINING_BOT_SCORE 0


Raven_Learner::Datas Raven_Learner::GetTargetSet(Raven_Bot* Target) const
{
	auto BotTargetingSystem = GetTargetSys();

	auto TargetDistance = Target ? Vec2DDistance(Pos(), Target->Pos()) : 0; // save

	auto TargetVisibleTimeSpan = Target ? GetSensoryMem()->GetTimeOpponentHasBeenVisible(Target) : 0; // save

	auto TargetIsShootable = BotTargetingSystem->isTargetShootable(); // save
	auto TargetInFOV = BotTargetingSystem->isTargetWithinFOV(); // save
	auto TargetHealth = Target ? Target->Health() : 0; // save

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

void Raven_Learner::OnShootHit(Raven_ShootHitContext Hit, Raven_Bot* Target)
{
	const auto* Bot = this;

	if (Bot->Score() <= MIN_TRAINING_BOT_SCORE) return;

	Raven_Learner::Datas Datas
	{
		Hit.Speed,
		(double)Hit.WeaponType,
	};

	auto Targets = GetTargetSet(Target);

	Targets.push_back(Hit.Hit); // decision

	auto* Neural = Raven_Learner::Neural::Instance();

	Neural->AddData(Datas, Targets);

	auto* CNeuralNetwork = Neural->GetCNeuralNet();

	Raven_Learner::Datas Inputs;

	Inputs.insert(Inputs.end(), Datas.begin(), Datas.end());
	Inputs.insert(Inputs.end(), Targets.begin(), Targets.end());

	auto outputs = CNeuralNetwork->Update(Inputs);
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

void Raven_Learner::Render()
{
	Raven_Bot::Render();

	gdi->TextAtPos(Pos().x - 40, Pos().y + 15, "Learner");
}

void Raven_Learner::Neural::Train()
{
	m_bTraining = GetCNeuralNet()->Train(m_pCData);
}

bool Raven_Learner::Neural::TryStartTrainThread()
{
	if (m_bTraining) {
		return false;
	}

	debug_con << "Starting train thread" << "";

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
