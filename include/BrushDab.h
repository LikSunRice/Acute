#pragma once

namespace Acute {

// Represents a single brush dab (stamp) to be rendered
struct BrushDab {
    float x;              // Position X
    float y;              // Position Y
    float size;           // Size in pixels
    float opacity;        // 0.0 to 1.0
    float rotation;       // Rotation in degrees
    float hardness;       // Edge hardness 0.0 to 1.0
    float flow;           // Flow/strength 0.0 to 1.0
    float scatter;        // Random scatter amount
    
    // Color (RGB)
    float r, g, b;
    
    BrushDab()
        : x(0.0f), y(0.0f), size(10.0f)
        , opacity(1.0f), rotation(0.0f)
        , hardness(0.5f), flow(1.0f), scatter(0.0f)
        , r(0.0f), g(0.0f), b(0.0f)
    {}
};

} // namespace Acute


