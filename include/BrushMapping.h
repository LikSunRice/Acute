#pragma once

#include <functional>
#include <string>
#include <map>

namespace Acute {

// Input sources that can be mapped
enum class InputSource {
    Pressure,
    TiltX,
    TiltY,
    TiltMagnitude,
    Speed,
    Rotation,
    Random,
    Constant
};

// Brush properties that can be affected
enum class BrushProperty {
    Size,
    Opacity,
    Spacing,
    Hardness,
    Flow,
    Scatter,
    Rotation,
    ColorH,  // Hue
    ColorS,  // Saturation
    ColorV   // Value
};

// Mapping curve types
enum class CurveType {
    Linear,
    Quadratic,
    Cubic,
    Custom
};

// A single mapping from input to output
struct InputMapping {
    InputSource source;
    BrushProperty target;
    float minOutput;      // Minimum output value
    float maxOutput;      // Maximum output value
    float strength;       // Mapping strength (0.0 to 1.0)
    CurveType curve;
    bool inverted;
    
    InputMapping()
        : source(InputSource::Constant)
        , target(BrushProperty::Size)
        , minOutput(0.0f)
        , maxOutput(1.0f)
        , strength(1.0f)
        , curve(CurveType::Linear)
        , inverted(false)
    {}
    
    // Apply the mapping curve to transform input [0,1] to output [minOutput, maxOutput]
    float apply(float inputValue) const {
        if (inverted) {
            inputValue = 1.0f - inputValue;
        }
        
        float curvedValue = inputValue;
        switch (curve) {
            case CurveType::Linear:
                curvedValue = inputValue;
                break;
            case CurveType::Quadratic:
                curvedValue = inputValue * inputValue;
                break;
            case CurveType::Cubic:
                curvedValue = inputValue * inputValue * inputValue;
                break;
            case CurveType::Custom:
                // TODO: Implement custom curve evaluation
                curvedValue = inputValue;
                break;
        }
        
        // Interpolate between min and max
        float result = minOutput + (maxOutput - minOutput) * curvedValue;
        
        // Apply strength (blend with base value)
        float baseValue = (minOutput + maxOutput) * 0.5f;
        return baseValue + (result - baseValue) * strength;
    }
};

} // namespace Acute


