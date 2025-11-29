# Acute Drawing Software - File Tree

Complete project structure with descriptions.

```
Acute/
│
├── 📄 README.md                    # Project overview, features, and quick start
├── 📄 LICENSE                      # MIT License
├── 📄 QUICKSTART.md                # 5-minute setup guide
├── 📄 BUILDING.md                  # Detailed build instructions
├── 📄 ARCHITECTURE.md              # System design and architecture docs
├── 📄 FEATURES.md                  # Current and planned features
├── 📄 CONTRIBUTING.md              # Contribution guidelines
├── 📄 PROJECT_STATUS.md            # Implementation status tracking
├── 📄 SUMMARY.md                   # Complete implementation summary
├── 📄 FILETREE.md                  # This file
│
├── 🔧 CMakeLists.txt               # CMake build configuration
├── 🔧 vcpkg.json                   # vcpkg dependency manifest
├── 🔧 .gitignore                   # Git ignore rules
├── 🔧 .clang-format                # Code formatting configuration
├── 🔧 build.sh                     # Linux/macOS build script
├── 🔧 build.bat                    # Windows build script
│
├── 📁 include/                     # Public header files
│   ├── Application.h               # Main application class
│   ├── Window.h                    # SDL2 window management
│   ├── Canvas.h                    # Drawing surface management
│   ├── Renderer.h                  # OpenGL rendering utilities
│   ├── InputManager.h              # Input processing and callbacks
│   ├── BrushEngine.h               # Core brush logic and dab generation
│   ├── BrushDab.h                  # Brush dab data structure
│   ├── BrushMapping.h              # Input mapping system
│   ├── InputTypes.h                # Input data structures
│   └── Shader.h                    # GLSL shader management
│
├── 📁 src/                         # Implementation files
│   ├── main.cpp                    # Entry point
│   ├── Application.cpp             # Application implementation
│   ├── Window.cpp                  # Window implementation
│   ├── Canvas.cpp                  # Canvas implementation (includes shaders)
│   ├── Renderer.cpp                # Renderer implementation
│   ├── InputManager.cpp            # Input manager implementation
│   ├── BrushEngine.cpp             # Brush engine implementation
│   └── Shader.cpp                  # Shader implementation
│
└── 📁 examples/                    # Example code and presets
    └── brush_presets.cpp           # 7 example brush configurations
```

## File Descriptions

### Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Project overview, features, controls, build steps |
| `ARCHITECTURE.md` | System design, components, data flow, patterns |
| `FEATURES.md` | Current features and roadmap |
| `FILETREE.md` | This file structure documentation |

You can add more docs (like platform-specific guides or troubleshooting) as the project evolves.

### Header Files (include/)

| File | Lines | Purpose |
|------|-------|---------|
| `Application.h` | ~35 | Main application class definition |
| `Window.h` | ~35 | SDL2 window wrapper |
| `Canvas.h` | ~50 | Canvas/framebuffer management |
| `Renderer.h` | ~20 | OpenGL rendering utilities |
| `InputManager.h` | ~40 | Input event processing |
| `BrushEngine.h` | ~65 | Brush engine with mapping system |
| `BrushDab.h` | ~25 | Single dab data structure |
| `BrushMapping.h` | ~90 | Input mapping configuration |
| `InputTypes.h` | ~45 | Input data structures |
| `WindowsInkInput.h` | ~70 | Windows Ink pen/pressure integration (Windows only) |
| `Shader.h` | ~35 | GLSL shader wrapper |

**Total Headers:** ~440 lines

### Source Files (src/)

| File | Lines | Purpose |
|------|-------|---------|
| `main.cpp` | ~30 | Entry point and startup |
| `Application.cpp` | ~200 | Application logic and coordination |
| `Window.cpp` | ~80 | Window creation and OpenGL context |
| `Canvas.cpp` | ~280 | Canvas rendering and compositing |
| `Renderer.cpp` | ~30 | Basic rendering setup |
| `InputManager.cpp` | ~80 | Input event handling |
| `BrushEngine.cpp` | ~200 | Brush logic and dab generation |
| `WindowsInkInput.cpp` | ~300 | Windows Ink pen/pressure implementation (Windows only) |
| `Shader.cpp` | ~120 | Shader loading and compilation |

**Total Implementation:** ~1,020 lines

### Example Files (examples/)

| File | Lines | Purpose |
|------|-------|---------|
| `brush_presets.cpp` | ~300 | 7 example brush configurations |

**Total Examples:** ~300 lines

### Build Files

| File | Lines | Purpose |
|------|-------|---------|
| `CMakeLists.txt` | ~70 | CMake build configuration |
| `vcpkg.json` | ~12 | Dependency manifest for vcpkg |
| `.gitignore` | ~45 | Git ignore patterns |
| `.clang-format` | ~65 | Code formatting rules |
| `build.sh` | ~80 | Linux/macOS build script |
| `build.bat` | ~70 | Windows build script |

**Total Build System:** ~342 lines

## Grand Total

```
Documentation:     ~3,200 lines (10 files)
Header Files:      ~440 lines (10 files)
Source Files:      ~1,020 lines (8 files)
Examples:          ~300 lines (1 file)
Build System:      ~342 lines (6 files)
────────────────────────────────────
Total:             ~5,300 lines (35 files)
```

## Component Relationships

```
Application
    ├── Window (SDL2 + OpenGL)
    ├── Canvas
    │   ├── Shader (Dab Shader)
    │   └── Shader (Screen Shader)
    ├── InputManager
    │   └── InputPoint (data)
    ├── BrushEngine
    │   ├── BrushSettings
    │   ├── BrushMapping (system)
    │   └── BrushDab (output)
    └── Renderer (utilities)
```

## Data Flow

```
User Input (SDL Events)
    ↓
InputManager (process & normalize)
    ↓
InputPoint (structured data)
    ↓
BrushEngine (apply mappings)
    ↓
BrushDab[] (generated dabs)
    ↓
Canvas (render to framebuffer)
    ↓
Screen (composite to window)
```

## Key Files for Understanding

1. **Start Here:**
   - `README.md` - Project overview
   - `QUICKSTART.md` - Get it running
   - `main.cpp` - Entry point

2. **Core Logic:**
   - `BrushEngine.h/.cpp` - Brush mechanics
   - `Canvas.cpp` - Rendering pipeline
   - `Application.cpp` - Coordination

3. **Input System:**
   - `InputTypes.h` - Data structures
   - `InputManager.cpp` - Event processing
   - `BrushMapping.h` - Mapping system

4. **Architecture:**
   - `ARCHITECTURE.md` - System design
   - `FEATURES.md` - Capabilities
   - `PROJECT_STATUS.md` - What's done

## Build Output

After building, you'll have:
```
build/
├── AcuteDrawing[.exe]          # Main executable
├── CMakeCache.txt              # CMake configuration cache
├── CMakeFiles/                 # CMake build files
└── [various build artifacts]
```

## External Dependencies (Not in Repo)

These are installed via package managers:
- SDL2 (libsdl2-dev / sdl2:x64-windows)
- GLEW (libglew-dev / glew:x64-windows)
- OpenGL (system provided)

---

*This file tree represents the complete Acute Drawing Software project structure.*


