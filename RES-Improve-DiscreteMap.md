# Discrete Map Track System - Optimization and Improvement Analysis

Analysis of DiscreteMapTrackEngine, DiscreteMapSequence, and associated structures for performance, memory, and code quality improvements.

## Performance Optimizations

### 1. Threshold Cache Invalidation Optimization

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:168-171`

**Current Issue**:
```cpp
    // Invalidate threshold cache if voltage window is being modulated
    if (_sequence->isRouted(Routing::Target::DiscreteMapRangeHigh) ||
        _sequence->isRouted(Routing::Target::DiscreteMapRangeLow)) {
        _thresholdsDirty = true;
    }
```
The engine checks routing flags on every tick to see if range parameters are modulated. If they are, it marks thresholds dirty, forcing a full recalculation of length thresholds (if in Length mode) later in the tick.

**Optimization**:
Only mark thresholds dirty if the *actual values* of RangeHigh or RangeLow have changed, or if the routing modulation amount has changed. Currently, `isRouted` just returns true if a route exists, not if the value changed.

A better approach might be to cache the previous range values:
```cpp
float currentRangeHigh = _sequence->rangeHigh();
float currentRangeLow = _sequence->rangeLow();

if (currentRangeHigh != _prevRangeHigh || currentRangeLow != _prevRangeLow) {
    _thresholdsDirty = true;
    _prevRangeHigh = currentRangeHigh;
    _prevRangeLow = currentRangeLow;
}
```
This avoids recalculation when the route exists but the source CV isn't changing (e.g. constant CV).

**Impact**: Reduces CPU load when routing is active but values are stable.

---

### 2. Stage Crossing Scan Optimization

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:435-460`

**Current Issue**:
`findActiveStage` iterates through all 32 stages (or `StageCount`) linearly on every tick where a crossing needs to be checked.
```cpp
    for (int i = 0; i < DiscreteMapSequence::StageCount; i++) {
        // ... getThresholdVoltage(i) ... checks ...
    }
```
`getThresholdVoltage` involves calculation (especially in Position mode mapping).

**Optimization**:
1.  **Sorted Thresholds**: If stages were sorted by voltage, we could use binary search or early exit. However, user defines arbitrary thresholds, so sorting isn't guaranteed physically.
2.  **Active Region**: Since we know `prevInput` and `input`, we only care about thresholds `T` where `min(prev, curr) <= T <= max(prev, curr)`.
3.  **Pre-calculated Voltages**: Cache the voltage values for all stages in `update()` or when parameters change, instead of calculating them inside the loop via `getThresholdVoltage`.

**Impact**: Faster `tick()` execution, especially with many stages.

---

### 3. Ramp Update Optimization

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:387-417`

**Current Issue**:
Ramp calculations involve division and float arithmetic every tick.
```cpp
    _rampPhase = float(posInPeriod) / float(periodTicks);
```

**Optimization**:
Use a reciprocal for `periodTicks` if it doesn't change often.
```cpp
    if (periodTicks != _cachedPeriodTicks) {
        _periodTicksInv = 1.0f / float(periodTicks);
        _cachedPeriodTicks = periodTicks;
    }
    _rampPhase = float(posInPeriod) * _periodTicksInv;
```
Division is expensive on some MCUs (though STM32F4 has FPU, division is still slower than multiplication).

---

## Memory Optimizations

### 4. DiscreteMapSequence::Stage Bit Packing

**Location**: `src/apps/sequencer/model/DiscreteMapSequence.h` (inferred)

**Current Structure**:
Likely uses standard types:
- `int8_t _threshold` (1 byte)
- `TriggerDir _direction` (1 byte / enum)
- `int8_t _noteIndex` (1 byte)
- Padding likely aligns this to 4 bytes.

**Optimization**:
Pack into 32 bits if needed, but current usage is likely efficient enough (3 bytes actual data).
If we have 32 stages, saving 1 byte per stage = 32 bytes per sequence. Not huge, but tidy.

**Impact**: Low priority.

---

## Code Quality Improvements

### 5. Magic Numbers & Hardcoded Ranges

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp`

