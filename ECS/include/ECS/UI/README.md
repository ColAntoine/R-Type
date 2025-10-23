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

#### UIInputField
A text input field component with:
- Placeholder text support
- Max length validation
- Password mode (text masking)
- Blinking cursor animation
- Focus management
- Callbacks (onTextChanged, onEnterPressed, onFocus, onFocusLost)
- Customizable style (colors, font, border)

#### UIPanel
A container/background panel component with:
- Rounded corners support
- Border rendering
- Drop shadow effect
- Semi-transparent background
- Customizable style (background, border, shadow, corner radius)

#### UIText
A text display component with:
- Text alignment (Left, Center, Right)
- Drop shadow support
- Customizable style (color, font size, shadow)
- Automatic text positioning based on alignment

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

#### Example: Creating a Button

```cpp
#include "ECS/UI/Components/Button.hpp"

// Spawn an entity
auto button_entity = reg.spawn_entity();

// Create a button
auto play_button = std::make_shared<UI::UIButton>(100, 100, 200, 60, "Play Game");

// Customize the button style
UI::ButtonStyle style;
style._normal_color = {70, 70, 70, 255};
style._hovered_color = {100, 100, 255, 255};
style._pressed_color = {50, 50, 150, 255};
style._font_size = 24;
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

#### Example: Creating a Panel

```cpp
#include "ECS/UI/Components/Panel.hpp"

auto panel_entity = reg.spawn_entity();
auto background_panel = std::make_shared<UI::UIPanel>(200, 120, 400, 420);

UI::PanelStyle panel_style;
panel_style._background_color = {30, 30, 40, 220};  // Semi-transparent
panel_style._border_color = {0, 229, 255, 255};     // Cyan border
panel_style._border_thickness = 2.0f;
panel_style._corner_radius = 10.0f;                 // Rounded corners
panel_style._has_shadow = true;
panel_style._shadow_color = {0, 0, 0, 150};
panel_style._shadow_offset = 8.0f;
background_panel->set_style(panel_style);

// Register panel FIRST for proper z-order (behind other elements)
reg.add_component(panel_entity, UI::UIComponent(background_panel));
```

#### Example: Creating an Input Field

```cpp
#include "ECS/UI/Components/InputField.hpp"

auto input_entity = reg.spawn_entity();
auto username_input = std::make_shared<UI::UIInputField>(
    250, 220, 300, 50, "Enter username..."
);

UI::InputFieldStyle input_style;
input_style._background_color = {50, 50, 50, 255};
input_style._focused_color = {70, 70, 90, 255};
input_style._border_color = {100, 100, 100, 255};
input_style._focused_border_color = {0, 229, 255, 255};
input_style._text_color = {255, 255, 255, 255};
input_style._placeholder_color = {150, 150, 150, 255};
input_style._cursor_color = {0, 229, 255, 255};
input_style._font_size = 20;
input_style._border_thickness = 2.0f;
username_input->set_style(input_style);

username_input->set_max_length(20);

username_input->set_on_text_changed([](const std::string& text) {
    std::cout << "Text: " << text << std::endl;
});

username_input->set_on_enter_pressed([](const std::string& text) {
    std::cout << "Submitted: " << text << std::endl;
});

reg.add_component(input_entity, UI::UIComponent(username_input));
```

#### Example: Creating Text

```cpp
#include "ECS/UI/Components/Text.hpp"

auto text_entity = reg.spawn_entity();
auto title_text = std::make_shared<UI::UIText>(400, 140, "GAME TITLE");

UI::TextStyle text_style;
text_style._text_color = {0, 229, 255, 255};       // Cyan
text_style._font_size = 36;
text_style._alignment = UI::TextAlignment::Center;
text_style._has_shadow = true;
text_style._shadow_color = {0, 0, 0, 200};
text_style._shadow_offset = {3.0f, 3.0f};
title_text->set_style(text_style);

