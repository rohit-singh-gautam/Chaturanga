// Chaturanga.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <commandline.h>
#include <Ashtapada.h>
#include <NitiAyog.h>
#include <Evaluator.h>
#include <version.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>
#include <memory>

using namespace rohit;

void DisplaySizes() {
	std::cout << "row_t:" << sizeof(row_t) << std::endl;
	std::cout << "col_t:" << sizeof(col_t) << std::endl;
	std::cout << "Position:" << sizeof(Position) << std::endl;
	std::cout << "DiffPosition:" << sizeof(DiffPosition) << std::endl;
	std::cout << "player_t:" << sizeof(player_t) << std::endl;
	std::cout << "mohra_t:" << sizeof(mohra_t) << std::endl;
	std::cout << "bitmohra_t:" << sizeof(bitmohra_t) << std::endl;
	std::cout << "Piece:" << sizeof(piece_t) << std::endl;
	std::cout << "PieceEntry:" << sizeof(PieceEntry) << std::endl;
	std::cout << "PieceList:" << sizeof(PieceList) << std::endl;
	std::cout << "Pieces:" << sizeof(Pieces) << std::endl;
	std::cout << "PadatiPromotion:" << sizeof(PadatiPromotion) << std::endl;
	std::cout << "Chal:" << sizeof(Chal) << std::endl;
	std::cout << "MohraChal:" << sizeof(MohraChal) << std::endl;
	std::cout << "RajaChal:" << sizeof(RajaChal) << std::endl;
	std::cout << "SenapatiChal:" << sizeof(SenapatiChal) << std::endl;
	std::cout << "RathaChal:" << sizeof(RathaChal) << std::endl;
	std::cout << "GajaChal:" << sizeof(GajaChal) << std::endl;
	std::cout << "AshvaChal:" << sizeof(AshvaChal) << std::endl;
	std::cout << "PadatiChal:" << sizeof(PadatiChal) << std::endl;
	std::cout << "ChalFactory:" << sizeof(ChalFactory) << std::endl;
	std::cout << "Ashtapada:" << sizeof(Ashtapada) << std::endl;
	std::cout << "NitiAyogLRU:" << sizeof(NitiAyogLRU) << std::endl;
	std::cout << "Niti:" << sizeof(Niti) << std::endl;
	std::cout << "Evaluator:" << sizeof(Evaluator) << std::endl;
	std::cout << "NitiAyog:" << sizeof(NitiAyog) << std::endl;
}

std::string board[] = {
	"..g....c"
	".ap..r.."
	"p..gp..p"
	"a..p.Gp."
	".c.P...P"
	"GPA.PCPp"
	"P...RP.C"
	".A......",

	"..g....."
	".ap..r.c"
	"p..gp..p"
	"a..p.Gp."
	".c.P...P"
	"GPA.PCPp"
	"P...RP.C"
	".A......"

};

void TestAshtapada(const niti_depth_type maxdepth, const niti_depth_type player1depth, const niti_depth_type player2depth) {
	const int depth[] { player1depth, player2depth };
	std::vector<Chal> moves;

	std::cout.imbue(std::locale(""));

	auto pAshtapada { std::make_unique<Ashtapada>() };
	//pAshtapada->apply(board[1], player_t::first);
	pAshtapada->apply(Ashtapada::DEFAULT, player_t::first);
	Evaluator evaluator(*pAshtapada);
	evaluator.refresh();
	NitiAyog nitiAyog(*pAshtapada, evaluator, maxdepth);

	pAshtapada->display_flag = Ashtapada::display_board | Ashtapada::display_captured | Ashtapada::display_summary;
	std::cout << *pAshtapada << std::endl;

	while (pAshtapada->isCheckmate() == player_t::none && pAshtapada->getMoveCount() < Ashtapada::max_chal - maxdepth) {
		int playerid = pAshtapada->getCurrentPlayer();
		const Chal chal = nitiAyog.getNextMove(depth[playerid]);
		bool moved = pAshtapada->applyChal(chal.from, chal.to);
		assert(moved);
		std::cout << *pAshtapada;
		std::cout << "stat:: " << nitiAyog.getStat() << std::endl;
		std::cout << "-------------------------------" << std::endl << std::endl;
	}
	std::cout << "Checkmate: " << pAshtapada->isCheckmate().to_string() << " player" << std::endl;
}

int main(int argc, char *argv[])
{
	bool display_version { false };
	uint16_t maxdepth { 9 };
    uint16_t player1depth { 3 };
    uint16_t player2depth { 3 };
    rohit::commandline param_parser(
        "Chaturanga",
        "Chaturanga two player board game",
        {
            {'v', "version", "Display version", display_version},
			{'m', "maxdepth", "Max depth", "Maximum depth to which seach will be performed", maxdepth},
            {'f', "player1", "Player1 Depth", "Player1 or first search depth", player1depth},
            {'s', "player2", "Player2 Depth", "Player2 or second search depth", player2depth}
        }
    );

    if (!param_parser.parser(argc, argv)) {
        std::cout << param_parser.usage() << std::endl;
        return EXIT_SUCCESS;
    }

    if (display_version) {
        std::cout << param_parser.get_name() << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
		return EXIT_SUCCESS;
    }
	TestAshtapada(maxdepth, player1depth, player2depth);
	return 0;
}
