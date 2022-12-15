#pragma once
#include "Raven_Bot.h"

#include "CNeuralNet.h"

class Raven_Learner : public Raven_Bot
{
public:
	typedef double Data;

	typedef std::vector<Raven_Learner::Data> Datas;

	class Neural {
	private:

		bool m_bTraining;

		CData* m_pCData;
		CNeuralNet* m_pCNeuralNetwork;

		Neural() : m_pCData(nullptr), m_pCNeuralNetwork(nullptr) {}

		void Train();
		bool TryStartTrainThread();
	public:

		static Neural* Instance();

		CNeuralNet* GetCNeuralNet();

		void LoadDatas(std::string path);

		void AddData(Datas datas, Datas targets);

		Neural(Neural& other) = delete;
		void operator=(const Neural&) = delete;
	};
private:

	Datas ComputeShoot(bool shoot);

	Datas GetTargetSet() const;
	Datas GetDataSet() const;

public:

	Raven_Learner(Raven_Game* world, Vector2D pos) : Raven_Bot(world, pos) {
	}

	bool MayShoot() override;

	bool HandleMessage(const Telegram& msg) override;

	void Update() override;

	void Render() override;
};



