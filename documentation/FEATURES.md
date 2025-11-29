# Acute Drawing Software - Features

## Current Features

### Input System
- **Mouse Support**: Full mouse input with position and velocity tracking
- **Tablet Support (Windows Ink)**: Real pen pressure and tilt on Windows 10/11
- **Velocity Calculation**: Real-time speed and direction tracking
- **Extensible Architecture**: Designed to support additional tablet APIs (e.g., Wintab, XInput2) in the future

### Advanced Brush Engine
- **Dynamic Dab Generation**: Generates brush stamps along stroke path
- **Intelligent Spacing**: Automatic dab spacing based on brush size and speed
- **Input Interpolation**: Smooth strokes through position and pressure interpolation
- **Scatter Effects**: Randomized dab placement for texture

### Flexible Input Mapping System
The heart of brush customization:

#### Input Sources
- `Pressure`: Stylus pressure (0.0 to 1.0)
- `TiltX/TiltY`: Stylus tilt angles
- `TiltMagnitude`: Combined tilt amount
- `Speed`: Movement velocity
- `Rotation`: Stylus barrel rotation
- `Random`: Random value per dab
- `Constant`: Fixed value

#### Mappable Properties
- `Size`: Brush diameter
- `Opacity`: Transparency
- `Spacing`: Gap between dabs
- `Hardness`: Edge softness
- `Flow`: Paint accumulation
- `Scatter`: Random position offset
- `Rotation`: Brush angle

#### Curve Types
- **Linear**: Direct 1:1 mapping
- **Quadratic**: Squared response curve
- **Cubic**: Cubed response for stronger sensitivity
- **Custom**: Extendable for bezier curves

#### Mapping Controls
- **Min/Max Output**: Define output range
- **Strength**: Blend amount (0% to 100%)
- **Inversion**: Reverse input response

### High-Performance Rendering
- **OpenGL 3.3+ Core**: Modern graphics pipeline
- **GPU-Accelerated**: All rendering on graphics card
- **Framebuffer Canvas**: Off-screen rendering for compositing
- **Alpha Blending**: Proper transparency and layering
- **Smooth Gradients**: High-quality brush edges

### Canvas System
- **Resolution Independent**: Clean rendering at any size
- **Real-time Display**: Immediate visual feedback
- **Persistent Canvas**: Drawing accumulates on framebuffer
- **Clear Function**: Instant canvas reset (Ctrl+C)
- **Resizable**: Dynamic window resizing support

### Build System
- **CMake**: Modern, cross-platform build system
- **vcpkg Integration**: Easy dependency management
- **Multiple Compilers**: MSVC, GCC, Clang support

## Example Brush Presets

The project includes 7 example brush presets:

1. **Pencil**: Hard edges, strong pressure sensitivity
2. **Pen**: Consistent lines, minimal variation
3. **Marker**: Medium soft, tilt-sensitive
4. **Airbrush**: Soft, accumulating paint
5. **Calligraphy**: Angle-sensitive, thick/thin strokes
6. **Splatter**: Random scatter, variable spacing
7. **Watercolor**: Transparent, flowing, textured



