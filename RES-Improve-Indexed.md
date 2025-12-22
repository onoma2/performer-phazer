# Indexed Track System - Optimization and Improvement Analysis

Analysis of IndexedTrackEngine, IndexedSequence, and routing system for performance, memory, and code quality improvements.

## Performance Optimizations

### 1. IndexedTrackEngine::tick() - Reduce Redundant Calculations

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp:43`

**Current Issue**:
```cpp
// CURRENT: Retrieves sequence every tick
void IndexedTrackEngine::tick(uint32_t tick) {
    _sequence = &_indexedTrack.sequence(pattern());  // Line 44
```

The sequence pointer is retrieved on every tick, even when the pattern hasn't changed.

**Optimization**:
```cpp
// Only update sequence when pattern actually changes
void IndexedTrackEngine::tick(uint32_t tick) {
    if (_currentPattern != pattern()) {
        _sequence = &_indexedTrack.sequence(pattern());
        _currentPattern = pattern();
    }
```

**Impact**: Eliminates redundant pointer lookups on every tick cycle.

---

### 2. Route Modulation - Cache Frequently Used Values

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp:156-276`

**Current Issue**:
`triggerStep()` recalculates route configuration checks on every step trigger:
- Lines 165-167: RouteConfig references retrieved repeatedly
- Lines 169-174: `matchesGroup` lambda created every call
- Lines 176-178: Combine mode checks repeated

**Optimization**:
Pre-calculate and cache these values when routes change:

```cpp
struct CachedRouteState {
    bool combineRoutes;
    bool routeAActive;
    bool routeBActive;
    ModTarget sharedTarget;
    // Update only when route config changes
};
```

**Impact**: Reduces lambda creation and repeated conditional evaluations per step.

---

### 3. Zero-Duration Step Handling

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp:89-118`

**Current Issue**:
Uses `goto STEP_BEGIN` for zero-duration steps, which can iterate up to `maxSkips` times. This is a potential performance bottleneck when multiple consecutive steps have zero duration.

**Optimization**:
Pre-compute the next non-zero duration step:

```cpp
// Cache next active step index during primeNextStep()
int _nextActiveStepIndex;

void primeNextStep() {
    // Find next step with non-zero duration
    _nextActiveStepIndex = _currentStepIndex;
    int attempts = 0;
    while (attempts < _sequence->activeLength() &&
           _sequence->step(_nextActiveStepIndex).duration() == 0) {
        _nextActiveStepIndex = (_nextActiveStepIndex + 1) % _sequence->activeLength();
        attempts++;
    }
}
```

**Impact**: Eliminates goto-based loop iteration, reduces tick() execution time.

---

## Memory Optimizations

### 4. IndexedSequence::Step Bit Packing

**Location**: `src/apps/sequencer/model/IndexedSequence.h:34-106`

**Current Structure**:
- Uses 32 bits for packed data + 8 bits for group mask = **40 bits per step (5 bytes)**
- Note index: 7 bits (-63 to +64)
- Duration: 16 bits (0-65535)
- Gate length: 9 bits (0-101)
- Group mask: 4 bits actually used, but stored in 8-bit byte

**Optimization**:
Pack group mask into remaining bits in `_packed`:

```cpp
// OPTION 1: Use 64-bit packing with future expansion room
uint64_t _packed;

// bits 0-6:   note_index (7 bits)
// bits 7-22:  duration (16 bits)
// bits 23-29: gate_length (7 bits = 0-127, enough for 0-101)
// bits 30-33: group_mask (4 bits)
// bits 34-63: reserved for future features (30 bits)

// OPTION 2: Stay at 32-bit and reduce duration precision
uint32_t _packed;

// bits 0-6:   note_index (7 bits)
// bits 7-21:  duration (15 bits = 0-32767, still plenty)
// bits 22-28: gate_length (7 bits = 0-127)
// bits 29-32: group_mask (4 bits)
```

**Impact**:
- Save 1 byte per step × 32 steps × 8 patterns = **256 bytes per track**
- With 8 tracks = **2KB saved total**

---

### 5. RouteConfig Storage Optimization

**Location**: `src/apps/sequencer/model/IndexedSequence.h:134-162`

**Current Structure**:
- `float amount` = 4 bytes (range: -200% to +200%)
- `bool enabled` = 1 byte
- Total: ~7 bytes per RouteConfig (with padding)

**Optimization**:
Use fixed-point for amount:

```cpp
struct RouteConfig {
    uint8_t targetGroups;          // 1 byte
    ModTarget targetParam;          // 1 byte (enum)
    int16_t amount;                 // 2 bytes (-200.00 to +200.00 in 0.01 steps)
    bool enabled;                   // 1 byte
    // Total: 5 bytes (was 7+ bytes)
};

// Helper functions
float getAmountFloat() const { return amount * 0.01f; }
void setAmountFloat(float f) { amount = static_cast<int16_t>(f * 100.0f); }
```

**Impact**: Reduces from ~7 bytes to 5 bytes per route config, 2 routes per sequence.

---

## Code Quality Improvements

### 6. Magic Numbers - Extract Constants

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp`

**Issues**:
- Line 255: `gateTicks = 3` - hardcoded trigger pulse duration
- Line 213, 222: Hardcoded range limits (65535, 100, etc.)

**Improvement**:
Define constants in class:

```cpp
class IndexedTrackEngine {
    static constexpr uint32_t TRIGGER_PULSE_TICKS = 3;
    static constexpr uint16_t MAX_DURATION = 65535;
    static constexpr uint16_t MAX_GATE_PERCENT = 100;
    static constexpr int8_t MIN_NOTE_INDEX = -63;
    static constexpr int8_t MAX_NOTE_INDEX = 64;

    // Usage:
    gateTicks = TRIGGER_PULSE_TICKS;
    baseNote = clamp(newNote, MIN_NOTE_INDEX, MAX_NOTE_INDEX);
};
```

**Impact**: Improves readability, maintainability, and allows tuning from single location.

---

### 7. Duplicate Code in Modulation Logic

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp:206-250 vs 296-341`

**Issue**:
The modulation logic is split and duplicated:
1. Combined route mode (lines 206-236)
2. Sequential route mode (lines 239-249)
3. `applyModulation()` function (lines 296-341)

**Improvement**:
Consolidate into a single, clearer function:

```cpp
struct ModulationContext {
    float cvA, cvB;
    const IndexedSequence::RouteConfig &routeA, &routeB;
    bool aActive, bActive;
    bool combine;
    IndexedSequence::RouteCombineMode combineMode;
};

void applyModulationToStep(
    const ModulationContext &ctx,
    uint16_t &duration,
    uint16_t &gate,
    int8_t &note
) {
    // Unified modulation logic for all cases
    for (auto target : {ModTarget::Duration, ModTarget::GateLength, ModTarget::NoteIndex}) {
        if (ctx.routeA.targetParam == target || ctx.routeB.targetParam == target) {
            float finalMod = calculateCombinedModulation(ctx, target);
            applyToParameter(target, finalMod, duration, gate, note);
        }
    }
}
```

**Impact**: Reduces code duplication, easier to maintain and extend.

---

### 8. Route Modulation Value Range Documentation

**Location**: `src/apps/sequencer/model/IndexedSequence.cpp:19`

**Issue**:
Line 19 shows `floatValue * 0.01f` which suggests values come in as -100 to +100, but this normalization and expected range is not documented.

**Improvement**:
Document and validate the expected range:

```cpp
void IndexedSequence::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::IndexedA:
        // Expected range: -100.0 to +100.0 (from routing engine)
        // Normalize to -1.0 to +1.0 for modulation calculations
        _routedIndexedA = clamp(floatValue * 0.01f, -1.0f, 1.0f);
        break;
    case Routing::Target::IndexedB:
        // Expected range: -100.0 to +100.0 (from routing engine)
        // Normalize to -1.0 to +1.0 for modulation calculations
        _routedIndexedB = clamp(floatValue * 0.01f, -1.0f, 1.0f);
        break;
    // ...
    }
}
```

**Impact**: Prevents invalid values, documents interface contract.

---

## Feature Improvements

### 9. Group Routing - Add "None" Option

**Current**:
Groups can target "ALL" (0), "UNGROUPED" (0x10), or specific groups A-D (bits 0-3).

**Enhancement**:
Add a "NONE" target for routes that should be manually controlled (not step-triggered):

```cpp
static constexpr uint8_t TargetGroupsAll = 0;
static constexpr uint8_t TargetGroupsUngrouped = 0x10;
static constexpr uint8_t TargetGroupsNone = 0x20;  // NEW: Manual control only
```

**Use Case**: Routes that modulate sequence-level parameters (tempo, scale) without being tied to step groups.

---

### 10. Duration Transfer Mode UI Clarity

**Location**: `src/apps/sequencer/ui/pages/IndexedSequenceEditPage.cpp:376-398`

**Issue**:
Duration transfer mode redistributes duration between steps but the UI label "DURT" (line 274) is cryptic.

**Improvements**:
1. Better label: "DUR↔" or "D-XFER" or "DUR→"
2. Add visual feedback showing the target step (next step gets highlighted)
3. Show redistribution amounts in real-time
4. Consider making this a two-step selection (select source, then target step)

```cpp
// Visual enhancement
if (_durationTransfer) {
    // Draw arrow from current step to next step
    int nextIndex = (stepIndex + 1) % activeLength;
    drawTransferArrow(canvas, stepIndex, nextIndex);
}
```

---

## Architecture Improvements

### 11. Routing State Management Consolidation

**Issue**:
Route state is split across multiple locations:
- `RoutingEngine` manages global routes (RoutingEngine.cpp)
- `IndexedSequence` stores route-specific values (IndexedSequence.h:536-537)
- `IndexedTrackEngine` applies modulation (IndexedTrackEngine.cpp)

**Improvement**:
Consider a unified routing state manager:

```cpp
class TrackRoutingState {
private:
    IndexedSequence *_sequence;
    float _routedA, _routedB;

public:
    void update(const RoutingEngine &engine, int trackIndex);
    void applyToStep(IndexedSequence::Step &step,
                     const IndexedSequence::RouteConfig &routeA,
                     const IndexedSequence::RouteConfig &routeB);