**Issues**:
- Line 399, 400: `-5.0f` and `5.0f` hardcoded as internal ramp range.
- Line 119: `-5.001f` initialization.
- Line 427: `200.0f` normalization factor.

**Improvement**:
Define constants:
```cpp
static constexpr float INTERNAL_RAMP_MIN = -5.0f;
static constexpr float INTERNAL_RAMP_MAX = 5.0f;
static constexpr float THRESHOLD_RANGE = 200.0f; // -100 to +100
```

**Impact**: Improved readability and easier tuning of voltage ranges.

### 6. Logic Separation in tick()

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:160-377`

**Issue**:
`tick()` is very long and handles:
1. Loop logic
2. Sync/Reset logic
3. Input source update (Ramp vs External)
4. External "Once" mode logic
5. Scanner logic
6. Threshold invalidation
7. Stage crossing detection
8. Gate/CV Output generation

**Improvement**:
Refactor into helper methods:
- `processSync(tick)`
- `updateInputSource(relativeTick)`
- `processExternalOnceMode()`
- `processScanner()`
- `determineActiveStage()`
- `updateOutputs()`

**Impact**: Much better readability and easier unit testing of individual components.

---

## Architecture Improvements

### 7. CvUpdateMode Logic Clarification

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:335-344`

**Current**:
Complex boolean logic for `shouldOutputCv`:
```cpp
    bool muteCondition = !mute() || _discreteMapTrack.cvUpdateMode() == DiscreteMapTrack::CvUpdateMode::Always;
    bool gateCondition = (_activeStage >= 0) || _discreteMapTrack.cvUpdateMode() == DiscreteMapTrack::CvUpdateMode::Always;
    shouldOutputCv = muteCondition && gateCondition;
```

**Improvement**:
Simplify logic. `Always` mode seems to override everything.
```cpp
    bool alwaysMode = (_discreteMapTrack.cvUpdateMode() == DiscreteMapTrack::CvUpdateMode::Always);
    bool gateActive = (_activeStage >= 0);
    bool notMuted = !mute();

    if (alwaysMode) {
        shouldOutputCv = true; // Always output, even if muted? (Check requirements)
        // If muted means "0V output" even in Always mode, logic needs adjustment.
    } else {
        shouldOutputCv = notMuted && gateActive;
    }
```
*Note*: Current code outputs 0V if muted in Gate mode, but output follows curve if muted in Always mode? Logic at line 348 (`_targetCv = 0.0f`) handles the "no stage active" case for Always mode.

### 8. External "Once" Mode State Machine

**Location**: `src/apps/sequencer/engine/DiscreteMapTrackEngine.cpp:227-268`

**Issue**:
The state machine for External Once mode (`_extOnceArmed`, `_extOnceDone`, `_extMinSeen`, etc.) is embedded in `tick()`.

**Improvement**:
Encapsulate this logic into a small state helper class or struct within the engine.
```cpp
struct ExternalSweepTracker {
    bool armed;
    bool done;
    float minSeen, maxSeen;
    void reset() { ... }
    bool update(float input, float min, float max, float span);
};
```

**Impact**: Cleaner `tick()` method.

---

## UI/UX Improvements (Inferred from Engine)

### 9. Scanner Visualization

**Idea**:
The `Scanner` feature maps a 0-34 value to stage selection. It implies a "playhead" moving through stages based on a knob/CV.
Providing visual feedback on the UI (highlighting the scanned stage) would be valuable if not already present.

### 10. Threshold Visualization

**Idea**:
Since `getThresholdVoltage` knows the exact voltage crossing points, the UI could render these as ticks on a voltage ruler, helping users visualize where triggers will happen relative to the input CV.

---

## Priority Recommendations

1.  **Refactor `tick()`**: Break down the monolithic function. (High value for maintainability)
2.  **Optimize Threshold Invalidation**: Fix the `isRouted` check to avoid redundant recalculations. (Performance)
3.  **Encapsulate External Sweep Logic**: Clean up the "Once" mode implementation. (Code Quality)
4.  **Define Constants**: Remove magic numbers for voltages and ranges. (Code Quality)

