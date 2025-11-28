#pragma once

#include <cstdint>
#include <cmath>

namespace Acute {

// Input data structure from stylus/mouse
struct InputPoint {
    float x;              // X coordinate in pixels
    float y;              // Y coordinate in pixels
    float pressure;       // 0.0 to 1.0
    float tiltX;          // Tilt angle X (-1.0 to 1.0)
    float tiltY;          // Tilt angle Y (-1.0 to 1.0)
    float rotation;       // Barrel rotation (0.0 to 360.0 degrees)
    float velocityX;      // Speed in X direction
    float velocityY;      // Speed in Y direction
    uint64_t timestamp;   // Timestamp in milliseconds
    
    InputPoint()
        : x(0.0f), y(0.0f), pressure(1.0f)
        , tiltX(0.0f), tiltY(0.0f), rotation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), timestamp(0)
    {}
    
    float getSpeed() const {
        return static_cast<float>(std::sqrt(velocityX * velocityX + velocityY * velocityY));
    }
    
    float getTiltMagnitude() const {
        return static_cast<float>(std::sqrt(tiltX * tiltX + tiltY * tiltY));
    }
};

// Input device type
enum class DeviceType {
    Mouse,
    StylusPen,
    StylusEraser,
    Touch
};

} // namespace Acute


