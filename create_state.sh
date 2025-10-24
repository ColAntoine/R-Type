#!/bin/bash
# filepath: /home/gab/delivery/tek3/R-Type/create_state.sh

# Vérifier les arguments
if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo "Usage: $0 <StateName> [--unblock]"
    echo "Example: $0 Settings"
    echo "Example: $0 Settings --unblock"
    exit 1
fi

STATE_NAME=$1
UNBLOCK_FLAG=${2:-""}
STATE_NAME_LOWER=$(echo $1 | tr '[:upper:]' '[:lower:]')
STATE_NAME_CLASS="${STATE_NAME}State"
STATE_DIR="Games/RType/Core/States/${STATE_NAME}"

# Créer le dossier
mkdir -p "$STATE_DIR"
echo "✓ Dossier créé: $STATE_DIR"

# Créer le fichier .hpp
if [ "$UNBLOCK_FLAG" == "--unblock" ]; then
    cat > "$STATE_DIR/${STATE_NAME}.hpp" << 'EOF'
#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class STATE_NAME_CLASS : public AGameState {
    public:
        STATE_NAME_CLASS() = default;
        ~STATE_NAME_CLASS() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;
        void handle_input() override;

        void setup_ui() override;
        void render() override;
        void update(float delta_time) override;

        std::string get_name() const override { return "STATE_NAME_LOWER"; }
        
        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }
    private:
        // STATE_NAME_CLASS specific members
};
EOF
else
    cat > "$STATE_DIR/${STATE_NAME}.hpp" << 'EOF'
#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class STATE_NAME_CLASS : public AGameState {
    public:
        STATE_NAME_CLASS() = default;
        ~STATE_NAME_CLASS() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;
        void handle_input() override;

        void setup_ui() override;
        void render() override;
        void update(float delta_time) override;

        std::string get_name() const override { return "STATE_NAME_LOWER"; }
    private:
        // STATE_NAME_CLASS specific members
};
EOF
fi

# Remplacer les placeholders dans le .hpp
sed -i "s/STATE_NAME_CLASS/${STATE_NAME_CLASS}/g" "$STATE_DIR/${STATE_NAME}.hpp"
sed -i "s/STATE_NAME_LOWER/${STATE_NAME_LOWER}/g" "$STATE_DIR/${STATE_NAME}.hpp"

echo "✓ Fichier créé: $STATE_DIR/${STATE_NAME}.hpp"

# Créer le fichier .cpp
cat > "$STATE_DIR/${STATE_NAME}.cpp" << 'EOF'
// filepath: Games/RType/Core/States/STATE_NAME/STATE_NAME.cpp
#include "STATE_NAME.hpp"
#include <iostream>

void STATE_NAME_CLASS::enter()
{
    std::cout << "[STATE_NAME_CLASS] Entering state" << std::endl;
    
    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);
    
    setup_ui();
    _initialized = true;
}

void STATE_NAME_CLASS::exit()
{
    std::cout << "[STATE_NAME_CLASS] Exiting state" << std::endl;
    _initialized = false;
}

void STATE_NAME_CLASS::pause()
{
    std::cout << "[STATE_NAME_CLASS] Pausing state" << std::endl;
}

void STATE_NAME_CLASS::resume()
{
    std::cout << "[STATE_NAME_CLASS] Resuming state" << std::endl;
}

void STATE_NAME_CLASS::handle_input()
{
    if (!_initialized)
        return;
    
    // Handle input logic here
}

void STATE_NAME_CLASS::setup_ui()
{
    std::cout << "[STATE_NAME_CLASS] Setting up UI" << std::endl;
    
    // Setup UI components here
}

void STATE_NAME_CLASS::render()
{
    if (!_initialized)
        return;
    
    this->_systemLoader.update_all_systems(this->_registry, 0.0f, DLLoader::RenderSystem);
}

void STATE_NAME_CLASS::update(float delta_time)
{
    if (!_initialized)
        return;
    
    // Update logic here
}
EOF

# Remplacer les placeholders dans le .cpp
sed -i "s/STATE_NAME_CLASS/${STATE_NAME_CLASS}/g" "$STATE_DIR/${STATE_NAME}.cpp"
sed -i "s/STATE_NAME/${STATE_NAME}/g" "$STATE_DIR/${STATE_NAME}.cpp"

echo "✓ Fichier créé: $STATE_DIR/${STATE_NAME}.cpp"
echo ""
echo "✅ État créé avec succès!"
echo "   Classe: ${STATE_NAME_CLASS}"
echo "   Dossier: $STATE_DIR"
if [ "$UNBLOCK_FLAG" == "--unblock" ]; then
    echo "   Mode: UNBLOCKED (blocks_update/render = false)"
else
    echo "   Mode: BLOCKED (hérite de AGameState)"
fi
echo ""
echo "Ajoute cette ligne à ton CMakeLists.txt:"
echo "   set(GAME_STATES_SOURCES \${GAME_STATES_SOURCES} Games/RType/Core/States/${STATE_NAME}/${STATE_NAME}.cpp)"