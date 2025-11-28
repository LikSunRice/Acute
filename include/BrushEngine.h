#pragma once

#include "InputTypes.h"
#include "BrushMapping.h"
#include "BrushDab.h"
#include <vector>
#include <map>
#include <memory>

namespace Acute {

// Brush settings that define the base behavior
struct BrushSettings {
    float baseSize;          // Base brush size in pixels
    float baseOpacity;       // Base opacity
    float baseHardness;      // Base hardness
    float baseFlow;          // Base flow
    float baseSpacing;       // Spacing between dabs (as fraction of size)
    float baseRotation;      // Base rotation
    
    // Color
    float colorR, colorG, colorB;
    
    // Mappings
    std::vector<InputMapping> mappings;
    
    BrushSettings()
        : baseSize(20.0f)
        , baseOpacity(1.0f)
        , baseHardness(0.5f)
        , baseFlow(1.0f)
        , baseSpacing(0.15f)
        , baseRotation(0.0f)
        , colorR(0.0f), colorG(0.0f), colorB(0.0f)
    {}
};

// The brush engine processes input and generates dabs
class BrushEngine {
public:
    BrushEngine();
    ~BrushEngine();
    
    // Set the current brush settings
    void setBrushSettings(const BrushSettings& settings);
    const BrushSettings& getBrushSettings() const { return m_settings; }
    
    // Process input and generate dabs for a stroke
    std::vector<BrushDab> processInput(const InputPoint& input);
    
    // Reset the engine state (call at start of new stroke)
    void beginStroke();
    void endStroke();
    
    // Add a mapping to the current brush
    void addMapping(const InputMapping& mapping);
    void clearMappings();
    
private:
    BrushSettings m_settings;
    
    // Stroke state
    bool m_strokeActive;
    InputPoint m_lastInput;
    float m_distanceSinceLastDab;
    
    // Generate a single dab from input
    BrushDab generateDab(const InputPoint& input);
    
    // Apply all mappings to modify dab properties
    void applyMappings(const InputPoint& input, BrushDab& dab);
    
    // Get value from input source
    float getInputValue(const InputPoint& input, InputSource source);
    
    // Calculate spacing based on current settings
    float calculateSpacing(const BrushDab& dab);
    
    // Apply random scatter to dab position
    void applyScatter(BrushDab& dab);
};

} // namespace Acute


