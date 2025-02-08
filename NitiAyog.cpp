#include "NitiAyog.h"
#include <algorithm>
#include <random>
#include <vector>

namespace rohit {

	const Chal NitiAyog::getNextMove(niti_depth_type depth) {
		if (depth == -1) depth = this->depth;
		auto startTime = std::chrono::high_resolution_clock::now();

		const player_t player = ashtapada.getCurrentPlayer();
		const MohraChal::StoreType &moves = getValidMoves();

		std::vector<Chal> bestChal;
		const int playerSign = player.getSign();

		stat.boardEvaluated = moves.size(); // Note we are initializing it here hence not +=

		board_score_type bestScore = board_score_type_min;
		for (const Chal &chal : moves) {
			evaluator.apply(chal);
			if (!applyChalNoValidation(chal)) {
				evaluator.undo(chal);
				continue;
			}
			board_score_type score = minmax(depth - 1, board_score_type_min, board_score_type_max) * playerSign;
			undoChalNoValidation();
			evaluator.undo(chal);
			if (score > bestScore) {
				bestChal.clear();
				bestChal.push_back(chal);
				bestScore = score;
			}
			else if (score == bestScore) bestChal.push_back(chal);
		}

		auto seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
		Chal ret = bestChal[seed % bestChal.size()];

		auto endTime = std::chrono::high_resolution_clock::now();
		stat.elapsedTime = endTime - startTime;
		if (stat.elapsedTime > stat.elapsedTimeMax) stat.elapsedTimeMax = stat.elapsedTime;
		stat.boardEvaluatedMax = std::max(stat.boardEvaluatedMax, stat.boardEvaluated);
		return ret;
	}

	int NitiAyog::minmax(niti_depth_type currdepth, board_score_type alpha, board_score_type beta) {
		player_t currentPlayer = ashtapada.getCurrentPlayer();
		if (currdepth == 0) {
			if (ashtapada.isChecked() != player_t::none) {
				assert(ashtapada.isChecked() == currentPlayer);
				// Find checkmate
				MohraChal::StoreType &moves = movesList[currdepth];
				moves.clear();
				ashtapada.fillAllMoves(moves);
				int applied = 0;
				for (const Chal &chal : moves) {
					if (!applyChalNoValidation(chal)) {
						continue;
					}
					applied++;
					undoChalNoValidation();
				}

				if (applied == 0) {
					return Evaluator::checkmateScore[currentPlayer];
				}
			}
			return evaluator.getScore();
		}

		MohraChal::StoreType &moves = movesList[currdepth];
		moves.clear();
		ashtapada.fillAllMoves(moves);

		stat.boardEvaluated += moves.size();

		board_score_type bestScore = Evaluator::lowest_score[currentPlayer];
		int applied = 0;
		for (const Chal &chal : moves) {
			evaluator.apply(chal);
			if (!applyChalNoValidation(chal)) {
				evaluator.undo(chal);
				continue;
			}
			applied++;
			board_score_type score = minmax(currdepth - 1, alpha, beta);

			undoChalNoValidation();
			evaluator.undo(chal);

			if (currentPlayer == player_t::second) {
				bestScore = std::max(bestScore, score);
				alpha = std::max(alpha, bestScore);
			}
			else {
				bestScore = std::min(bestScore, score);
				beta = std::min(beta, bestScore);
			}
			if (beta < alpha) break;
		}

		if (applied == 0) {
			return Evaluator::checkmateScore[currentPlayer];
		}

		return bestScore;
	}

