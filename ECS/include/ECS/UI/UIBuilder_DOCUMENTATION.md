# UIBuilder Documentation

## Overview

The `UIBuilder` is a template-based builder pattern implementation for creating UI components in the R-Type engine. It provides a fluent, chainable API for constructing buttons, text, panels, and input fields with minimal boilerplate.

The builder supports automatic centering, color theming, styling, and callbacks, making it easy to create consistent and responsive UI elements.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Available Builders](#available-builders)
3. [Core Methods](#core-methods)
4. [Positioning](#positioning)
5. [Styling](#styling)
6. [Colors](#colors)
7. [Examples](#examples)

---

## Quick Start

The UIBuilder uses a fluent interface pattern. Chain methods together and call `build()` to create your UI component:

```cpp
auto button = ButtonBuilder()
    .centered()
    .size(200, 50)
    .text("Click Me")
    .color(BLUE)
    .onClick([]() { 
        std::cout << "Button clicked!" << std::endl; 
    })
    .build(screenWidth, screenHeight);
```

---

## Available Builders

### ButtonBuilder
Creates interactive button components with hover and press states.

**Template specialization:** `UIBuilder<UI::UIButton>`

**Alias:** `ButtonBuilder`

```cpp
auto button = ButtonBuilder()
    .text("Button Text")
    .build(screenWidth, screenHeight);
```

### TextBuilder
Creates non-interactive text display components.

**Template specialization:** `UIBuilder<UI::UIText>`

**Alias:** `TextBuilder`

```cpp
auto text = TextBuilder()
    .text("Display Text")
    .fontSize(24)
    .build(screenWidth, screenHeight);
```

### PanelBuilder
Creates background panel components for grouping UI elements.

**Template specialization:** `UIBuilder<UI::UIPanel>`

**Alias:** `PanelBuilder`

```cpp
auto panel = PanelBuilder()
    .size(400, 300)
    .backgroundColor(GRAY)
    .border(2, WHITE)
    .build(screenWidth, screenHeight);
```

### InputBuilder
Creates interactive input field components with placeholder text and focus states.

**Template specialization:** `UIBuilder<UI::UIInputField>`

**Alias:** `InputBuilder`

```cpp
auto input = InputBuilder()
    .placeholder("Enter text...")
    .size(300, 40)
    .build(screenWidth, screenHeight);
```

---

## Core Methods

### Positioning

#### `at(float x, float y)`
Sets the exact position of the component at pixel coordinates (x, y).

**Parameters:**
- `x` (float): Horizontal position
- `y` (float): Vertical position

**Returns:** Reference to this builder

**Example:**
```cpp
ButtonBuilder()
    .at(100, 50)
    .size(100, 40)
    .text("Position Test")
    .build(screenWidth, screenHeight);
```

#### `centered(float yOffset = 0)`
Centers the component on the screen. Optionally offset vertically.

**Parameters:**
- `yOffset` (float, optional): Vertical offset in pixels. Positive values move down, negative values move up.

**Returns:** Reference to this builder

**Example:**
```cpp
// Center horizontally and vertically
ButtonBuilder()
    .centered()
    .build(screenWidth, screenHeight);

// Center with 50px offset upward
ButtonBuilder()
    .centered(-50)
    .build(screenWidth, screenHeight);

// Center with 100px offset downward
ButtonBuilder()
    .centered(100)
    .build(screenWidth, screenHeight);
```

### Size

#### `size(float width, float height)`
Sets the width and height of the component.

**Parameters:**
- `width` (float): Component width in pixels
- `height` (float): Component height in pixels

**Returns:** Reference to this builder

**Example:**
```cpp
ButtonBuilder()
    .size(200, 50)
    .build(screenWidth, screenHeight);
```

### Text Content

#### `text(const std::string& text)`
Sets the text content (for buttons, text elements, and placeholders in input fields).

**Parameters:**
- `text` (const std::string&): The text to display

**Returns:** Reference to this builder

**Example:**
```cpp
TextBuilder()
    .text("Hello, World!")
    .build(screenWidth, screenHeight);
```

#### `placeholder(const std::string& text)`
Sets placeholder text for input fields (shown when empty).

**Parameters:**
- `text` (const std::string&): The placeholder text

**Returns:** Reference to this builder

**Example:**
```cpp
InputBuilder()
    .placeholder("Enter your name...")
    .build(screenWidth, screenHeight);
```

---

## Styling

### Colors

#### `color(Color baseColor, int hoverAmount = 20, int pressAmount = 20)`
Sets the base color and automatically calculates hover/pressed states.

**Parameters:**
- `baseColor` (Color): The normal state color
- `hoverAmount` (int, optional): How much lighter to make the hover state (0-255)
- `pressAmount` (int, optional): How much darker to make the pressed state (0-255)

**Returns:** Reference to this builder

**Example:**
```cpp
ButtonBuilder()
    .color(Color{0, 120, 215, 255}, 30, 20)
    .build(screenWidth, screenHeight);
```

#### `colors(Color normal, Color hovered, Color pressed)`
Manually set all three button color states.

**Parameters:**
- `normal` (Color): Color for normal state
- `hovered` (Color): Color for hovering state
- `pressed` (Color): Color for pressed state

**Returns:** Reference to this builder

**Example:**
```cpp
ButtonBuilder()
    .colors(
        Color{50, 50, 50, 255},      // normal
        Color{100, 100, 100, 255},   // hovered
        Color{25, 25, 25, 255}       // pressed
    )
    .build(screenWidth, screenHeight);
```

#### Preset Color Methods

Quick methods for common colors:

- `.blue()` - Blue (#0078D7)
- `.red()` - Red (#B40000)
- `.green()` - Green (#00B400)
- `.yellow()` - Yellow (#C8B400)
- `.gray()` - Gray (#646464)
- `.purple()` - Purple (#9600C8)
- `.cyan()` - Cyan (#00B4C8)

**Example:**
```cpp
ButtonBuilder()
    .red()
    .build(screenWidth, screenHeight);

TextBuilder()
    .green()
    .build(screenWidth, screenHeight);
```

#### `textColor(Color color)`
Sets the text/foreground color.

**Parameters:**
- `color` (Color): Text color

**Returns:** Reference to this builder

**Example:**
```cpp
TextBuilder()
    .textColor(WHITE)
    .build(screenWidth, screenHeight);
```

#### `backgroundColor(Color color)`
Sets the background color (primarily for panels and input fields).

**Parameters:**
- `color` (Color): Background color

**Returns:** Reference to this builder

**Example:**
```cpp
PanelBuilder()
    .backgroundColor(DARKGRAY)
    .build(screenWidth, screenHeight);
```

### Typography

#### `fontSize(int size)`
Sets the font size for text elements and buttons.

**Parameters:**
- `size` (int): Font size in pixels

**Returns:** Reference to this builder

**Example:**
```cpp
TextBuilder()
    .fontSize(32)
    .text("Large Text")
    .build(screenWidth, screenHeight);

ButtonBuilder()
    .fontSize(18)
    .text("Button")
    .build(screenWidth, screenHeight);
```

#### `alignment(UI::TextAlignment align)`
Sets text alignment (Left, Center, Right).

**Parameters:**
- `align` (UI::TextAlignment): Alignment type

**Returns:** Reference to this builder

**Example:**
```cpp
TextBuilder()
    .text("Centered Text")
    .alignment(UI::TextAlignment::Center)
    .build(screenWidth, screenHeight);
```

### Borders

#### `border(float thickness, Color color)`
Sets border style for the component.

**Parameters:**
- `thickness` (float): Border thickness in pixels
- `color` (Color): Border color

**Returns:** Reference to this builder

**Example:**
```cpp
PanelBuilder()
    .border(3, WHITE)
    .backgroundColor(BLACK)
    .build(screenWidth, screenHeight);

ButtonBuilder()
    .border(2, RED)
    .build(screenWidth, screenHeight);
```

### Input Field Styling

#### `focusedColor(Color color)`
Sets the background color when the input field is focused.

**Parameters:**
- `color` (Color): Focused background color

**Returns:** Reference to this builder

**Example:**
```cpp
InputBuilder()
    .focusedColor(LIGHTBLUE)
    .build(screenWidth, screenHeight);
```

#### `focusedBorderColor(Color color)`
Sets the border color when the input field is focused.

**Parameters:**
- `color` (Color): Focused border color

**Returns:** Reference to this builder

**Example:**
```cpp
InputBuilder()
    .focusedBorderColor(SKYBLUE)
    .build(screenWidth, screenHeight);
```

#### `placeholderColor(Color color)`
Sets the color of placeholder text.

**Parameters:**
- `color` (Color): Placeholder text color

**Returns:** Reference to this builder

**Example:**
```cpp
InputBuilder()
    .placeholderColor(DARKGRAY)
    .placeholder("Enter text...")
    .build(screenWidth, screenHeight);
```

#### `cursorColor(Color color)`
Sets the color of the input cursor.

**Parameters:**
- `color` (Color): Cursor color

**Returns:** Reference to this builder

**Example:**
```cpp
InputBuilder()
    .cursorColor(YELLOW)
    .build(screenWidth, screenHeight);
```

### Callbacks

#### `onClick(std::function<void()> callback)`
Sets a callback function to be called when the button is clicked (buttons only).

**Parameters:**
- `callback` (std::function<void()>): Function to call on click

**Returns:** Reference to this builder

**Example:**
```cpp
ButtonBuilder()
    .text("Play")
    .onClick([]() {
        std::cout << "Play button clicked!" << std::endl;
    })
    .build(screenWidth, screenHeight);

// With lambda capturing state
int score = 100;
ButtonBuilder()
    .text("Add Points")
    .onClick([&score]() {
        score += 10;
    })
    .build(screenWidth, screenHeight);
```

---

## Color Constants

The following color constants are predefined and ready to use:

```cpp
Color WHITE        // White
Color BLACK        // Black
Color GRAY         // Medium Gray
Color DARKGRAY     // Dark Gray
Color LIGHTGRAY    // Light Gray
Color BLUE         // Blue (#0078D7)
Color SKYBLUE      // Sky Blue (lighter blue)
Color DARKBLUE     // Dark Blue (darker blue)
Color RED          // Red
Color GREEN        // Green
Color YELLOW       // Yellow
Color PURPLE       // Purple
Color CYAN         // Cyan
```

---

## Examples

### Example 1: Simple Button

```cpp
auto playButton = ButtonBuilder()
    .centered(-50)
    .size(200, 60)
    .text("PLAY")
    .color(BLUE)
    .fontSize(24)
    .onClick([]() {
        std::cout << "Starting game..." << std::endl;
    })
    .build(screenWidth, screenHeight);
```

### Example 2: Menu Panel with Title and Buttons

```cpp
// Background panel
auto menuPanel = PanelBuilder()
    .centered()
    .size(400, 500)
    .backgroundColor(DARKGRAY)
    .border(5, BLUE)
    .build(screenWidth, screenHeight);

// Title text
auto titleText = TextBuilder()
    .centered(-150)
    .size(400, 60)
    .text("MAIN MENU")
    .fontSize(40)
    .textColor(WHITE)
    .alignment(UI::TextAlignment::Center)
    .build(screenWidth, screenHeight);

// Play button
auto playButton = ButtonBuilder()
    .centered(-50)
    .size(200, 50)
    .text("PLAY")
    .color(GREEN)
    .fontSize(20)
    .onClick([]() { /* Start game */ })
    .build(screenWidth, screenHeight);

// Settings button
auto settingsButton = ButtonBuilder()
    .centered(20)
    .size(200, 50)
    .text("SETTINGS")
    .color(BLUE)
    .fontSize(20)
    .onClick([]() { /* Open settings */ })
    .build(screenWidth, screenHeight);

// Quit button
auto quitButton = ButtonBuilder()
    .centered(90)
    .size(200, 50)
    .text("QUIT")
    .color(RED)
    .fontSize(20)
    .onClick([]() { /* Exit */ })
    .build(screenWidth, screenHeight);
```

### Example 3: Input Form

```cpp
// Background panel
auto formPanel = PanelBuilder()
    .centered()
    .size(350, 250)
    .backgroundColor(GRAY)
    .border(2, WHITE)
    .build(screenWidth, screenHeight);

// Label text
auto label = TextBuilder()
    .centered(-80)
    .text("Enter Your Name:")
    .fontSize(18)
    .textColor(WHITE)
    .build(screenWidth, screenHeight);

// Input field
auto nameInput = InputBuilder()
    .centered(-30)
    .size(300, 40)
    .placeholder("Your name here...")
    .textColor(WHITE)
    .backgroundColor(DARKGRAY)
    .focusedColor(LIGHTGRAY)
    .focusedBorderColor(SKYBLUE)
    .placeholderColor(DARKGRAY)
    .build(screenWidth, screenHeight);

// Submit button
auto submitButton = ButtonBuilder()
    .centered(40)
    .size(150, 40)
    .text("SUBMIT")
    .color(GREEN)
    .fontSize(16)
    .onClick([]() { /* Process form */ })
    .build(screenWidth, screenHeight);
```

### Example 4: Themed Game Over Screen

```cpp
auto gameOverPanel = PanelBuilder()
    .centered()
    .size(500, 400)
    .backgroundColor(Color{20, 20, 40, 255})  // Dark blue background
    .border(4, RED)
    .build(screenWidth, screenHeight);

auto gameOverTitle = TextBuilder()
    .centered(-120)
    .text("GAME OVER")
    .fontSize(48)
    .textColor(RED)
    .alignment(UI::TextAlignment::Center)
    .build(screenWidth, screenHeight);

auto scoreText = TextBuilder()
    .centered(-20)
    .text("Final Score: 5250")
    .fontSize(24)
    .textColor(YELLOW)
    .alignment(UI::TextAlignment::Center)
    .build(screenWidth, screenHeight);

auto retryButton = ButtonBuilder()
    .centered(60)
    .size(180, 50)
    .text("RETRY")
    .color(GREEN)
    .fontSize(20)
    .onClick([]() { /* Restart game */ })
    .build(screenWidth, screenHeight);

auto menuButton = ButtonBuilder()
    .centered(130)
    .size(180, 50)
    .text("MENU")
    .color(BLUE)
    .fontSize(20)
    .onClick([]() { /* Return to menu */ })
    .build(screenWidth, screenHeight);
```

---

## Best Practices

1. **Use `centered()` for responsive layouts** - Automatically adjusts to different screen sizes
2. **Chain methods for readability** - More readable than setting each property separately
3. **Use preset color methods** - Ensures consistency throughout your UI
4. **Provide meaningful text** - Use clear, action-oriented button text
5. **Set reasonable font sizes** - Ensure text is readable at your target resolution
6. **Use color to indicate state** - Different colors for different button purposes (play, settings, quit)
7. **Call `build()` with correct screen dimensions** - Use the actual render target size for accurate positioning

---

## Default Values

If methods are not called, the builder uses these defaults:

| Property | Default Value |
|----------|--------------|
| Position (x, y) | (0, 0) |
| Width | 400.0 |
| Height | 100.0 |
| Text | "" (empty) |
| Centered | false |
| Normal Color | BLUE |
| Hovered Color | SKYBLUE |
| Pressed Color | DARKBLUE |
| Text Color | WHITE |
| Background Color | GRAY |
| Border Color | WHITE |
| Border Thickness | 2.0 |
| Font Size | 24 |
| Placeholder Text | "" (empty) |
| Text Alignment | Left |

---

## Technical Details

### ColorCalculator

The `ColorCalculator` class provides utility functions for color manipulation:

- **`lighter(Color base, int amount = 20)`** - Returns a lighter shade by adding the amount to each RGB component
- **`darker(Color base, int amount = 20)`** - Returns a darker shade by subtracting the amount from each RGB component

These are automatically used when setting button states with the `color()` method.

### Build Method

The `build()` method:
1. Creates the appropriate UI component
2. Applies all set styling and positioning
3. Calculates centered positions if needed
4. Returns a shared_ptr to the newly created component

Each builder type has its own specialization to handle component-specific properties.

---

## Compatibility

- **C++17 or later** - Required for shared_ptr and std::function
- **RayLib** - Uses RayLib colors and font rendering
- **ECS Framework** - Designed to work with the R-Type ECS system

---

## License

MIT License - See UIBuilder.hpp header for full license text.

---

## Authors

- paul1.emeriau (Original implementation)
- R-Type Development Team

---

---

# Advanced: Custom Renderers and Builder Extensions

## Overview

The UIBuilder system is extensible through template specialization, allowing you to create custom UI components with their own rendering logic and builder extensions. This section explains how to implement custom renderers and provide builders for them using **GlitchButton** as a practical example.

---

## Why Custom Renderers?

Custom renderers are needed when you want UI components with:
- Unique visual effects (glitch, neon glow, animations)
- Special drawing logic beyond standard shapes
- Enhanced interactivity and feedback
- Themed or stylized aesthetics

The GlitchButton is a perfect example: it extends the standard button with cyberpunk aesthetics including neon colors, glow effects, and glitch animations on hover.

---

## Architecture

### Component Hierarchy

```
UIComponent (Base)
    ↓
UIButton (Standard button)
    ↓
GlitchButton (Custom renderer - adds glitch effects)
```

The key principle is **inheritance**: Your custom component inherits from an existing UI component and overrides rendering methods.

---

## Creating a Custom Renderer: GlitchButton Example

### Step 1: Define the Component Class

Create a header file for your custom component:

```cpp
// GlitchButton.hpp
#pragma once

#include "ECS/UI/Components/Button.hpp"
#include <cmath>
#include <raylib.h>

namespace RType {
    class GlitchButton : public UI::UIButton {
    public:
        // Constructor
        GlitchButton(float x, float y, float w, float h, const std::string& text)
            : UIButton(x, y, w, h, text) {
            _hover_seed = rand() % 1000;
        }

        // Static defaults (shared across all instances)
        static Color default_neon_color;
        static Color default_neon_glow_color;
        static float default_jitter_amplitude;
        static float default_jitter_speed;
        static bool default_enable_glitch;

        // Setters for custom properties
        void set_neon_colors(Color neon, Color glow) {
            _neon_color = neon;
            _neon_glow_color = glow;
        }

        void set_glitch_params(float amplitude, float speed, bool enabled = true) {
            _hover_jitter_amplitude = amplitude;
            _hover_jitter_speed = speed;
            _enable_glitch_on_hover = enabled;
        }

    protected:
        // Override render methods from UIButton
        void drawButtonBackground() const override;
        void drawButtonText() const override;

    private:
        // Custom properties
        Color _neon_color{0, 229, 255, 255};
        Color _neon_glow_color{0, 229, 255, 100};
        float _hover_jitter_amplitude{2.0f};
        float _hover_jitter_speed{8.0f};
        bool _enable_glitch_on_hover{true};
        int _hover_seed{0};
    };
}
```

### Step 2: Implement the Custom Renderer

```cpp
// GlitchButton.cpp
#include "GlitchButton.hpp"
#include "ECS/Renderer/RenderManager.hpp"

// Initialize static defaults
Color RType::GlitchButton::default_neon_color = Color{0, 229, 255, 255};
Color RType::GlitchButton::default_neon_glow_color = Color{0, 229, 255, 100};
float RType::GlitchButton::default_jitter_amplitude = 2.0f;
float RType::GlitchButton::default_jitter_speed = 8.0f;
bool RType::GlitchButton::default_enable_glitch = true;

void RType::GlitchButton::drawButtonBackground() const {
    // Get current rendering state
    auto& renderManager = RenderManager::instance();
    
    // Calculate glitch offset if hovering
    float jitterX = 0.0f;
    float jitterY = 0.0f;
    
    if (_is_hovered && _enable_glitch_on_hover) {
        // Use sine wave for smooth glitch animation
        float time = GetTime();
        jitterX = std::sin(time * _hover_jitter_speed + _hover_seed) * _hover_jitter_amplitude;
        jitterY = std::cos(time * _hover_jitter_speed + _hover_seed) * _hover_jitter_amplitude;
    }
    
    // Draw glow effect (larger, semi-transparent)
    DrawRectangleRounded(
        {_x + jitterX - 2, _y + jitterY - 2, _width + 4, _height + 4},
        0.1f,
        10,
        _neon_glow_color
    );
    
    // Draw main background
    DrawRectangleRounded(
        {_x + jitterX, _y + jitterY, _width, _height},
        0.1f,
        10,
        _is_pressed ? _style.getPressedColor() : 
        (_is_hovered ? _style.getHoveredColor() : _style.getNormalColor())
    );
    
    // Draw neon border
    DrawRectangleRoundedLines(
        {_x + jitterX, _y + jitterY, _width, _height},
        0.1f,
        10,
        _style.getBorderThickness(),
        _neon_color
    );
}

void RType::GlitchButton::drawButtonText() const {
    // Calculate text position with potential glitch offset
    float jitterX = 0.0f;
    float jitterY = 0.0f;
    
    if (_is_hovered && _enable_glitch_on_hover) {
        float time = GetTime();
        jitterX = std::sin(time * _hover_jitter_speed + _hover_seed) * _hover_jitter_amplitude;
        jitterY = std::cos(time * _hover_jitter_speed + _hover_seed) * _hover_jitter_amplitude;
    }
    
    auto& renderManager = RenderManager::instance();
    Vector2 textSize = MeasureTextEx(
        renderManager.get_font(),
        _text.c_str(),
        static_cast<float>(_style.getFontSize()),
        0.0f
    );
    
    float textX = _x + (_width / 2.0f) - (textSize.x / 2.0f) + jitterX;
    float textY = _y + (_height / 2.0f) - (textSize.y / 2.0f) + jitterY;
    
    DrawTextEx(
        renderManager.get_font(),
        _text.c_str(),
        {textX, textY},
        static_cast<float>(_style.getFontSize()),
        0.0f,
        _style.getTextColor()
    );
}
```

### Step 3: Create a Builder Specialization

This is the key extension that makes the builder pattern work with custom components:

```cpp
// In GlitchButton.hpp, add:

// Forward declaration
template<>
class UIBuilder<RType::GlitchButton> : public UIBuilder<UI::UIButton> {
    public:
        UIBuilder() : UIBuilder<UI::UIButton>() {}

        // NEW: Custom builder methods for GlitchButton-specific properties
        UIBuilder<RType::GlitchButton>& neonColors(Color neon, Color glow) {
            _neonColor = neon;
            _neonGlowColor = glow;
            return *this;
        }

        UIBuilder<RType::GlitchButton>& glitchParams(float amplitude, float speed, bool enabled = true) {
            _jitterAmplitude = amplitude;
            _jitterSpeed = speed;
            _enableGlitch = enabled;
            return *this;
        }

        // Override inherited methods to return correct type
        UIBuilder<RType::GlitchButton>& at(float x, float y) {
            UIBuilder<UI::UIButton>::at(x, y);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& centered(float yOffset = 0) {
            UIBuilder<UI::UIButton>::centered(yOffset);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& size(float width, float height) {
            UIBuilder<UI::UIButton>::size(width, height);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& text(const std::string& text) {
            UIBuilder<UI::UIButton>::text(text);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& color(Color baseColor, int hoverAmount = 20, int pressAmount = 20) {
            UIBuilder<UI::UIButton>::color(baseColor, hoverAmount, pressAmount);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& textColor(Color color) {
            UIBuilder<UI::UIButton>::textColor(color);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& fontSize(int size) {
            UIBuilder<UI::UIButton>::fontSize(size);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& border(float thickness, Color color) {
            UIBuilder<UI::UIButton>::border(thickness, color);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& onClick(std::function<void()> callback) {
            UIBuilder<UI::UIButton>::onClick(callback);
            return *this;
        }

        // Build method - creates and configures the component
        std::shared_ptr<RType::GlitchButton> build(float screenWidth, float screenHeight) {
            // Create the button instance
            auto button = std::make_shared<RType::GlitchButton>(
                calculateX(screenWidth), 
                calculateY(screenHeight), 
                _width, 
                _height, 
                _text
            );

            // Apply standard button styling
            UI::ButtonStyle style;
            style.setNormalColor(_normalColor);
            style.setHoveredColor(_hoveredColor);
            style.setPressedColor(_pressedColor);
            style.setTextColor(_textColor);
            style.setFontSize(_fontSize);
            style.setBorderThickness(_borderThickness);
            style.setBorderColor(_borderColor);
            button->setStyle(style);

            // Apply callback
            if (_onClick) {
                button->setOnClick(_onClick);
            }

            // Apply custom GlitchButton properties
            button->set_neon_colors(_neonColor, _neonGlowColor);
            button->set_glitch_params(_jitterAmplitude, _jitterSpeed, _enableGlitch);

            return button;
        }

    private:
        // Custom properties
        Color _neonColor{RType::GlitchButton::default_neon_color};
        Color _neonGlowColor{RType::GlitchButton::default_neon_glow_color};
        float _jitterAmplitude{RType::GlitchButton::default_jitter_amplitude};
        float _jitterSpeed{RType::GlitchButton::default_jitter_speed};
        bool _enableGlitch{RType::GlitchButton::default_enable_glitch};
};

// Convenient type alias
using GlitchButtonBuilder = UIBuilder<RType::GlitchButton>;
```

---

## Using Custom Renderers with Builders

Once you've created the builder specialization, using it is simple:

### Basic Example

```cpp
auto glitchButton = GlitchButtonBuilder()
    .centered()
    .size(200, 50)
    .text("GLITCH MODE")
    .color(CYAN)
    .neonColors(CYAN, Color{0, 229, 255, 100})
    .glitchParams(2.0f, 8.0f, true)
    .onClick([]() { std::cout << "Glitch!" << std::endl; })
    .build(screenWidth, screenHeight);
```

### Complex Example - Themed Game Menu

```cpp
// Red neon buttons
auto deleteButton = GlitchButtonBuilder()
    .centered(-50)
    .size(200, 50)
    .text("DELETE")
    .red()
    .neonColors(RED, Color{255, 0, 0, 100})
    .glitchParams(3.0f, 10.0f, true)
    .onClick([]() { /* Handle deletion */ })
    .build(screenWidth, screenHeight);

// Green neon buttons
auto confirmButton = GlitchButtonBuilder()
    .centered(50)
    .size(200, 50)
    .text("CONFIRM")
    .green()
    .neonColors(GREEN, Color{0, 255, 0, 100})
    .glitchParams(2.0f, 6.0f, true)
    .onClick([]() { /* Handle confirmation */ })
    .build(screenWidth, screenHeight);

// Purple neon buttons for special effects
auto effectButton = GlitchButtonBuilder()
    .centered(150)
    .size(200, 50)
    .text("SPECIAL")
    .purple()
    .neonColors(PURPLE, Color{150, 0, 200, 100})
    .glitchParams(4.0f, 12.0f, true)
    .onClick([]() { /* Handle special effect */ })
    .build(screenWidth, screenHeight);
```

---

## Best Practices for Custom Renderers

### 1. Inherit from Existing Components
```cpp
// Good - reuses existing functionality
class GlitchButton : public UI::UIButton { /*...*/ };

// Avoid - reimplements everything
class GlitchButton { /*...*/ };
```

### 2. Override Only What You Need
```cpp
// Good - override only the drawing methods
protected:
    void drawButtonBackground() const override;
    void drawButtonText() const override;

// Avoid - reimplementing the entire update cycle
```

### 3. Use Builder Specialization for Extensibility
```cpp
// Good - provides fluent API for custom properties
UIBuilder<RType::GlitchButton>& neonColors(Color neon, Color glow);
UIBuilder<RType::GlitchButton>& glitchParams(float amplitude, float speed, bool enabled);

// Avoid - requiring manual method calls after building
auto button = std::make_shared<GlitchButton>(...);
button->set_neon_colors(...);  // No fluent chaining
```

### 4. Provide Sensible Defaults
```cpp
// In your component
static Color default_neon_color = Color{0, 229, 255, 255};
static float default_jitter_speed = 8.0f;

// In your builder specialization
Color _neonColor{RType::GlitchButton::default_neon_color};
float _jitterSpeed{RType::GlitchButton::default_jitter_speed};
```

### 5. Use Time-Based Animations
```cpp
// Good - smooth animations based on time
float time = GetTime();
float jitterX = std::sin(time * _hover_jitter_speed) * _hover_jitter_amplitude;

// Avoid - frame-dependent logic
static int frameCounter = 0;
frameCounter++;
if (frameCounter > 10) { /*...*/ }
```

### 6. Document Custom Properties
```cpp
/**
 * Sets neon glow colors for the button
 * @param neon The primary neon color
 * @param glow The glow aura color (usually semi-transparent)
 * @return Reference to this builder for chaining
 */
UIBuilder<RType::GlitchButton>& neonColors(Color neon, Color glow);

/**
 * Configures glitch animation parameters
 * @param amplitude How far pixels shift during glitch
 * @param speed Animation speed multiplier
 * @param enabled Whether to enable glitch on hover
 * @return Reference to this builder for chaining
 */
UIBuilder<RType::GlitchButton>& glitchParams(float amplitude, float speed, bool enabled = true);
```

---

## Creating Other Custom Renderers

The same pattern applies to any custom renderer. Examples:

### GradientButton
```cpp
class GradientButton : public UI::UIButton {
    void drawButtonBackground() const override;
    // Draw with gradient fill
};

template<>
class UIBuilder<GradientButton> : public UIBuilder<UI::UIButton> {
    UIBuilder<GradientButton>& gradientColors(Color start, Color end);
    std::shared_ptr<GradientButton> build(float screenWidth, float screenHeight);
};
```

### AnimatedButton
```cpp
class AnimatedButton : public UI::UIButton {
    void update() override;
    void drawButtonBackground() const override;
    // Draw with animations
};

template<>
class UIBuilder<AnimatedButton> : public UIBuilder<UI::UIButton> {
    UIBuilder<AnimatedButton>& animationSpeed(float speed);
    UIBuilder<AnimatedButton>& animationType(AnimationType type);
    std::shared_ptr<AnimatedButton> build(float screenWidth, float screenHeight);
};
```

### PulseButton
```cpp
class PulseButton : public UI::UIButton {
    void drawButtonBackground() const override;
    // Draw with pulsing size/opacity
};

template<>
class UIBuilder<PulseButton> : public UIBuilder<UI::UIButton> {
    UIBuilder<PulseButton>& pulseIntensity(float intensity);
    UIBuilder<PulseButton>& pulseFrequency(float frequency);
    std::shared_ptr<PulseButton> build(float screenWidth, float screenHeight);
};
```

---

## Template Specialization Reference

When creating a builder specialization:

```cpp
template<>
class UIBuilder<YourCustomComponent> : public UIBuilder<BaseComponent> {
public:
    // 1. Default constructor
    UIBuilder() : UIBuilder<BaseComponent>() {}

    // 2. Custom builder methods (return your type for chaining)
    UIBuilder<YourCustomComponent>& customProperty(...) {
        _customProperty = ...;
        return *this;
    }

    // 3. Override all inherited methods (return your type)
    UIBuilder<YourCustomComponent>& at(float x, float y) {
        UIBuilder<BaseComponent>::at(x, y);
        return *this;
    }
    
    // ... other inherited methods ...

    // 4. Custom build method
    std::shared_ptr<YourCustomComponent> build(float screenWidth, float screenHeight) {
        auto component = std::make_shared<YourCustomComponent>(...);
        
        // Apply base properties
        // Apply custom properties
        
        return component;
    }

private:
    // Custom member variables with defaults
    int _customProperty{defaultValue};
};

// 5. Create convenient alias
using YourCustomBuilder = UIBuilder<YourCustomComponent>;
```

---

## Debugging Custom Renderers

When your custom renderer isn't displaying correctly:

1. **Check the draw order** - Are you drawing background then text?
2. **Verify coordinates** - Use `DrawRectangle()` with bright color to test positions
3. **Test animations** - Comment out custom draw code, render standard button
4. **Check builder build()** - Ensure all properties are being set correctly
5. **Enable debug rendering** - Add temporary debug rectangles and text

```cpp
void RType::GlitchButton::drawButtonBackground() const {
    // Debug: Show button bounds
    #ifdef DEBUG_UI
    DrawRectangleLines(_x, _y, _width, _height, YELLOW);
    #endif
    
    // Normal rendering
    DrawRectangleRounded({_x, _y, _width, _height}, 0.1f, 10, _style.getNormalColor());
}
```

---

## Changelog

### Version 1.1 (November 2025)
- Added custom renderer documentation
- Added builder specialization guide
- Added GlitchButton as detailed example
- Added best practices section
- Added template specialization reference