    float routedA() const { return _routedA; }
    float routedB() const { return _routedB; }
};
```

**Impact**: Clearer separation of concerns, easier testing.

---

### 12. Step Processing Pipeline

**Location**: `src/apps/sequencer/engine/IndexedTrackEngine.cpp:156-276`

**Current Issue**:
Step triggering mixes multiple concerns:
- Route fetching
- Group matching
- Modulation application
- Gate/CV calculation

**Improvement**:
Separate into distinct pipeline stages:

```cpp
struct RawStepValues {
    uint16_t duration;
    uint16_t gateLength;
    int8_t noteIndex;
};

struct ModulatedStepValues : RawStepValues {
    uint32_t gateTicks;
    float cvOutput;
};

void triggerStep() {
    auto rawValues = getRawStepValues();
    auto modContext = buildModulationContext();
    auto modulatedValues = applyModulation(rawValues, modContext);
    setOutputs(modulatedValues);
    updateActivity();
}

RawStepValues getRawStepValues() const {
    const auto &step = _sequence->step(effectiveStepIndex());
    return {step.duration(), step.gateLength(), step.noteIndex()};
}

ModulationContext buildModulationContext() const {
    return {
        .cvA = _sequence->routedIndexedA(),
        .cvB = _sequence->routedIndexedB(),
        .routeA = _sequence->routeA(),
        .routeB = _sequence->routeB(),
        // ...
    };
}
```

**Impact**: Easier to test individual stages, clearer code flow.

---

### 13. Error Handling for Invalid States

**Missing**:
No bounds checking or error recovery for:
- Invalid step indices
- Corrupted sequence data
- Overflow in duration calculations
- Null sequence pointer

**Improvement**:
Add defensive programming:

```cpp
void IndexedTrackEngine::triggerStep() {
    // Defensive checks
    if (!_sequence) {
        DBG("ERROR: Null sequence pointer in triggerStep()");
        _cvOutput = 0.0f;
        _gateTimer = 0;
        _effectiveStepDuration = 0;
        return;
    }

    if (_currentStepIndex >= _sequence->activeLength()) {
        DBG("ERROR: Step index %d out of bounds (length: %d)",
            _currentStepIndex, _sequence->activeLength());
        _currentStepIndex = 0;  // Safe fallback
    }

    int effectiveStepIndex = (_currentStepIndex + _sequence->firstStep()) % _sequence->activeLength();
    if (effectiveStepIndex < 0 || effectiveStepIndex >= IndexedSequence::MaxSteps) {
        DBG("ERROR: Effective step index %d invalid", effectiveStepIndex);
        effectiveStepIndex = 0;
    }

    // ... rest of function
}
```

**Impact**: Prevents crashes from corrupted data or edge cases.

---

## UI/UX Improvements

### 14. Timeline Visualization Enhancement

**Location**: `src/apps/sequencer/ui/pages/IndexedSequenceEditPage.cpp:66-121`

**Issue**:
Fixed minimum step width of 7 pixels (line 83) makes long sequences hard to view.

**Enhancement**:
Add zoom levels or scrolling for the timeline bar:

```cpp
enum class TimelineZoom {
    Fit,      // All steps visible (current behavior)
    x2,       // 2x zoom
    x4,       // 4x zoom
    Scroll    // Free scroll mode
};

