#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Network/NetworkManager.hpp"

#ifdef _WIN32
  #include "ECS/WinLoader.hpp"
  using PlatformLoader = WinLoader;
#else
  #include "ECS/LinuxLoader.hpp"
  using PlatformLoader = LinuxLoader;
#endif

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif

#include <raylib.h>

class GameClient : public AGameCore {
    public:
        GameClient(float scale = 1.0f, bool windowed = false);
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;

        registry &GetRegistry();
        ILoader &GetILoader();

    private:
        registry ecs_registry_;
        PlatformLoader ecs_loader_;
        std::unique_ptr<NetworkManager> network_manager_;

        GameStateManager _stateManager;
        float _scale;
        bool _windowed;
        void register_states();
        void set_bindings();
};
