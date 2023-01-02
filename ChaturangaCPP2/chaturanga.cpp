#include <version.h>
#include <commandline.h>
#include <iostream>
#include <core.h>
#include <ashtapada.h>

int main(int const argc, char const * const * const argv) {
    bool display_version { false };
    bool player1auto { false };
    bool player2auto { false };
    rohit::commandline param_parser(
        "Chaturanga",
        "Chaturanga two player board game",
        {
            {'v', "version", "Display version", display_version},
            {'f', "player1", "Player1 or first will be played by computer", player1auto},
            {'s', "player2", "Player2 or second will be played by computer", player2auto}
        }
    );

    if (!param_parser.parser(argc, argv)) {
        std::cout << param_parser.usage() << std::endl;
        return EXIT_SUCCESS;
    }

    if (display_version) {
        std::cout << param_parser.get_name() << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
    }

}