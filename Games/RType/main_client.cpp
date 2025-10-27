#include "Core/Client/GameClient.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    float scale = 1.0f;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-s" || arg == "--scale") && i + 1 < argc) {
            try {
                scale = std::stof(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Invalid scale value: " << argv[i] << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    GameClient client(scale);
    if (!client.init())
        return 1;
    client.run();
    client.shutdown();
    return 0;
}
