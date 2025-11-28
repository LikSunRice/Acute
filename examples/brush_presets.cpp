// Example brush presets for Acute Drawing Software
// This file demonstrates how to create various brush styles

#include "../include/BrushEngine.h"
#include "../include/BrushMapping.h"

namespace Acute {
namespace BrushPresets {

// Pencil-like brush with strong pressure sensitivity
BrushSettings createPencil() {
    BrushSettings settings;
    settings.baseSize = 3.0f;
    settings.baseOpacity = 0.6f;
    settings.baseHardness = 0.9f;
    settings.baseFlow = 0.7f;
    settings.baseSpacing = 0.05f;
    settings.colorR = 0.1f;
    settings.colorG = 0.1f;
    settings.colorB = 0.1f;
    
    // Strong pressure to size mapping
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.2f;
    pressureSize.maxOutput = 2.0f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Cubic;
    settings.mappings.push_back(pressureSize);
    
    // Pressure affects opacity slightly
    InputMapping pressureOpacity;
    pressureOpacity.source = InputSource::Pressure;
    pressureOpacity.target = BrushProperty::Opacity;
    pressureOpacity.minOutput = 0.3f;
    pressureOpacity.maxOutput = 1.0f;
    pressureOpacity.strength = 0.7f;
    settings.mappings.push_back(pressureOpacity);
    
    return settings;
}

// Soft airbrush with low hardness
BrushSettings createAirbrush() {
    BrushSettings settings;
    settings.baseSize = 50.0f;
    settings.baseOpacity = 0.15f;
    settings.baseHardness = 0.1f;
    settings.baseFlow = 0.3f;
    settings.baseSpacing = 0.1f;
    settings.colorR = 0.0f;
    settings.colorG = 0.0f;
    settings.colorB = 0.0f;
    
    // Pressure to size
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.5f;
    pressureSize.maxOutput = 1.5f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Linear;
    settings.mappings.push_back(pressureSize);
    
    // Pressure to flow (important for airbrush feel)
    InputMapping pressureFlow;
    pressureFlow.source = InputSource::Pressure;
    pressureFlow.target = BrushProperty::Flow;
    pressureFlow.minOutput = 0.1f;
    pressureFlow.maxOutput = 1.0f;
    pressureFlow.strength = 1.0f;
    pressureFlow.curve = CurveType::Quadratic;
    settings.mappings.push_back(pressureFlow);
    
    return settings;
}

// Ink pen with consistent line width
BrushSettings createPen() {
    BrushSettings settings;
    settings.baseSize = 5.0f;
    settings.baseOpacity = 1.0f;
    settings.baseHardness = 0.95f;
    settings.baseFlow = 1.0f;
    settings.baseSpacing = 0.08f;
    settings.colorR = 0.0f;
    settings.colorG = 0.0f;
    settings.colorB = 0.0f;
    
    // Minimal pressure sensitivity for consistent lines
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.8f;
    pressureSize.maxOutput = 1.2f;
    pressureSize.strength = 0.5f;
    pressureSize.curve = CurveType::Linear;
    settings.mappings.push_back(pressureSize);
    
    return settings;
}

// Marker with tilt sensitivity
BrushSettings createMarker() {
    BrushSettings settings;
    settings.baseSize = 20.0f;
    settings.baseOpacity = 0.7f;
    settings.baseHardness = 0.8f;
    settings.baseFlow = 0.9f;
    settings.baseSpacing = 0.12f;
    settings.colorR = 0.2f;
    settings.colorG = 0.2f;
    settings.colorB = 0.8f;
    
    // Pressure to size
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.6f;
    pressureSize.maxOutput = 1.4f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Linear;
    settings.mappings.push_back(pressureSize);
    
    // Tilt affects opacity (more tilt = more transparent)
    InputMapping tiltOpacity;
    tiltOpacity.source = InputSource::TiltMagnitude;
    tiltOpacity.target = BrushProperty::Opacity;
    tiltOpacity.minOutput = 1.0f;
    tiltOpacity.maxOutput = 0.4f;
    tiltOpacity.strength = 0.8f;
    tiltOpacity.curve = CurveType::Linear;
    settings.mappings.push_back(tiltOpacity);
    
    return settings;
}

// Splatter brush with random scatter
BrushSettings createSplatter() {
    BrushSettings settings;
    settings.baseSize = 15.0f;
    settings.baseOpacity = 0.6f;
    settings.baseHardness = 0.7f;
    settings.baseFlow = 0.8f;
    settings.baseSpacing = 0.3f;
    settings.colorR = 0.0f;
    settings.colorG = 0.0f;
    settings.colorB = 0.0f;
    
    // Pressure to size
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.3f;
    pressureSize.maxOutput = 1.8f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Quadratic;
    settings.mappings.push_back(pressureSize);
    
    // Random scatter
    InputMapping randomScatter;
    randomScatter.source = InputSource::Random;
    randomScatter.target = BrushProperty::Scatter;
    randomScatter.minOutput = 0.0f;
    randomScatter.maxOutput = 1.0f;
    randomScatter.strength = 1.0f;
    randomScatter.curve = CurveType::Linear;
    settings.mappings.push_back(randomScatter);
    
    // Speed affects spacing (faster = more spread)
    InputMapping speedSpacing;
    speedSpacing.source = InputSource::Speed;
    speedSpacing.target = BrushProperty::Spacing;
    speedSpacing.minOutput = 0.2f;
    speedSpacing.maxOutput = 0.5f;
    speedSpacing.strength = 0.7f;
    speedSpacing.curve = CurveType::Linear;
    settings.mappings.push_back(speedSpacing);
    
    return settings;
}

// Calligraphy brush with rotation sensitivity
BrushSettings createCalligraphy() {
    BrushSettings settings;
    settings.baseSize = 25.0f;
    settings.baseOpacity = 0.9f;
    settings.baseHardness = 0.85f;
    settings.baseFlow = 1.0f;
    settings.baseSpacing = 0.1f;
    settings.colorR = 0.0f;
    settings.colorG = 0.0f;
    settings.colorB = 0.0f;
    
    // Pressure to size (for thick/thin strokes)
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.4f;
    pressureSize.maxOutput = 1.6f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Quadratic;
    settings.mappings.push_back(pressureSize);
    
    // Tilt to rotation (pen angle affects stroke angle)
    InputMapping tiltRotation;
    tiltRotation.source = InputSource::TiltX;
    tiltRotation.target = BrushProperty::Rotation;
    tiltRotation.minOutput = -45.0f;
    tiltRotation.maxOutput = 45.0f;
    tiltRotation.strength = 1.0f;
    tiltRotation.curve = CurveType::Linear;
    settings.mappings.push_back(tiltRotation);
    
    return settings;
}

// Watercolor-like brush with low opacity and flow
BrushSettings createWatercolor() {
    BrushSettings settings;
    settings.baseSize = 40.0f;
    settings.baseOpacity = 0.25f;
    settings.baseHardness = 0.3f;
    settings.baseFlow = 0.4f;
    settings.baseSpacing = 0.08f;
    settings.colorR = 0.3f;
    settings.colorG = 0.5f;
    settings.colorB = 0.8f;
    
    // Pressure to size
    InputMapping pressureSize;
    pressureSize.source = InputSource::Pressure;
    pressureSize.target = BrushProperty::Size;
    pressureSize.minOutput = 0.5f;
    pressureSize.maxOutput = 1.5f;
    pressureSize.strength = 1.0f;
    pressureSize.curve = CurveType::Linear;
    settings.mappings.push_back(pressureSize);
    
    // Pressure to flow
    InputMapping pressureFlow;
    pressureFlow.source = InputSource::Pressure;
    pressureFlow.target = BrushProperty::Flow;
    pressureFlow.minOutput = 0.2f;
    pressureFlow.maxOutput = 1.0f;
    pressureFlow.strength = 0.9f;
    pressureFlow.curve = CurveType::Quadratic;
    settings.mappings.push_back(pressureFlow);
    
    // Slight scatter for texture
    InputMapping randomScatter;
    randomScatter.source = InputSource::Random;
    randomScatter.target = BrushProperty::Scatter;
    randomScatter.minOutput = 0.0f;
    randomScatter.maxOutput = 0.3f;
    randomScatter.strength = 1.0f;
    randomScatter.curve = CurveType::Linear;
    settings.mappings.push_back(randomScatter);
    
    return settings;
}

} // namespace BrushPresets
} // namespace Acute