void drawTimeline(Canvas &canvas) {
    switch (_timelineZoom) {
    case TimelineZoom::Fit:
        drawTimelineFit(canvas);  // Current implementation
        break;
    case TimelineZoom::Scroll:
        drawTimelineScrollable(canvas, _scrollOffset);
        break;
    // ...
    }
}
```

**UI**:
- SHIFT + Left/Right arrows to adjust zoom
- Encoder to scroll when zoomed

---

### 15. Route Configuration Discoverability

**Location**: `src/apps/sequencer/ui/pages/IndexedSequenceEditPage.cpp:349`

**Current**:
Routes are accessed via SHIFT+F5, which is not discoverable.

**Enhancements**:

1. **Visual indicators when routes are active**:
```cpp
// In draw() function
if (_sequence->routeA().enabled || _sequence->routeB().enabled) {
    canvas.setFont(Font::Tiny);
    canvas.setColor(Color::Bright);
    canvas.drawText(230, 2, "R");  // Route active indicator
}
```

2. **Display route modulation amounts in real-time**:
```cpp
// Show modulation depth per step
for (int i = 0; i < activeLength; ++i) {
    if (stepMatchesRouteGroups(i)) {
        float modAmount = calculateModulationForStep(i);
        // Draw modulation indicator (bar, color, etc.)
    }
}
```

3. **Add route enable/disable toggle in main edit view**:
- Long-press F5 to quick-toggle routes without entering config page

---

## Priority Recommendations

### High Priority (Performance)
1. **Cache sequence pointer** (§1) - Simple change, immediate benefit
2. **Optimize zero-duration step handling** (§3) - Eliminates goto loop
3. **Pre-calculate route configurations** (§2) - Reduces per-step overhead

**Estimated Impact**: 10-20% reduction in tick() execution time

---

### Medium Priority (Memory)
4. **Optimize Step bit packing** (§4) - 256 bytes saved per track
5. **Use fixed-point for route amounts** (§5) - Minor memory savings

**Estimated Impact**: ~2KB RAM savings total

---

### Low Priority (Code Quality)
6. **Extract magic numbers** (§6) - Maintainability
7. **Consolidate modulation code** (§7) - Reduces duplication
8. **Improve error handling** (§13) - Robustness

**Estimated Impact**: Better maintainability, fewer bugs

---

### Future Enhancements
9. **Group routing "None" option** (§9) - New feature
10. **Duration transfer UI improvements** (§10) - Better UX
11. **Routing state consolidation** (§11) - Architecture cleanup
12. **Step processing pipeline** (§12) - Testability
14. **Timeline visualization** (§14) - Better workflow
15. **Route discoverability** (§15) - Improved UX

---

## Implementation Notes

### Testing Strategy
1. Create unit tests for optimized bit-packing (TestIndexedSequence.cpp)
2. Add performance benchmarks for tick() timing
3. Test zero-duration sequences thoroughly
4. Validate route modulation accuracy after fixed-point conversion

### Backward Compatibility
- Bit packing changes require serialization version bump
- Fixed-point route amounts need migration code
- All changes should maintain existing project file compatibility

### Hardware Considerations
- Test timing-critical optimizations on actual STM32 hardware
- Verify that zero-duration optimization doesn't affect real-time behavior
- Profile memory usage on target hardware after optimizations

---

## Related Files

- `src/apps/sequencer/engine/IndexedTrackEngine.cpp` - Main sequencer engine
- `src/apps/sequencer/engine/IndexedTrackEngine.h` - Engine interface
- `src/apps/sequencer/model/IndexedSequence.h` - Sequence data model
- `src/apps/sequencer/model/IndexedSequence.cpp` - Sequence routing
- `src/apps/sequencer/model/IndexedTrack.h` - Track wrapper
- `src/apps/sequencer/model/IndexedTrack.cpp` - Track implementation
- `src/apps/sequencer/engine/RoutingEngine.cpp` - Global routing system
- `src/apps/sequencer/ui/pages/IndexedSequenceEditPage.cpp` - Main edit UI
- `src/apps/sequencer/ui/pages/IndexedStepsPage.cpp` - Step list UI

---

**Last Updated**: 2025-12-22
**Analysis Version**: 1.0
