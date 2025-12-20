# PEW|FORMER Resource Optimization Guide

## Overview
This document provides recommendations for optimizing RAM usage in the PEW|FORMER firmware without removing major features. Based on the resource analysis, optimization is needed to address the current memory pressure with only ~18KB of free SRAM remaining.

## Current Memory Situation
- **Total SRAM**: 128 KB
- **Current usage**: ~110KB out of 128KB (86%)
- **Free SRAM**: ~18KB remaining (concerning)
- **Free CCMRAM**: ~19KB remaining
- **Flash utilization**: 41% (~418KB out of 1024KB), good headroom

## RAM Optimization Recommendations

### 1. Data Structure Optimization

#### Bit Packing
- Pack boolean flags and small integer values into bitfields to reduce memory usage
- Optimize sequence step structures by packing multiple flags into single bytes
- Example: Instead of separate bools for gate, slide, etc., use bitfields in NoteSequence steps

#### Reduced Precision Data Types
- Use int8_t or int16_t instead of int32_t where possible for parameters with smaller ranges
- Consider 16-bit floats for parameters that don't require full 32-bit precision
- Optimize CurveSequence step data from 3 bytes to 2 bytes per step where possible

### 2. Dynamic Memory Allocation

#### Lazy Loading
- Load project data only when needed rather than keeping everything in memory
- Implement a page-in/page-out system for sequences not currently in use
- Only keep active sequences and engines in RAM, swap others to flash when possible

#### Memory Pooling
- Implement custom memory allocators for frequently used objects
- Use object pooling for temporary objects that are created/destroyed frequently
- Reuse memory regions instead of allocating/deallocating repeatedly

### 3. Shared Resources

#### Parameter Sharing
- Share common parameters and settings arrays across similar track types
- Use common lookup tables or reference the same memory for similar functionality
- Share routing state between similar track types

#### Scale Optimization
- Instead of storing full scale data per track, only store differences from project scale
- Implement scale data compression techniques

### 4. Memory Layout Optimization

#### CCMRAM Utilization
- Move frequently accessed data structures to CCMRAM for better performance
- Prioritize critical real-time data in CCMRAM
- Move sequence data that's currently in SRAM to CCMRAM where possible

#### Memory Compaction
- Eliminate padding in data structures
- Reorder struct members to minimize padding
- Use `#pragma pack` or equivalent to reduce struct size

### 5. Compression Techniques

#### Parameter Compression
- Compress sequence data when stored in memory
- Use delta encoding for similar consecutive values
- Implement run-length encoding for sequences with repeated values

#### Lookup Table Optimization
- Use smaller lookup tables with interpolation where appropriate
- Share common lookup tables between engines

### 6. Code-Level Optimizations

#### Engine State Reduction
- Minimize the state stored in track engines by calculating values on demand
- Remove unnecessary cached values in engine objects
- Optimize temporary variables and local storage in engine processing

#### Serialization Improvements
- Implement more efficient serialization formats
- Use variable-length encoding instead of fixed-width where possible

## Implementation Priorities

### Phase 1: High Impact, Low Risk
1. **Bit packing and struct layout optimization**
   - Estimated savings: 10-15%
   - Risk level: Low
   - Difficulty: Medium

2. **Eliminate padding in data structures**
   - Estimated savings: 5-10%
   - Risk level: Low
   - Difficulty: Low

### Phase 2: Medium Impact, Medium Risk
1. **Parameter sharing**
   - Estimated savings: 15-20%
   - Risk level: Medium
   - Difficulty: High

2. **Memory pooling**
   - Estimated savings: 5-10%
   - Risk level: Medium
   - Difficulty: High

### Phase 3: High Impact, High Risk
1. **Dynamic loading and compression**
   - Estimated savings: 20-30%
   - Risk level: High
   - Difficulty: Very High

## Specific Technical Approaches

### For NoteSequence/CurveSequence Optimization:
- Steps are already bit-packed into two 32-bit words (gate/slide/probabilities/note/etc.). No easy headroom left without changing semantics.
- High-impact lever is multiplicity: 17 patterns per track dominates SRAM. Consider shared/lazy snapshots or trimming pattern count to save tens of KB.
- If acceptable, store per-track snapshots on demand or share a single snapshot buffer across track types.

### For Routing System:
- Routes still store float min/max and per-track arrays; pack min/max to fixed-point (int16) and pack per-track shaper/bias/depth/crease to tighter types to shave a few KB (routing table ~4.5 KB).
- Move routing/shaper state to CCM where possible.

### For Track Engines:
- Optimize the length threshold cache in DiscreteMapTrackEngine (currently 32 floats = 128 bytes)
- Reduce temporary storage in engine tick/update methods
- Remove unnecessary duplicate state data

### UI/Staging and Buffers:
- Routing bias/shaper overlay uses staging/clipboard arrays; keep them int8/enums and consider placing in CCM.
- Audit other page caches/list models for static buffers and move non-DMA data to CCM to free SRAM.

### Task Stacks:
- Profile Engine/UI stacks (4 KB each by default); reduce if headroom exists to reclaim SRAM.

## Memory Usage Before/After Projections

### Current Memory Usage (All Note Tracks - Maximum):
- Sequences: 87,040 bytes
- Engines: ~4,800 bytes (8 tracks × ~600 bytes)
- System overhead: ~25,000 bytes
- **Total: ~116,840 bytes**

### Projected After Optimization (Target Range):
- With 10-15% reduction: ~103,000 bytes (14KB+ freed)
- With 20-25% reduction: ~90,000 bytes (25KB+ freed)

## Implementation Guidelines

### Testing Strategy
1. Unit tests for each optimization
2. Memory footprint verification after each change
3. Performance impact assessment
4. Regression testing for all features

### Risk Mitigation
1. Small, incremental changes
2. Comprehensive testing after each optimization
3. Revert capability maintained
4. Performance monitoring during development

## Conclusion

These optimization strategies will help maintain all major features while reducing overall memory usage, providing more headroom for future development without compromising functionality. The recommended approach is to implement optimizations in phases, starting with low-risk, high-impact options first.
