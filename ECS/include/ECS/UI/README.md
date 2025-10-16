# UI Components System

This directory contains the UI component architecture for the ECS library. The UI system provides a generic, reusable way to create and manage user interface elements within the ECS framework.

## Architecture Overview

The UI system follows a three-layer architecture:

```
IUIComponent (Interface)
    ↓
AUIComponent (Abstract base with common implementation)
    ↓
UIButton, UIInputField, UIPanel, etc. (Concrete UI elements)
    ↓
UIComponent (ECS wrapper that holds shared_ptr to IUIComponent)
    ↓
Registry (Stores UIComponent like any other ECS component)
```

## Core Components

### 1. IUIComponent (Interface)
Located in `IUIComponent.hpp`

Defines the contract that all UI elements must follow:
- `update(float delta_time)` - Update logic
- `render()` - Rendering logic
- `handle_input()` - Input handling
- Position and size management
- Visibility and enabled state
- State management (Normal, Hovered, Pressed, Disabled)

### 2. AUIComponent (Abstract Base Class)
Located in `AUIComponent.hpp`

Provides common functionality for all UI components:
- Default implementations of position, size, and state management
- `update_state()` helper method for mouse interaction
- Protected member variables for derived classes

### 3. Concrete Components
Located in `Components/` directory

#### UIButton
A generic button component with:
- Customizable style (colors, font size, border)
- Callback support (onClick, onHover, onHoverExit, onPress, onRelease)
- Custom rendering support
- Virtual methods for style overriding

Example styled buttons in games can inherit from `UIButton` to add specific effects (e.g., neon glow, glitch effects).

### 4. UIComponent (ECS Wrapper)
Located in `include/ECS/Components/UIComponent.hpp`

Wraps any `IUIComponent` implementation to integrate with the ECS Registry:
```cpp
class UIComponent : public IComponent {
public:
    std::shared_ptr<UI::IUIComponent> ui_element;

    UIComponent() = default;
    explicit UIComponent(std::shared_ptr<UI::IUIComponent> element);

    template<typename T, typename... Args>
    static UIComponent create(Args&&... args);
};
```

## UI System

Located in `include/ECS/Systems/UISystem.hpp` and `src/systems/UISystem.cpp`

The UISystem manages all UI components in the ECS:
- **Input Processing**: Handles mouse and keyboard input for all UI elements
- **Update Logic**: Updates all UI components each frame
- **Rendering**: Renders all visible UI components
- **Focus Management**: Tracks which UI element has focus (for keyboard input)
- **Uses Registry**: Queries UI components using the registry (no separate container)

Key methods:
```cpp
void update(registry& registry, float deltaTime);
void process_input(registry& registry);
void render(registry& registry);
void set_focused_entity(size_t entity_id);
void clear_focus();
```

## How to Integrate UI Components

### Step 1: Register UIComponent in your Registry

```cpp
#include "ECS/Registry.hpp"
#include "ECS/Components/UIComponent.hpp"

registry reg;
reg.register_component<UI::UIComponent>();
```

### Step 2: Create a UI System Instance

```cpp
#include "ECS/Systems/UISystem.hpp"

UI::UISystem ui_system;
```

### Step 3: Create UI Elements and Add to Registry

```cpp
#include "ECS/UI/Components/Button.hpp"

// Spawn an entity
auto button_entity = reg.spawn_entity();

// Create a button
auto play_button = std::make_shared<UI::UIButton>(100, 100, 200, 60, "Play Game");

// Customize the button style
UI::ButtonStyle style;
style.normal_color = {70, 70, 70, 255};
style.hovered_color = {100, 100, 255, 255};
style.pressed_color = {50, 50, 150, 255};
style.font_size = 24;
play_button->set_style(style);

// Set callbacks
play_button->set_on_click([]() {
    std::cout << "Button clicked!" << std::endl;
});

play_button->set_on_hover([]() {
    std::cout << "Hovering!" << std::endl;
});

// Add to registry
reg.add_component(button_entity, UI::UIComponent(play_button));
```

### Step 4: Update and Render in Game Loop

```cpp
while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    // Process input first
    ui_system.process_input(reg);

    // Update
    ui_system.update(reg, deltaTime);

    // Render
    BeginDrawing();
    ClearBackground(BLACK);

    // ... render game elements ...

    ui_system.render(reg);

    EndDrawing();
}
```

## Creating Custom UI Components

### Option 1: Inherit from UIButton (Recommended for styled buttons)

```cpp
class NeonButton : public UI::UIButton {
public:
    NeonButton(float x, float y, float w, float h, const std::string& text)
        : UIButton(x, y, w, h, text) {}

protected:
    void draw_button_background() const override {
        // Custom neon glow rendering
        UIButton::draw_button_background();
        // Add neon effects...
    }

    void draw_button_text() const override {
        // Custom text with glitch effect
        // ...
    }
};
```

### Option 2: Inherit from AUIComponent (For completely new UI elements)

```cpp
class UIInputField : public UI::AUIComponent {
public:
    void update(float delta_time) override {
        if (!visible_) return;
        update_state();
        // Custom input field update logic
    }

    void render() override {
        if (!visible_) return;
        // Render input field
    }

    void handle_input() override {
        // Handle text input
    }

private:
    std::string text_;
    bool is_focused_;
};
```

