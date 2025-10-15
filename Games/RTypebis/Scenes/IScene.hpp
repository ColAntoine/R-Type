#pragma once

class IScene {
    public:
        virtual ~IScene() = default;
        virtual void load() = 0;
        virtual void unload() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
}