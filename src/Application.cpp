#include "Application.h"
#include "Window.h"
#include "Canvas.h"
#include "InputManager.h"
#include "BrushEngine.h"
#include "Renderer.h"
#include <SDL2/SDL.h>
#include <iostream>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <SDL2/SDL_syswm.h>
#endif

namespace Acute {

Application::Application()
    : m_running(false)
    , m_strokeActive(false)
{
}

Application::~Application() {
    shutdown();
}

bool Application::initialize(const std::string& title, int width, int height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create window
    m_window = std::make_unique<Window>(title, width, height);
    if (!m_window->initialize()) {
        return false;
    }
    
    // Create renderer
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize()) {
        return false;
    }
    
    // Create canvas
    m_canvas = std::make_unique<Canvas>(width, height);
    if (!m_canvas->initialize()) {
        return false;
    }
    
    // Create input manager
    m_inputManager = std::make_unique<InputManager>();
    
    // Initialize input manager with native window handle (for Windows Ink)
    void* nativeHandle = m_window->getNativeHandle();
    std::cout << "Initializing input manager with native handle: " << nativeHandle << std::endl;
    if (!m_inputManager->initialize(nativeHandle)) {
        std::cerr << "Warning: Failed to initialize Windows Ink input" << std::endl;
    } else {
        if (m_inputManager->isPenAvailable()) {
            std::cout << "Pen/tablet is available - pressure sensitivity enabled!" << std::endl;
        } else {
            std::cout << "No pen detected - using mouse input (no pressure sensitivity)" << std::endl;
        }
    }
    
    // Create brush engine
    m_brushEngine = std::make_unique<BrushEngine>();
    setupDefaultBrush();
    
    // Set up input callback
    m_inputManager->setInputCallback([this](const InputPoint& input, bool isPressed) {
        if (!m_brushEngine) return;
        
        if (isPressed) {
            // Begin stroke if not already active
            if (!m_strokeActive) {
                m_brushEngine->beginStroke();
                m_strokeActive = true;
            }
            
            // Process input through brush engine
            auto dabs = m_brushEngine->processInput(input);
            
            // Draw dabs on canvas
            if (!dabs.empty() && m_canvas) {
                m_canvas->drawDabs(dabs);
            }
        } else {
            // End stroke when pressure is released
            if (m_strokeActive) {
                m_brushEngine->endStroke();
                m_strokeActive = false;
            }
        }
    });
    
    m_running = true;
    
    std::cout << "Application initialized successfully" << std::endl;
    return true;
}

void Application::setupDefaultBrush() {
    BrushSettings settings;
    settings.baseSize = 30.0f;
    settings.baseOpacity = 0.8f;
    settings.baseHardness = 0.7f;
    settings.baseFlow = 0.9f;
    settings.baseSpacing = 0.15f;
    settings.colorR = 0.0f;
    settings.colorG = 0.0f;
    settings.colorB = 0.0f;
    
    // Add pressure -> size mapping
    InputMapping pressureToSize;
    pressureToSize.source = InputSource::Pressure;
    pressureToSize.target = BrushProperty::Size;
    pressureToSize.minOutput = 0.3f;
    pressureToSize.maxOutput = 1.5f;
    pressureToSize.strength = 1.0f;
    pressureToSize.curve = CurveType::Quadratic;
    settings.mappings.push_back(pressureToSize);
    
    // Add pressure -> opacity mapping
    InputMapping pressureToOpacity;
    pressureToOpacity.source = InputSource::Pressure;
    pressureToOpacity.target = BrushProperty::Opacity;
    pressureToOpacity.minOutput = 0.2f;
    pressureToOpacity.maxOutput = 1.0f;
    pressureToOpacity.strength = 0.8f;
    pressureToOpacity.curve = CurveType::Linear;
    settings.mappings.push_back(pressureToOpacity);
    
    // Add speed -> spacing mapping
    InputMapping speedToSpacing;
    speedToSpacing.source = InputSource::Speed;
    speedToSpacing.target = BrushProperty::Spacing;
    speedToSpacing.minOutput = 0.1f;
    speedToSpacing.maxOutput = 0.3f;
    speedToSpacing.strength = 0.5f;
    speedToSpacing.curve = CurveType::Linear;
    settings.mappings.push_back(speedToSpacing);
    
    m_brushEngine->setBrushSettings(settings);
}

void Application::run() {
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    
    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>((currentTime - lastTime) / frequency);
        lastTime = currentTime;
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Application::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    m_canvas->resize(width, height);
                    glViewport(0, 0, width, height);
                }
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_running = false;
                } else if (event.key.keysym.sym == SDLK_c && (event.key.keysym.mod & KMOD_CTRL)) {
                    // Clear canvas
                    m_canvas->clear();
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Process mouse input - callback will handle beginStroke
                    m_inputManager->processEvent(event);
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Process mouse input - callback will handle endStroke
                    m_inputManager->processEvent(event);
                }
                break;
                
            case SDL_MOUSEMOTION:
                // Process mouse input
                m_inputManager->processEvent(event);
                break;
                
            default:
                break;
        }
    }
}

void Application::update(float deltaTime) {
    // Update logic here if needed
    (void)deltaTime; // Unused for now
}

void Application::render() {
    // Render canvas to screen
    m_canvas->render();
    
    // Swap buffers
    m_window->swapBuffers();
}

void Application::shutdown() {
    m_brushEngine.reset();
    m_inputManager.reset();
    m_canvas.reset();
    m_renderer.reset();
    m_window.reset();
    
    SDL_Quit();
}

} // namespace Acute