## Building the UI System

The UI system is automatically built when you build the ECS library (requires raylib).

### Prerequisites
- **CMake** (version 3.10 or higher)
- **C++ compiler** with C++17 support (g++, clang++)
- **raylib** library installed on your system
- **Make** build tool

### Build Steps

1. **Navigate to the ECS directory:**
```bash
cd /path/to/RTYPE/ECS
```

2. **Create and enter the build directory:**
```bash
mkdir -p build
cd build
```

3. **Generate build files with CMake:**
```bash
cmake ..
```

CMake will:
- Detect if raylib is installed
- Configure the build for all components and systems
- Generate the Makefile

Expected output:
```
-- raylib found. UI components will be built.
--   - lib/ui/libui_components.so: UI components library
--   - lib/systems/libui_system.so: UI management system
--   - ui_test: UI system test executable
-- Configuring done
-- Generating done
```

4. **Build the project:**
```bash
make -j4
```

The `-j4` flag enables parallel compilation using 4 cores (adjust based on your CPU).

### Build Output

After a successful build, you'll have:
- `libECS.so` - Core ECS library (components, registry, dlloader)
- `lib/systems/libposition_system.so` - Position update system
- `lib/systems/libcollision_system.so` - Collision detection system
- `lib/systems/libsprite_system.so` - Sprite rendering system
- `lib/systems/libanimation_system.so` - Animation system
- `lib/ui/libui_components.so` - UI components library
- `lib/systems/libui_system.so` - UI system library
- `ui_test` - UI system test executable

### Building Only the UI Test

If you've already built the project and only want to rebuild the test:

```bash
cd /path/to/RTYPE/ECS/build
make ui_test
```

### Clean Build

To rebuild everything from scratch:

```bash
cd /path/to/RTYPE/ECS/build
make clean
cmake ..
make -j4
```

### Troubleshooting Build Issues

**If raylib is not found:**
```bash
# On Arch Linux
sudo pacman -S raylib

# On Ubuntu/Debian
sudo apt-get install libraylib-dev

# On macOS with Homebrew
brew install raylib
```

**If build fails with include errors:**
- Ensure you're building from the `build/` directory
- Check that all header files exist in `include/ECS/`
- Verify CMake completed successfully without errors

**If linking fails:**
- Make sure raylib is properly installed
- Check that all required libraries are in your system path

## Running the Test

A test program is provided to demonstrate the UI system:

```bash
cd /path/to/ECS/build
./ui_test
```

The test creates a window with two buttons:
- **Play Game** button - Logs click and hover events
- **Quit** button - Logs click events

### Expected Output:
```
INFO: Initializing raylib 5.5
...
Hovering over play button
Play button clicked!
Stopped hovering
Quit button clicked!
```

## Dependencies

- **raylib** - Required for rendering and input handling
- **C++17** - Standard requirement

## File Structure

```
include/ECS/UI/
├── README.md                      (this file)
├── IUIComponent.hpp               (UI interface)
├── AUIComponent.hpp               (Abstract base class)
└── Components/
    ├── Button.hpp                 (Generic button)
    └── (future: InputField.hpp, Panel.hpp, Text.hpp)

src/UI/Components/
└── Button.cpp                     (Button implementation)

include/ECS/Components/
└── UIComponent.hpp                (ECS wrapper)

include/ECS/Systems/
└── UISystem.hpp                   (UI system interface)

src/systems/
└── UISystem.cpp                   (UI system implementation)

test/
└── ui_test.cpp                    (Test program)
```

## Best Practices

1. **Keep UI generic in ECS**: The ECS layer should provide generic, reusable components
2. **Add styling in game layer**: Game-specific effects (neon, glitch, etc.) should be in game-specific classes
3. **Use callbacks for interaction**: Leverage `std::function` callbacks for flexibility
4. **Focus management**: Use the UISystem's focus management for keyboard input
5. **Z-index for layering**: Set z-index values to control rendering order (future enhancement)

## Future Enhancements

- [ ] Add z-index sorting for proper layering
- [ ] Implement UIPanel component
- [ ] Implement UIInputField component
- [ ] Implement UIText component
- [ ] Add animation support for UI transitions
- [ ] Add drag-and-drop support
- [ ] Add UI layout managers (grid, flex, etc.)

## Troubleshooting

### Build Errors
- **"raylib not found"**: Install raylib or ensure it's in your CMake path
- **Include errors**: Make sure you're using correct relative paths from the `include/` directory

### Runtime Issues
- **Buttons not responding**: Ensure `process_input()` is called before `update()`
- **Nothing rendering**: Check that UI components are visible and render() is called
- **Callbacks not firing**: Verify callbacks are set before adding component to registry

## Contributing

When adding new UI components:
1. Create interface/implementation in `UI/Components/`
2. Follow the IUIComponent interface
3. Add implementation to `src/UI/Components/`
4. Update CMakeLists.txt to include new source files
5. Add tests in `test/` directory
6. Update this README
