#pragma once
#include "Raven_Bot.h"

#include "CNeuralNet.h"

class Raven_Learner
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

	const Raven_Bot* m_pBot;

public:

	Raven_Learner(const Raven_Bot* bot) : m_pBot(bot) {
	}

	Datas Compute(bool shoot);

	~Raven_Learner() {
	}
};