	const Chal NitiAyogLRU::getNextMove(niti_depth_type depth) {
		if (depth == -1) depth = this->depth;
		auto startTime = std::chrono::high_resolution_clock::now();

		const player_t player = ashtapada.getCurrentPlayer();
		const MohraChal::StoreType &moves = getValidMoves();

		std::vector<Chal> bestChal;
		const int playerSign = player.getSign();

		stat.boardEvaluated = moves.size(); // Note we are initializing it here hence not +=
		stat.skipped = 0;

		board_score_type bestScore = board_score_type_min * playerSign;
		map.push_front(evaluator.getHash(), { bestScore, depth });
		for (const Chal &chal : moves) {
			evaluator.apply(chal);
			auto it = map.find_no_move(evaluator.getHash());
			board_score_type score;
			if (it != map.end() && (*it).second.depth >= depth - 2) {
				score = (*it).second.score;
				stat.skipped++;
			}
			else {
				if (!applyChalNoValidation(chal)) {
					score = board_score_type_min * playerSign;
					map.push_front(evaluator.getHash(), { score, (niti_depth_type)(depth + 10)});
					evaluator.undo(chal);
					continue;
				}
				score = minmax(depth - 1, board_score_type_min, board_score_type_max);
				undoChalNoValidation();
				map.push_front(evaluator.getHash(), { score, depth });
			}
			evaluator.undo(chal);
			if (score == bestScore) bestChal.push_back(chal);
			else {
				if (playerSign == 1) {
					if (score > bestScore) {
						bestChal.clear();
						bestChal.push_back(chal);
						bestScore = score;
					}
				}
				else {
					if (score < bestScore) {
						bestChal.clear();
						bestChal.push_back(chal);
						bestScore = score;

					}
				}
			}
		}

		auto seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
		Chal ret = bestChal[seed % bestChal.size()];
		//map.clear();
		auto endTime = std::chrono::high_resolution_clock::now();
		stat.elapsedTime = endTime - startTime;
		if (stat.elapsedTime > stat.elapsedTimeMax) stat.elapsedTimeMax = stat.elapsedTime;
		stat.boardEvaluatedMax = std::max(stat.boardEvaluatedMax, stat.boardEvaluated);
		stat.skippedMax = std::max(stat.skippedMax, stat.skipped);
		return ret;
	}

	int NitiAyogLRU::minmax(niti_depth_type currdepth, board_score_type alpha, board_score_type beta) {
		player_t currentPlayer = ashtapada.getCurrentPlayer();
		if (currdepth == 0) {
			if (ashtapada.isChecked() != player_t::none) {
				assert(ashtapada.isChecked() == currentPlayer);
				// Find checkmate
				MohraChal::StoreType &moves = movesList[currdepth];
				moves.clear();
				ashtapada.fillAllMoves(moves);
				int applied = 0;
				for (const Chal &chal : moves) {
					if (!applyChalNoValidation(chal)) {
						continue;
					}
					applied++;
					undoChalNoValidation();
				}

				if (applied == 0) {
					return Evaluator::checkmateScore[currentPlayer];
				}
			}
			return evaluator.getScore();
		}

		MohraChal::StoreType &moves = movesList[currdepth];
		moves.clear();
		ashtapada.fillAllMoves(moves);

		stat.boardEvaluated += moves.size();

		const int playerSign = ashtapada.getCurrentPlayer().getSign();
		board_score_type bestScore = Evaluator::lowest_score[currentPlayer];
		map.push_front(evaluator.getHash(), { bestScore, depth });
		for (const Chal &chal : moves) {
			evaluator.apply(chal);

			auto it = map.find_no_move(evaluator.getHash());
			board_score_type score;
			if (it != map.end() && (*it).second.depth >= currdepth - 2) {
				score = (*it).second.score;
				stat.skipped++;
			}
			else {
				if (!applyChalNoValidation(chal)) {
					score = board_score_type_min * playerSign;
					map.push_front(evaluator.getHash(), { score, currdepth });
					evaluator.undo(chal);
					continue;
				}
				score = minmax(currdepth - 1, alpha, beta);
				undoChalNoValidation();
				map.push_front(evaluator.getHash(), { score, currdepth });
			}
			evaluator.undo(chal);
			if (currentPlayer == player_t::second) {
				bestScore = std::max(bestScore, score);
				alpha = std::max(alpha, bestScore);
			}
			else {
				bestScore = std::min(bestScore, score);
				beta = std::min(beta, bestScore);
			}
			if (beta < alpha) break;
		}

		return bestScore;
	}

	std::ostream & operator<<(std::ostream & os, const NitiAyogStat & nitiAyogStat)
	{
		os << "Duration: " << nitiAyogStat.elapsedTime.count() << "s max: " << nitiAyogStat.elapsedTimeMax.count() << "s, Moves Evaluated: " << nitiAyogStat.boardEvaluated << " max: " << nitiAyogStat.boardEvaluatedMax;
		return os;
	}


	std::ostream & operator<<(std::ostream & os, const NitiAyogStatLRU & nitiAyogStat)
	{
		os << "Duration: " << nitiAyogStat.elapsedTime.count() << "s max: " << nitiAyogStat.elapsedTimeMax.count() <<
			"s, Moves Evaluated: " << nitiAyogStat.boardEvaluated << " max: " << nitiAyogStat.boardEvaluatedMax <<
			", Eval Skiped:" << nitiAyogStat.skipped << " max: " << nitiAyogStat.skippedMax;
		return os;
	}
}