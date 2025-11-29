# Acute - Advanced Drawing Software

A cross-platform drawing application written in C++ with a sophisticated brush engine that supports pressure-sensitive input, tilt detection, and customizable input mapping.

## Features

### Brush Engine
- **Input Gathering**: Comprehensive stylus/mouse input support
  - Position (X, Y coordinates)
  - Pressure (0.0 to 1.0)
  - Tilt/Declination angles
  - Speed/Velocity tracking
  - Rotation (barrel rotation)

- **Input Mapping System**: Flexible mapping between input sources and brush properties
  - Map pressure → brush size, opacity, etc.
  - Map tilt → transparency, rotation
  - Map speed → spacing between dabs
  - Support for multiple curve types (linear, quadratic, cubic)
  - Adjustable mapping strength and inversion

- **Dab Generation & Compositing**: High-quality brush rendering
  - Customizable dab appearance (shape, texture, color)
  - Alpha blending and compositing
  - Continuous strokes via rapid dab generation
  - Adjustable hardness, flow, and scatter

### Platform Support
- **Windows**: fully supported (pressure-sensitive pen input with Windows Ink)
- **Linux / macOS**: not yet supported (planned for the future)

## Dependencies

- **SDL2**: Windowing and input handling
- **OpenGL 3.3+**: Graphics rendering
- **GLEW**: OpenGL extension loading
- **CMake 3.15+**: Build system
- **C++17 compiler**: MSVC, GCC, or Clang

## Building

### Windows (vcpkg + build script)

1. Clone vcpkg into the project folder (once):
```powershell
git clone https://github.com/Microsoft/vcpkg.git
```

2. Bootstrap vcpkg (first time only):
```powershell
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

3. Build Acute (this will install SDL2/GLEW/OpenGL automatically via vcpkg):
```powershell
.\build.bat
```

This configures CMake with the vcpkg toolchain and builds `AcuteDrawing` into the `build/Release` folder.


## Running

After building, run the executable:

### Windows
```bash
cd build
.\Release\AcuteDrawing.exe
```

## Controls

- **Left Mouse Button**: Draw
- **Ctrl+C**: Clear canvas
- **ESC**: Exit application

## Architecture

The application is organized into several key components:

### Core Components

1. **Application**: Main application loop and coordination
2. **Window**: SDL2 window management and OpenGL context
3. **Canvas**: Drawing surface with framebuffer-based rendering
4. **Renderer**: OpenGL rendering utilities
5. **InputManager**: Input event processing and velocity calculation
6. **BrushEngine**: Core brush logic with input mapping
7. **Shader**: GLSL shader management

### Data Structures

- **InputPoint**: Stores stylus/mouse input data
- **BrushDab**: Represents a single brush stamp
- **BrushSettings**: Configurable brush parameters
- **InputMapping**: Maps input sources to brush properties

## Customization

### Creating Custom Brushes

You can customize brush behavior by modifying the settings in `Application.cpp`:

```cpp
BrushSettings settings;
settings.baseSize = 30.0f;        // Base brush size
settings.baseOpacity = 0.8f;      // Base opacity
settings.baseHardness = 0.7f;     // Edge hardness
settings.baseFlow = 0.9f;         // Flow strength
settings.baseSpacing = 0.15f;     // Spacing between dabs

// Add custom mappings
InputMapping pressureToSize;
pressureToSize.source = InputSource::Pressure;
pressureToSize.target = BrushProperty::Size;
pressureToSize.minOutput = 0.3f;
pressureToSize.maxOutput = 1.5f;
pressureToSize.strength = 1.0f;
pressureToSize.curve = CurveType::Quadratic;
settings.mappings.push_back(pressureToSize);
```

### Available Input Sources

- `Pressure`: Stylus pressure (0.0 to 1.0)
- `TiltX`, `TiltY`: Stylus tilt angles
- `TiltMagnitude`: Combined tilt magnitude
- `Speed`: Movement velocity
- `Rotation`: Stylus barrel rotation
- `Random`: Random value for each dab
- `Constant`: Fixed value

### Available Brush Properties

- `Size`: Brush size in pixels
- `Opacity`: Overall transparency
- `Spacing`: Distance between dabs
- `Hardness`: Edge hardness/softness
- `Flow`: Paint flow strength
- `Scatter`: Random position scatter
- `Rotation`: Brush rotation angle

## Future Enhancements

- [ ] Native tablet API integration (Wintab for Windows, XInput2 for Linux)
- [ ] Multiple brush presets
- [ ] Color picker and palette
- [ ] Layer support
- [ ] Undo/redo functionality
- [ ] Save/load canvas
- [ ] Custom brush textures
- [ ] Brush texture stamps
- [ ] Advanced blending modes
- [ ] Performance optimizations for large canvases

