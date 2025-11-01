#include "Connection.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "ECS/Zipper.hpp"

void Connection::enter()
{
    std::cout << "[Connection] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    _systemLoader->load_components(ecsLib, _registry);
    _systemLoader->load_system(uiSys, ILoader::RenderSystem);
    
    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void Connection::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    _initialized = false;
}

void Connection::pause()
{
    std::cout << "[Connection] Pausing state" << std::endl;
}

void Connection::resume()
{
    std::cout << "[Connection] Resuming state" << std::endl;
}

void Connection::setup_ui()
{
    std::cout << "[Connection] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Create bar to split the panel
    auto bar = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(20, renderManager.scaleSizeH(90))
        .border(20, WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto barEntity = _registry.spawn_entity();
    _registry.add_component(barEntity, UI::UIComponent(bar));

    // Left side: Input fields for IP and Port
    // IP Label
    auto ipLabel = TextBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(13))
        .text("IP Address:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto ipLabelEntity = _registry.spawn_entity();
    _registry.add_component(ipLabelEntity, UI::UIComponent(ipLabel));

    // IP Input Field
    auto ipInput = InputBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter IP address")
        .backgroundColor(Color{50, 50, 50, 255})
        .text("127.0.0.1")
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto ipInputEntity = _registry.spawn_entity();
    _registry.add_component(ipInputEntity, UI::UIComponent(ipInput));
    // register and tag this entity for quick access
    _registry.register_component<Connection::IpFieldTag>();
    _registry.add_component<Connection::IpFieldTag>(ipInputEntity, Connection::IpFieldTag());

    // Port Label
    auto portLabel = TextBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(28))
        .text("Port:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto portLabelEntity = _registry.spawn_entity();
    _registry.add_component(portLabelEntity, UI::UIComponent(portLabel));

    // Port Input Field
    auto portInput = InputBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(33))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter port")
        .text("8080")
        .backgroundColor(Color{50, 50, 50, 255})
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto portInputEntity = _registry.spawn_entity();
    _registry.add_component(portInputEntity, UI::UIComponent(portInput));
    _registry.register_component<Connection::PortFieldTag>();
    _registry.add_component<Connection::PortFieldTag>(portInputEntity, Connection::PortFieldTag());

    // Back Button (Red)
    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(63), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("BACK")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            // ? THIS IS PRINTED 2 times ?
            std::cout << "++++++++TEST++++++++++" << std::endl;
            if (this->_stateManager) {
                this->_stateManager->pop_state();
                this->_stateManager->pop_state();
                this->_stateManager->push_state("MainMenu");
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));

    // Connection Button (Green)
    auto connectButton = ButtonBuilder()
        .at(renderManager.scalePosX(79), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("CONNECT")
        .green()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            this->connection_callback();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto connectButtonEntity = _registry.spawn_entity();
    _registry.add_component(connectButtonEntity, UI::UIComponent(connectButton));

    // right side: Input fields for IP and Port
    // IP Label
    auto nameLabel = TextBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(13))
        .text("Name:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto nameLabelEntity = _registry.spawn_entity();
    _registry.add_component(nameLabelEntity, UI::UIComponent(nameLabel));

    // IP Input Field
    auto nameInput = InputBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter your name")
        .backgroundColor(Color{50, 50, 50, 255})
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto nameInputEntity = _registry.spawn_entity();
    _registry.add_component(nameInputEntity, UI::UIComponent(nameInput));
    _registry.register_component<Connection::NameFieldTag>();
    _registry.add_component<Connection::NameFieldTag>(nameInputEntity, Connection::NameFieldTag());
}

std::shared_ptr<UI::UIInputField> Connection::GetIpInput() {
            auto* ui_comps = _registry.get_if<UI::UIComponent>();
            auto* tag = _registry.get_if<IpFieldTag>();
            if (!ui_comps || !tag) return nullptr;
            for (auto [ui_comp, t, ent] : zipper(*ui_comps, *tag)) {
                return std::dynamic_pointer_cast<UI::UIInputField>(ui_comp._ui_element);
            }
            return nullptr;
        }

std::shared_ptr<UI::UIInputField> Connection::GetPortInput() {
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    auto* tag = _registry.get_if<PortFieldTag>();
    if (!ui_comps || !tag) return nullptr;
    for (auto [ui_comp, t, ent] : zipper(*ui_comps, *tag)) {
        return std::dynamic_pointer_cast<UI::UIInputField>(ui_comp._ui_element);
    }
    return nullptr;
}

std::shared_ptr<UI::UIInputField> Connection::GetNameInput() {
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    auto* tag = _registry.get_if<NameFieldTag>();
    if (!ui_comps || !tag) return nullptr;
    for (auto [ui_comp, t, ent] : zipper(*ui_comps, *tag)) {
        return std::dynamic_pointer_cast<UI::UIInputField>(ui_comp._ui_element);
    }
    return nullptr;
}

void Connection::connection_callback()
{
    std::cout << "[Connection] Connect button clicked" << std::endl;
    if (connecting_) {
        std::cout << "[Connection] Connection already in progress, ignoring duplicate click." << std::endl;
        return;
    }
    connecting_ = true;
    auto client = RType::Network::get_client();
    if (!client) {
        std::cout << "[Lobby] No network client available" << std::endl;
        connecting_ = false;
        return;
    }
    auto ip_input = GetIpInput();
    auto port_input = GetPortInput();
    auto name_input = GetNameInput();

    if (ip_input) { ServerIp_ = ip_input->getText(); }
    if (port_input) { ServerPort_ = port_input->getText(); }
    if (name_input) { PlayerName_ = name_input->getText(); }

    uint16_t port = 0;
    try {
        int p = std::stoi(ServerPort_);
        if (p <= 0 || p > 65535) throw std::out_of_range("port");
        port = static_cast<uint16_t>(p);
    } catch (...) {
        std::cout << "[Connection] Invalid port: " << ServerPort_ << std::endl;
        connecting_ = false;
        return;
    }

    auto accept = client->connect(ServerIp_, port, PlayerName_, 1, 2000);
    if (accept) {
        std::cout << "[Connection] Server accepted connection. Assigned player_id=" << accept->player_id << " session_token=" << accept->session_id << " multi_instance=" << (accept->multi_instance ? "true" : "false") << std::endl;
        if (_stateManager) {
            _stateManager->pop_state();
            bool is_multi_instance = (accept->multi_instance != 0);
            if (is_multi_instance) {
                _stateManager->push_state("Browser");
            } else {
                _stateManager->push_state("Lobby");
            }
        }
    } else {
        std::cout << "[Connection] Failed to connect to server." << std::endl;
        // allow retry on failure
        connecting_ = false;
    }
}