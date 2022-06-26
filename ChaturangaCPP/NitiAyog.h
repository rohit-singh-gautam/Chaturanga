#pragma once
#include "Ashtapada.h"
#include "Evaluator.h"
#include "LRU.h"
#include <chrono>
#include <algorithm>

namespace rohit {
	struct NitiAyogStat {
		std::chrono::duration<double> elapsedTime;
		std::chrono::duration<double> elapsedTimeMax;
		size_t boardEvaluated;
		size_t boardEvaluatedMax;
		NitiAyogStat() : boardEvaluated(0), boardEvaluatedMax(0), elapsedTime(), elapsedTimeMax() {}
	};

	std::ostream & operator<<(std::ostream & os, const NitiAyogStat &nitiAyogStat);

	class NitiAyog : public Niti
	{
		static const niti_depth_type max_depth = 9;
		static const int max_moves = 400;
		const niti_depth_type depth;
		MohraChal::StoreType movesList[max_depth];
		Evaluator &evaluator;

		NitiAyogStat stat;
	public:
		inline NitiAyog(Ashtapada &ashtapada, Evaluator &evaluator, niti_depth_type depth) : Niti(ashtapada),
			evaluator(evaluator), depth(std::min(depth, max_depth)) {}

		const Chal getNextMove(niti_depth_type depth = -1) override;
		const NitiAyogStat &getStat() const { return stat; }

		int minmax(niti_depth_type depth, board_score_type alpha, board_score_type beta);
	};

	struct NitiAyogStatLRU {
		std::chrono::duration<double> elapsedTime;
		std::chrono::duration<double> elapsedTimeMax;
		size_t boardEvaluated;
		size_t boardEvaluatedMax;
		size_t skipped;
		size_t skippedMax;
		NitiAyogStatLRU() : boardEvaluated(0), boardEvaluatedMax(0), elapsedTime(), elapsedTimeMax(), skipped(0), skippedMax(0) {}
	};

	std::ostream & operator<<(std::ostream & os, const NitiAyogStatLRU &nitiAyogStat);

	struct niti_lru_entry {
		board_score_type score;
		niti_depth_type depth;
	};

	class NitiAyogLRU : public Niti
	{
		static const niti_depth_type max_depth = 9;
		static const size_t lru_capacity = 40000000;
		static const int max_moves = 400;
		const niti_depth_type depth;
		MohraChal::StoreType movesList[max_depth];
		Evaluator &evaluator;
		rohit::lru_map<board_hash_type, niti_lru_entry> map;

		NitiAyogStatLRU stat;
	public:
		inline NitiAyogLRU(Ashtapada &ashtapada, Evaluator &evaluator, const niti_depth_type depth) : Niti(ashtapada),
			evaluator(evaluator), depth(std::min(depth, max_depth)),
			map(lru_capacity) {}

		const Chal getNextMove(niti_depth_type depth = -1) override;
		const NitiAyogStatLRU &getStat() const { return stat; }

		int minmax(niti_depth_type depth, board_score_type alpha, board_score_type beta);
	};
}