reg.add_component(text_entity, UI::UIComponent(title_text));
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
    void drawButtonBackground() const override {
        // Custom neon glow rendering
        UIButton::drawButtonBackground();
        // Add neon effects...
    }

    void drawButtonText() const override {
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

The test creates a window with:
- **Background Panel** - Semi-transparent panel with cyan border and shadow
- **Title Text** - "UI SYSTEM TEST" with shadow effect
- **Username Label** - "Username:" text label
- **Input Field** - Text input with placeholder, max length, and callbacks
- **Play Game Button** - Glitch effect button with cyan neon glow
- **Quit Button** - Glitch effect button with red neon glow
- **Footer Text** - Instructions for user interaction

### Expected Output:
```
INFO: Initializing raylib 5.5
...
Input field focused
Text changed: t
Text changed: te
Text changed: tes
Text changed: test
Enter pressed! Username: test
Input field lost focus
Hovering over play button - GLITCH ACTIVE
Play button clicked!
Stopped hovering
Hovering over quit - WARNING GLITCH
Quit button clicked! Closing window...
```

### Test Features:
- Custom `GlitchButton` class demonstrating how to extend UIButton with game-specific effects
- Component registration order demonstrates z-order rendering (panel rendered first)
- All UI components working together in a cohesive interface
- Callbacks logging to console for debugging and verification

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
    ├── InputField.hpp             (Text input field)
    ├── Panel.hpp                  (Container/background panel)
    └── Text.hpp                   (Text display)

src/UI/Components/
├── Button.cpp                     (Button implementation)
├── InputField.cpp                 (Input field implementation)
├── Panel.cpp                      (Panel implementation)
└── Text.cpp                       (Text implementation)

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
2. **Add styling in game layer**: Game-specific effects (neon, glitch, etc.) should be in game-specific classes that inherit from base UI components
3. **Use callbacks for interaction**: Leverage `std::function` callbacks for flexibility and decoupling
4. **Focus management**: Use the UISystem's focus management for keyboard input (important for text fields)
5. **Component registration order**: Register background panels before interactive elements to ensure proper z-order rendering
6. **Smart pointers**: Use `std::shared_ptr` for UI components to enable safe sharing and polymorphism
7. **Style customization**: Set styles after component creation but before adding to registry
8. **Input processing order**: Always call `process_input()` before `update()` in your game loop

## Completed Features

- [x] IUIComponent interface with state management
- [x] AUIComponent abstract base class
- [x] UIButton component with custom styling
- [x] UIInputField component with text input and callbacks
- [x] UIPanel component with shadow and rounded corners
- [x] UIText component with alignment options
- [x] UISystem for managing all UI components via Registry
- [x] Zipper pattern integration for efficient component iteration
- [x] Focus management for keyboard input
- [x] Component registration order for z-order rendering
- [x] Test executable demonstrating all components

## Future Enhancements

- [ ] Add explicit z-index sorting for more flexible layering control
- [ ] Implement UICheckbox component
- [ ] Implement UISlider component
- [ ] Implement UIDropdown component
- [ ] Implement UIScrollbar component
- [ ] Add animation support for UI transitions (fade in/out, slide, etc.)
- [ ] Add drag-and-drop support for interactive elements
- [ ] Add UI layout managers (grid, flex, anchor-based positioning)
- [ ] Add UI theming system for consistent styling
- [ ] Add tooltip support
- [ ] Add multi-line text support for UIText
- [ ] Add clipping/scrolling for panels with overflow content

## Troubleshooting

### Build Errors
- **"raylib not found"**: Install raylib or ensure it's in your CMake path
- **Include errors**: Make sure you're using correct relative paths from the `include/` directory

### Runtime Issues
- **Buttons not responding**: Ensure `process_input()` is called before `update()`
- **Nothing rendering**: Check that UI components are visible and render() is called
- **Callbacks not firing**: Verify callbacks are set before adding component to registry

## Complete Working Example

For a complete working example demonstrating all UI components working together, see `test/ui_test.cpp`. This example includes:

- **Custom GlitchButton class** extending UIButton with cyberpunk-style glitch effects
- **Background panel** with shadow and rounded corners
- **Title and label text** with different alignments and styles
- **Text input field** with placeholder, max length, and callbacks
- **Multiple buttons** with custom styling and callbacks
- **Proper z-order rendering** by registering components in the correct order

The test demonstrates best practices for:
- Creating custom UI components by inheriting from base classes
- Keeping game-specific effects (glitch, neon glow) in the game layer, not in ECS
- Using callbacks for event handling
- Managing focus for keyboard input
- Proper component registration order for rendering

## Contributing

When adding new UI components:
1. Create interface/implementation in `UI/Components/`
2. Follow the IUIComponent interface
3. Inherit from AUIComponent for common functionality
4. Add implementation to `src/UI/Components/`
5. Update CMakeLists.txt to include new source files
6. Add comprehensive tests in `test/` directory
7. Update this README with usage examples
8. Follow the naming convention: `_private_member` for member variables
