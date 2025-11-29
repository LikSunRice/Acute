# Acute Drawing Software - Architecture

This document describes the architecture and design decisions of the Acute drawing software.

## Overview

Acute is designed as a modular, extensible drawing application with a sophisticated brush engine at its core. The architecture separates concerns into distinct components that communicate through well-defined interfaces.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     Application                         │
│  (Main loop, event coordination, lifecycle management)  │
└───────────┬─────────────────────────────────────────────┘
            │
    ┌───────┴───────┬──────────┬──────────┬──────────┐
    │               │          │          │          │
┌───▼───┐   ┌──────▼─────┐ ┌──▼────┐ ┌───▼──────┐ ┌▼─────────┐
│Window │   │InputManager│ │Canvas │ │BrushEng. │ │Renderer  │
│       │   │            │ │       │ │          │ │          │
└───────┘   └────────────┘ └───────┘ └──────────┘ └──────────┘
    │                          │          │
    │                          │          │
┌───▼──────┐            ┌──────▼────┐ ┌──▼──────┐
│  SDL2    │            │   Shader  │ │InputMap │
│  OpenGL  │            │   System  │ │         │
└──────────┘            └───────────┘ └─────────┘
```

## Core Components

### 1. Application
**Purpose**: Orchestrates the entire application lifecycle

**Responsibilities**:
- Initialize all subsystems
- Run the main event loop
- Coordinate between components
- Handle global events (quit, resize, etc.)
- Manage application state

**Key Methods**:
- `initialize()`: Set up all components
- `run()`: Main event loop
- `handleEvents()`: Process SDL events
- `update()`: Update application state
- `render()`: Render the frame
- `shutdown()`: Clean up resources

### 2. Window
**Purpose**: Manage the application window and OpenGL context

**Responsibilities**:
- Create and manage SDL window
- Initialize OpenGL context
- Handle window events (resize, close)
- Manage buffer swapping

**Dependencies**: SDL2, GLEW, OpenGL

### 3. InputManager
**Purpose**: Process and normalize input from various devices (mouse + Windows Ink pen on Windows)

**Responsibilities**:
- Capture mouse and stylus events
- Calculate velocity from position changes
- Normalize input data into `InputPoint` structure
- Trigger callbacks for input events
- On Windows, cooperate with `WindowsInkInput` to handle pen pressure and tilt

**Data Flow**:
```
SDL Events / Windows Pointer Messages → WindowsInkInput (Windows) → InputManager → InputPoint → Callback → BrushEngine
```

**Features**:
- Velocity calculation based on position delta
- Timestamp tracking for temporal data
- Windows Ink tablet support (pressure + tilt) on Windows
- Extensible for future tablet API integration

### 4. BrushEngine
**Purpose**: Core brush logic and dab generation

**Responsibilities**:
- Maintain brush settings and state
- Process input through mapping system
- Generate brush dabs along stroke path
- Calculate spacing, interpolation, and scatter

**Key Algorithms**:

#### Dab Spacing
```
distance_since_last_dab += distance_moved
while distance_since_last_dab >= spacing:
    generate_dab()
    distance_since_last_dab -= spacing
```

#### Input Mapping
```
For each mapping:
    input_value = get_input_value(mapping.source)
    curved_value = apply_curve(input_value, mapping.curve)
    output_value = lerp(mapping.min, mapping.max, curved_value)
    final_value = blend(base_value, output_value, mapping.strength)
    apply_to_property(mapping.target, final_value)
```

### 5. Input Mapping System
**Purpose**: Flexible system for mapping inputs to brush properties

**Components**:
- `InputSource`: Enumeration of available input sources
- `BrushProperty`: Enumeration of modifiable properties
- `CurveType`: Transformation curves (linear, quadratic, cubic)
- `InputMapping`: Configuration for a single mapping

**Mapping Pipeline**:
1. Read input value (0.0 to 1.0)
2. Apply curve transformation
3. Scale to output range [min, max]
4. Blend with base value using strength parameter
5. Apply to target property

**Example Mappings**:
- Pressure → Size: Light touch = small brush, hard press = large brush
- Tilt → Opacity: Vertical = opaque, angled = transparent
- Speed → Spacing: Slow = close dabs, fast = spread out dabs

### 6. Canvas
**Purpose**: Manage the drawing surface and compositing

**Responsibilities**:
- Maintain framebuffer for off-screen rendering
- Render brush dabs with proper blending
- Composite canvas to screen
- Handle canvas resize

**Rendering Pipeline**:
```
1. Bind canvas framebuffer
2. For each dab:
   - Set up transformation (position, size, rotation)
   - Configure blending (alpha compositing)
   - Render textured quad with brush shape
3. Unbind framebuffer
4. Render canvas texture to screen
```

**OpenGL Resources**:
- Framebuffer object (FBO) for canvas
- Texture for canvas content
- VAO/VBO for dab geometry
- VAO/VBO for screen quad
- Brush texture (radial gradient)

### 7. Shader System
**Purpose**: Manage GLSL shaders for rendering

**Shaders**:

#### Dab Shader
- Vertex: Transform dab position, size, and rotation
- Fragment: Apply brush texture, hardness, and opacity

#### Screen Shader
- Vertex: Pass through screen quad
- Fragment: Display canvas texture

### 8. Renderer
**Purpose**: Low-level rendering utilities

**Responsibilities**:
- Set up OpenGL state
- Manage blending modes
- Clear buffers

## Data Structures

### InputPoint
Encapsulates all input data from a stylus or mouse:
```cpp
struct InputPoint {
    float x, y;              // Position
    float pressure;          // 0.0 to 1.0
    float tiltX, tiltY;      // -1.0 to 1.0
    float rotation;          // 0.0 to 360.0
    float velocityX, velocityY; // Calculated
    uint64_t timestamp;      // Milliseconds
};
```

### BrushDab
Represents a single stamp to be rendered:
```cpp
struct BrushDab {
    float x, y;              // Position
    float size;              // Size in pixels
    float opacity;           // 0.0 to 1.0
    float rotation;          // Degrees
    float hardness;          // Edge softness
    float flow;              // Paint strength
    float scatter;           // Random scatter
    float r, g, b;           // Color
};
```

### BrushSettings
Configuration for brush behavior:
```cpp
struct BrushSettings {
    float baseSize, baseOpacity, baseHardness;
    float baseFlow, baseSpacing, baseRotation;
    float colorR, colorG, colorB;
    std::vector<InputMapping> mappings;
};
```

## Design Patterns

### 1. Component-Based Architecture
Each major system is a separate class with a single responsibility, making the codebase modular and testable.

### 2. Callback Pattern
InputManager uses callbacks to notify other components of input events without tight coupling.

### 3. Strategy Pattern
The mapping system allows different curves and transformations to be applied to input data.

### 4. Resource Acquisition Is Initialization (RAII)
OpenGL resources are managed through RAII principles, with cleanup in destructors.

## Threading Model

Currently single-threaded with the following execution flow:
```
Initialize → Event Loop → Shutdown
              ↓
         Handle Events
              ↓
         Update State
              ↓
         Render Frame
              ↑
         (repeat)
```

Future versions may introduce:
- Render thread for continuous canvas updates
- Worker threads for complex brush effects
- Async I/O for save/load operations



