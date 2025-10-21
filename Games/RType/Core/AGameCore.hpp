#pragma once

#include "IGameCore.hpp"
#include "ECS/RegisterAllComponents.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/LifeTime/LifeTime.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/RemotePlayer/RemotePlayer.hpp"
#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Spawner/Spawner.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"

class AGameCore : public IGameCore {
    public:
        AGameCore();
        virtual ~AGameCore();

        static void RegisterComponents(registry &r);

        // Default lifecycle implementations forward to overridable hooks
        bool init() override;
        void run() override;
        void update(float) override;
        void shutdown() override;

    protected:
        // Hooks for derived classes
        virtual bool onInit() { return true; }
        virtual void onRun() {}
        virtual void onUpdate(float) {}
        virtual void onShutdown() {}

        bool _running{false};
};
