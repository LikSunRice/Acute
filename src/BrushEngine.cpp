#include "BrushEngine.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>

namespace Acute {

BrushEngine::BrushEngine()
    : m_strokeActive(false)
    , m_distanceSinceLastDab(0.0f)
{
}

BrushEngine::~BrushEngine() = default;

void BrushEngine::setBrushSettings(const BrushSettings& settings) {
    m_settings = settings;
}

void BrushEngine::beginStroke() {
    m_strokeActive = true;
    m_distanceSinceLastDab = 0.0f;
    m_lastInput = InputPoint();
}

void BrushEngine::endStroke() {
    m_strokeActive = false;
}

void BrushEngine::addMapping(const InputMapping& mapping) {
    m_settings.mappings.push_back(mapping);
}

void BrushEngine::clearMappings() {
    m_settings.mappings.clear();
}

std::vector<BrushDab> BrushEngine::processInput(const InputPoint& input) {
    std::vector<BrushDab> dabs;
    
    if (!m_strokeActive) {
        return dabs;
    }
    
    // For the first point in a stroke
    if (m_lastInput.timestamp == 0) {
        BrushDab dab = generateDab(input);
        applyMappings(input, dab);
        applyScatter(dab);
        dabs.push_back(dab);
        m_lastInput = input;
        return dabs;
    }
    
    // Calculate distance from last dab
    float dx = input.x - m_lastInput.x;
    float dy = input.y - m_lastInput.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Generate a temporary dab to calculate spacing
    BrushDab tempDab = generateDab(input);
    applyMappings(input, tempDab);
    float spacing = calculateSpacing(tempDab);
    
    m_distanceSinceLastDab += distance;
    
    // Generate dabs along the path
    while (m_distanceSinceLastDab >= spacing && spacing > 0.0f) {
        // Interpolate position
        float t = (m_distanceSinceLastDab - spacing) / distance;
        t = std::max(0.0f, std::min(1.0f, t));
        
        InputPoint interpInput = input;
        interpInput.x = m_lastInput.x + dx * (1.0f - t);
        interpInput.y = m_lastInput.y + dy * (1.0f - t);
        
        // Interpolate other properties
        interpInput.pressure = m_lastInput.pressure + (input.pressure - m_lastInput.pressure) * (1.0f - t);
        interpInput.tiltX = m_lastInput.tiltX + (input.tiltX - m_lastInput.tiltX) * (1.0f - t);
        interpInput.tiltY = m_lastInput.tiltY + (input.tiltY - m_lastInput.tiltY) * (1.0f - t);
        
        BrushDab dab = generateDab(interpInput);
        applyMappings(interpInput, dab);
        applyScatter(dab);
        dabs.push_back(dab);
        
        m_distanceSinceLastDab -= spacing;
    }
    
    m_lastInput = input;
    return dabs;
}

BrushDab BrushEngine::generateDab(const InputPoint& input) {
    BrushDab dab;
    
    // Set position
    dab.x = input.x;
    dab.y = input.y;
    
    // Set base properties from settings
    dab.size = m_settings.baseSize;
    dab.opacity = m_settings.baseOpacity;
    dab.hardness = m_settings.baseHardness;
    dab.flow = m_settings.baseFlow;
    dab.rotation = m_settings.baseRotation;
    
    // Set color
    dab.r = m_settings.colorR;
    dab.g = m_settings.colorG;
    dab.b = m_settings.colorB;
    
    return dab;
}

void BrushEngine::applyMappings(const InputPoint& input, BrushDab& dab) {
    float originalSize = dab.size;
    
    // Apply each mapping
    for (const auto& mapping : m_settings.mappings) {
        float inputValue = getInputValue(input, mapping.source);
        float outputValue = mapping.apply(inputValue);
        
        // Apply to the appropriate property
        switch (mapping.target) {
            case BrushProperty::Size:
                dab.size *= outputValue;
                {
                    static int sizeDebugCount = 0;
                    if (sizeDebugCount++ % 50 == 0 && mapping.source == InputSource::Pressure) {
                        std::cout << "[Brush Engine] Size mapping - Input: " << inputValue 
                                  << ", Output: " << outputValue 
                                  << ", Original size: " << originalSize 
                                  << ", Final size: " << dab.size << std::endl;
                    }
                }
                break;
            case BrushProperty::Opacity:
                dab.opacity *= outputValue;
                break;
            case BrushProperty::Hardness:
                dab.hardness = std::max(0.0f, std::min(1.0f, outputValue));
                break;
            case BrushProperty::Flow:
                dab.flow *= outputValue;
                break;
            case BrushProperty::Rotation:
                dab.rotation += outputValue;
                break;
            case BrushProperty::Scatter:
                dab.scatter = outputValue;
                break;
            // Color properties would modify HSV and convert back to RGB
            default:
                break;
        }
    }
    
    // Clamp values
    dab.size = std::max(0.1f, dab.size);
    dab.opacity = std::max(0.0f, std::min(1.0f, dab.opacity));
    dab.flow = std::max(0.0f, std::min(1.0f, dab.flow));
}

float BrushEngine::getInputValue(const InputPoint& input, InputSource source) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    switch (source) {
        case InputSource::Pressure: {
            static int pressureDebugCount = 0;
            if (pressureDebugCount++ % 50 == 0) { // Print every 50th sample
                std::cout << "[Brush Engine] Using pressure: " << input.pressure << std::endl;
            }
            return input.pressure;
        }
        case InputSource::TiltX:
            return (input.tiltX + 1.0f) * 0.5f; // Convert from [-1,1] to [0,1]
        case InputSource::TiltY:
            return (input.tiltY + 1.0f) * 0.5f;
        case InputSource::TiltMagnitude:
            return std::min(1.0f, input.getTiltMagnitude());
        case InputSource::Speed:
            return std::min(1.0f, input.getSpeed() / 1000.0f); // Normalize speed
        case InputSource::Rotation:
            return input.rotation / 360.0f;
        case InputSource::Random:
            return dis(gen);
        case InputSource::Constant:
        default:
            return 1.0f;
    }
}

float BrushEngine::calculateSpacing(const BrushDab& dab) {
    return dab.size * m_settings.baseSpacing;
}

void BrushEngine::applyScatter(BrushDab& dab) {
    if (dab.scatter > 0.0f) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        float scatterAmount = dab.scatter * dab.size * 0.5f;
        dab.x += dis(gen) * scatterAmount;
        dab.y += dis(gen) * scatterAmount;
    }
}

} // namespace Acute


