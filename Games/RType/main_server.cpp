#include "Core/Server/GameServer.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    float scale = 1.0f;
    bool windowed = false;
    bool display = false;
    int maxLobbies = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-s" || arg == "--scale") && i + 1 < argc) {
            try {
                scale = std::stof(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Invalid scale value: " << argv[i] << std::endl;
                return 1;
            }
        } else if (arg == "-w" || arg == "--windowed") {
            windowed = true;
        } else if (arg == "-d" || arg == "--display") {
            display = true;
        } else if ((arg == "-m" || arg == "--multiInstance") && i + 1 < argc) {
            try {
                maxLobbies = std::stoi(argv[++i]);
                if (maxLobbies < 0) {
                    std::cerr << "Max lobbies must be non-negative: " << maxLobbies << std::endl;
                    return 1;
                }
            } catch (const std::exception&) {
                std::cerr << "Invalid max lobbies value: " << argv[i] << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Usage: " << argv[0] << " [-s|--scale SCALE] [-w|--windowed] [-d|--display] [-m|--multiInstance MAX_LOBBIES]" << std::endl;
            return 1;
        }
    }

    GameServer server(display, windowed, scale, maxLobbies);
    if (!server.init())
        return 1;
    server.run();
    server.shutdown();
    return 0;
}
