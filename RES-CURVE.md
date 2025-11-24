# CurveTrack Analysis and Enhancements for PEW|FORMER

## Overview

This document provides a comprehensive analysis of the CurveTrack implementation in the PEW|FORMER sequencer firmware, including findings and proposed enhancements for LFO shape generation and phasing capabilities.

## Current CurveTrack Implementation

### Architecture
- **Component**: `CurveTrack` (model) with `CurveTrackEngine` (engine)
- **Purpose**: Creates CV curves with various shapes and modulations
- **Structure**: Contains multiple `CurveSequence` patterns with up to 64 steps each

### Available Curve Shapes
The system provides 25 different curve functions:

- **Basic**: Low (0), High (1)
- **Ramps**: rampUp, rampDown, rampUpHalf, rampDownHalf
- **Exponential**: expUp, expDown, expUpHalf, expDownHalf, expDown2x/3x/4x
- **Logarithmic**: logUp, logDown, logUpHalf, logDownHalf
- **Smooth**: smoothUp, smoothDown, smoothUpHalf, smoothDownHalf
- **Waveforms**: triangle (18), bell (19 - sine-like), stepUp (20), stepDown (21)

### Parameters per Track
- `playMode`: Aligned or Free playback
- `fillMode`: How to fill during chain fills
- `muteMode`: How to handle mutes (Last Value, 0V, Min, Max)
- `slideTime`: Slide time percentage (0-100%)
- `offset`: Voltage offset (-5V to +5V in 0.01V steps)
- `rotate`: Step rotation (-64 to +64 steps)
- `shapeProbabilityBias`: Bias for shape variation (-8 to +8)
- `gateProbabilityBias`: Bias for gate probability
- `sequences`: Multiple pattern sequences

### Parameters per Step
- `shape`: Primary curve shape (0-24, maps to Curve::Type)
- `shapeVariation`: Alternative curve shape for variation
- `shapeVariationProbability`: Probability of using variation (0-8)
- `min`: Minimum output value (0-255, normalized 0.0-1.0)
- `max`: Maximum output value (0-255, normalized 0.0-1.0)
- `gate`: Gate pattern (4 segments per step)
- `gateProbability`: Probability of gate firing (-1 to 7)

### Technical Implementation Details
- Uses bitfield packing for efficient memory usage
- Step data is packed into 32+16 bits per step
- Rotation implemented in `SequenceUtils::rotateStep()`
- Curve evaluation uses function pointers from the Curve class
- Supports routing for CV control of parameters

## Enhancement 1: LFO Shape Population Functions

### Objective
Create functions to populate CurveSequence steps with standard LFO waveforms:
- Sine wave approximation
- Sawtooth wave
- Triangle wave
- Square wave

### Implementation
The existing `bell` curve (index 19) provides a sine-like shape: `0.5 - 0.5 * cos(x * 2π)`
- Triangle: Use existing `triangle` curve (index 18)
- Sawtooth: Use `rampUp` (index 2) for ascending, `rampDown` (index 3) for descending
- Square: Use `stepUp` (index 20) for rising edge at middle, `stepDown` (index 21) for falling edge

### Function Example
```cpp
static void populateLfoShapes(CurveSequence &sequence, Curve::Type shapeType) {
    for (int i = 0; i < CONFIG_STEP_COUNT; i++) {
        auto &step = sequence.step(i);
        step.setShape(shapeType);
        step.setMin(0);  // Minimum value for the CV range
        step.setMax(CurveSequence::Max::Max);  // Maximum value for the CV range
        step.setGate(0);  // No gate output
    }
}
```

### Resource Requirements
- **RAM Impact**: ~30 bytes (stack space)
- **Processing Overhead**: ~6,400-9,600 cycles for full sequence population (non real-time)
- **Flash Memory**: ~800-1,300 bytes
- **Audio Performance Impact**: None during normal operation

## Enhancement 2: System Tick Granularity Phasing

### Objective
Add phase control with system tick granularity for true LFO phasing effects.

### Current Limitations
The current `rotate` property (-64 to +64) shifts the entire sequence by step positions but operates at the step level, not system tick level.

### Proposed Enhancement
Add a phase parameter to allow for fine-grained phase adjustment within a single step's duration.
- New phase parameter: 0.0 to 1.0 representing fractional position within the step
- Modify the `updateOutput` function to apply this phase offset when evaluating the curve

### Implementation
```cpp
// In updateOutput function instead of just using _currentStepFraction
float phaseAdjustedFraction = fmod(_currentStepFraction + phaseOffset, 1.0f);
if (phaseAdjustedFraction < 0.0f) phaseAdjustedFraction += 1.0f;
```

### Resource Requirements
- **RAM Impact**: ~10-12 bytes per CurveTrack (~80-96 bytes total for 8 tracks)
- **Processing Overhead**: ~130-160 cycles per step update (real-time)
- **Flash Memory**: ~900-1,300 bytes
- **Audio Performance Impact**: Minimal, adds ~0.01-0.02% CPU load

## Combined Resource Impact

### Memory Usage
- **Additional RAM**: ~110-126 bytes total
- **Additional Flash**: ~1,700-2,600 bytes

### Performance Impact
- **CPU cycles**: ~130-160 additional per step update for phasing
- **System tick overhead**: ~50-80 cycles per tick (1kHz)
- **Audio impact**: Negligible with proper implementation

## STM32F4 Resource Assessment

### Current System Constraints
- STM32F4 (ARM Cortex-M4, 168MHz)
- Limited RAM and flash memory
- Real-time audio processing requirements

### Assessment
Both enhancements are extremely lightweight:
- Memory impact is negligible compared to available resources
- CPU overhead is minimal and well within processing capacity
- No significant impact on audio performance
- Compatible with existing timing constraints

## Recommended Implementation Order

1. **LFO shape population function**: Easiest to implement, no performance impact
2. **Phasing capability**: More complex but still lightweight and valuable for LFO-style applications

## Benefits

1. **LFO Generation**: Enables the CurveTrack to function effectively as an LFO with classic waveforms
2. **Phasing Effects**: Provides sophisticated phasing capabilities for creating evolving textures
3. **Modular Design**: Both enhancements integrate seamlessly with existing architecture
4. **Resource Efficient**: Minimal impact on STM32F4 resources
5. **Eurorack Compatibility**: Appropriate for CV control applications