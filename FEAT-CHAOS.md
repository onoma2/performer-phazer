# Feature Specification: Chaotic Attractor Modulation Sources

## Overview
This document specifies the implementation of chaotic attractor-based modulation sources for the PEW|FORMER sequencer, specifically for use as LFO sources in Curve tracks.

## Motivation
Chaotic attractors provide complex, non-repeating modulation patterns that can create evolving, organic textures in electronic music. The implementation of algorithms like the Latoocarfian map (as shown in the SuperCollider example) would significantly expand the sequencer's modulation capabilities beyond traditional LFO waveforms.

## SuperCollider Algorithm Analysis
The target algorithm uses:
```
LatoocarfianC.ar(hz*10, 1, LFNoise2.kr(hz*10/20,1.5+meta,1.5+meta), 
                 LFNoise2.kr(hz*10/20,0.5+meta,0.5+meta), 
                 LFNoise2.kr(hz*10/20,0.5+meta,0.5+meta))
                 .range(minval*1.02, maxval*0.98)
```

This creates a chaotic oscillator with time-varying parameters controlled by low-frequency noise.

## Design Requirements

### 1. Mathematical Implementation
- Implement Latoocarfian chaotic map with equations:
  - x(n+1) = sin(b * y(n) + c * sin(b * x(n)))
  - y(n+1) = sin(a * x(n) + d * sin(a * y(n)))
- Parameters a, b, c, d need to be controllable via UI
- Frequency parameter (hz) needs to map to sequencer's tempo sync system

### 2. Noise Functions
- Implement low-frequency noise generators using existing `Random` class
- Create time-varying parameter controls similar to SuperCollider's LFNoise2
- Support for parameter modulation via `meta` control

### 3. Range Mapping
- Implement output range control (minval to maxval)
- Map to sequencer's existing voltage range system
- Support both unipolar and bipolar outputs

### 4. Timing System Integration
- Calculate attractor at appropriate frequency relative to master clock
- Optionally sync to tempo subdivisions
- Support for rate CV input to control attractor frequency

## Implementation Plan

### Phase 1: Core Chaotic Generator
- Create `ChaoticGenerator` class with Latoocarfian implementation
- Add parameter controls (a, b, c, d coefficients)
- Implement basic range mapping

### Phase 2: Noise System
- Implement low-frequency noise (LFNoise) functions using existing Random utility
- Create time-varying parameter generators
- Map `meta` parameter to noise depth controls

### Phase 3: CurveTrack Integration
- Add chaotic generator as modulation source option
- Extend CurveTrack model with attractor parameters
- Integrate into CurveTrackEngine update cycle

### Phase 4: UI Implementation
- Add controls to CurveSequenceEditPage
- Parameter editing for attractor coefficients
- Visualization of attractor output

## Resource Analysis

### Memory Requirements
- State variables: 8 bytes (2 floats for x,y coordinates)
- Parameters: 16 bytes (4-6 coefficient parameters)
- Noise state: 8 bytes (2 floats for LFNoise state)
- Estimated: ~32 bytes per track instance

### CPU Requirements
- Per calculation: ~500-800 CPU cycles (with FPU)
- Update frequency: Once per step to minimize CPU load
- Estimated: ~0.1% of total CPU budget at max resolution

### Flash Memory
- Core implementation: ~300-500 bytes
- UI integration: ~200-300 bytes
- Total: ~500-800 bytes

## Integration Points

### Model Layer Changes
- Extend `CurveTrack` class with attractor parameters
- Add new `AttractorMode` enum to indicate modulation source
- Add serialization support for new parameters

### Engine Layer Changes
- Modify `CurveTrackEngine::updateOutput` to optionally use attractor output
- Add calculation frequency controls
- Integrate with existing processing chain (wavefolding, filtering, etc.)

### UI Layer Changes
- Add attractor-specific controls to CurveSequenceEditPage
- Parameter editing for coefficients
- Visual feedback for attractor behavior

## Testing Considerations
- Unit tests for chaotic generator calculations
- Stability tests to ensure bounded output
- Performance tests to validate CPU usage
- Integration tests with existing CurveTrack functions

## Risk Assessment

### Technical Risks
- **CPU Overload**: Calculations could consume too many cycles if updated too frequently
- **Non-convergent Attractors**: Some parameter combinations could cause unbounded output
- **Parameter Space Complexity**: Large parameter space could be difficult to navigate

### Mitigation Strategies
- Calculate at low frequency (e.g., once per step) to preserve CPU
- Implement parameter clamping to ensure stable attractor behavior
- Provide preset parameter sets for musical utility

## Timeline Estimate
- Phase 1: 2-3 days
- Phase 2: 2-3 days  
- Phase 3: 3-4 days
- Phase 4: 2-3 days
- Testing & Integration: 2-3 days
- **Total: 11-16 days**

## Conclusion
The chaotic attractor implementation is technically feasible within the PEW|FORMER hardware constraints. The feature would add significant musical utility through complex, evolving modulation patterns while maintaining real-time performance requirements.