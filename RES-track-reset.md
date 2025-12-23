# Per-Track Reset Routing Target Implementation Plan

## Overview

Add a per-track **Reset** routing target that triggers `trackEngine->reset()` on rising edge of the source CV signal. This allows external CV sources, MIDI triggers, or other tracks to reset individual tracks back to their initial state.

## Architecture Overview

Reset will be a **Track-level target** but handled specially in **RoutingEngine** (engine layer) rather than Routing (model layer), because it needs access to `trackEngine->reset()`.

## Implementation Steps

### 1. Add Reset to Routing::Target enum (Routing.h)

**File**: `src/apps/sequencer/model/Routing.h`
**Location**: Lines 54-69 (Track targets section)

```cpp
// Track targets
TrackFirst,
Run = TrackFirst,
Reset,          // NEW - triggers trackEngine->reset() on rising edge
SlideTime,
// ... rest unchanged
```

**Rationale**: Place Reset right after Run since they're both execution control targets.

---

### 2. Add target name mapping (Routing.cpp)

**File**: `src/apps/sequencer/model/Routing.cpp`

**Location 1**: `targetName()` function (~line 128)
```cpp
case Target::Run: return "Run";
case Target::Reset: return "Reset";    // NEW
case Target::SlideTime: return "Slide Time";
```

**Location 2**: `printTargetValue()` function (~line 488)
```cpp
case Target::Run:
case Target::Reset:    // NEW - same formatting as Run
    str(intValue ? "on" : "off");
    break;
```

**Rationale**: Reset displays as "on"/"off" like Run (threshold-based).

---

### 3. Add edge detection state (RoutingEngine.h)

**File**: `src/apps/sequencer/engine/RoutingEngine.h`
**Location**: After lines 64-65 (where _lastPlayToggleActive is defined)

```cpp
uint8_t _lastPlayToggleActive = false;
uint8_t _lastRecordToggleActive = false;
std::array<uint8_t, CONFIG_TRACK_COUNT> _lastResetActive;  // NEW - per-track reset state
```

**Rationale**: Need per-track state (8 tracks) since Reset is track-specific, unlike PlayToggle which is global.

---

### 4. Initialize reset state in constructor (RoutingEngine.cpp)

**File**: `src/apps/sequencer/engine/RoutingEngine.cpp`
**Location**: RoutingEngine constructor

```cpp
RoutingEngine::RoutingEngine(Engine &engine, Model &model) :
    _engine(engine),
    _model(model),
    _routing(_model.project().routing())
{
    _lastResetActive.fill(false);  // NEW - initialize all tracks to false
}
```

**Rationale**: Ensure clean initial state.

---

### 5. Reset state on route change (RoutingEngine.cpp)

**File**: `src/apps/sequencer/engine/RoutingEngine.cpp`
**Location**: `updateSinks()` function, in the routeChanged handling (~line 289-294)

```cpp
if (routeChanged) {
    // disable previous routing
    Routing::setRouted(routeState.target, routeState.tracks, false);
    // reset last state for play/record toggle
    if (routeState.target == Routing::Target::PlayToggle) {
        _lastPlayToggleActive = false;
    }
    if (routeState.target == Routing::Target::RecordToggle) {
        _lastRecordToggleActive = false;
    }
    // NEW - reset per-track reset state
    if (routeState.target == Routing::Target::Reset) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (routeState.tracks & (1 << i)) {
                _lastResetActive[i] = false;
            }
        }
    }
    // reset shaper state
    // ... rest unchanged
}
```

**Rationale**: When route configuration changes, clear edge detection state to prevent spurious triggers.

---

### 6. Intercept Reset target and handle edge detection (RoutingEngine.cpp)

**File**: `src/apps/sequencer/engine/RoutingEngine.cpp`
**Location**: `updateSinks()` function, inside the per-track loop (~line 307-348)

Replace the section from line 310 to 348 with:

```cpp
float routeSpan = route.max() - route.min();
for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
    if (tracks & (1 << trackIndex)) {
        float shapedSource = applyBiasDepthToSource(_sourceValues[routeIndex], route, trackIndex);
        // select shaper
        float shaperOut = shapedSource;
        auto shaper = route.shaper(trackIndex);
        auto &st = routeState.shaperState[trackIndex];
        switch (shaper) {
        case Routing::Shaper::None:
            break;
        case Routing::Shaper::Crease:
            shaperOut = applyCreaseSource(shapedSource);
            break;
        case Routing::Shaper::Location:
            shaperOut = applyLocation(shapedSource, st.location);
            break;
        case Routing::Shaper::Envelope:
            shaperOut = applyEnvelope(shapedSource, st.envelope);
            break;
        case Routing::Shaper::TriangleFold:
            shaperOut = applyTriangleFold(shapedSource);
            break;
        case Routing::Shaper::FrequencyFollower:
            shaperOut = applyFrequencyFollower(shapedSource, st);
            break;
        case Routing::Shaper::Activity:
            shaperOut = applyActivity(shapedSource, st);
            break;
        case Routing::Shaper::ProgressiveDivider:
            shaperOut = applyProgressiveDivider(shapedSource, st);
            break;
        case Routing::Shaper::Last:
            break;
        }
        if (route.creaseEnabled(trackIndex) && shaper != Routing::Shaper::Crease) {
            shaperOut = applyCreaseSource(shaperOut);
        }
        float routed = route.min() + shaperOut * routeSpan;

        // NEW - Special handling for Reset target
        if (target == Routing::Target::Reset) {
            bool active = routed > 0.5f;
            if (active != _lastResetActive[trackIndex]) {
                if (active) {
                    // Rising edge detected - reset this track
                    _engine.trackEngine(trackIndex)->reset();
                }
                _lastResetActive[trackIndex] = active;
            }
        } else {
            // Normal target handling
            _routing.writeTarget(target, (1 << trackIndex), routed);
        }
    }
}
```

**Rationale**:
- Intercept Reset before writeTarget() since model layer can't access engines
- Use 0.5f threshold like other toggle targets (Run, Play, etc.)
- Rising edge only triggers reset
- All other targets use existing path

---

### 7. Mark Reset as track target (Routing.h)

**File**: `src/apps/sequencer/model/Routing.h`
**Location**: Helper function `isTrackTarget()` (~line 282)

The function should already work automatically since Reset is between TrackFirst and TrackLast. Just verify this section exists:

```cpp
static bool isTrackTarget(Target target) {
    return target >= Target::TrackFirst && target <= Target::TrackLast;
}
```

**Rationale**: Reset needs to be recognized as a per-track target for proper routing UI behavior.

---

## Summary of Changes

| File | Lines Modified | Purpose |
|------|---------------|---------|
| **Routing.h** | 57 (insert) | Add Reset enum value |
| **Routing.cpp** | ~130, ~489 | Add name mapping and display formatting |
| **RoutingEngine.h** | ~66 | Add `_lastResetActive` state array |
| **RoutingEngine.cpp** | Constructor | Initialize reset state |
| **RoutingEngine.cpp** | ~295 | Clear reset state on route change |
| **RoutingEngine.cpp** | ~347 | Intercept and handle Reset with edge detection |

---

## Behavior

**When routed**:
- CV source > 0.5V = active
- Rising edge (inactive → active) triggers `trackEngine->reset()`
- Falling edge does nothing
- Each track independent (can reset different tracks from different sources)

**Example use cases**:
- Route CV In 1 → Track 1 Reset (external trigger resets track 1)
- Route CV Out 8 → Track 1-4 Reset (track 8 gates reset tracks 1-4)
- Route MIDI Note C4 → Track 2 Reset (MIDI note resets track 2)

---

## What reset() Does Per Track Type

### IndexedTrackEngine
- Resets `_currentStepIndex = 0` (back to first step)
- Clears `_stepTimer` and `_gateTimer`
- Resets `_sequenceState` (divisor, iteration counters)
- Sets `_running = true`

### NoteTrackEngine
- Resets `_currentStep = -1` (will advance to 0 on first tick)
- Clears gate/CV queues
- Resets accumulator to minValue
- Clears record history

### TuesdayTrackEngine
- Resets `_stepIndex = 0`
- Clears all timing: `_gateTicks`, `_coolDown`, `_slideCountDown`
- Resets CV: `_cvTarget`, `_cvCurrent = 0`

### DiscreteMapTrackEngine
- Resets `_activeStage = -1`
- Resets ramp: `_rampPhase = 0`, `_rampValue = -5V`
- Clears `_gateTimer`

### CurveTrackEngine
- Resets `_currentStep = -1`
- Clears chaos generators (Latoocarfian, Lorenz)
- Resets recorder and gate queue

### MidiCvTrackEngine
- Resets pitch bend and channel pressure
- Clears voice allocation

---

## Testing Plan

1. **Basic functionality**
   - Create route: CV In 1 → Track 1 Reset
   - Send rising edge on CV In 1 (0V → 5V)
   - Verify Track 1 returns to step 0
   - Send falling edge (5V → 0V)
   - Verify no additional reset occurs

2. **Multi-track reset**
   - Create route: CV In 1 → Tracks 1-4 Reset
   - Send rising edge
   - Verify all 4 tracks reset simultaneously on rising edge

3. **Independent per-track**
   - Route CV In 1 → Track 1 Reset
   - Route CV In 2 → Track 2 Reset
   - Trigger CV In 1 only
   - Verify only Track 1 resets

4. **Different track types**
   - Test with Indexed, Note, Tuesday, DiscreteMap, Curve tracks
   - Verify appropriate reset behavior for each type

5. **Route changes**
   - Configure route CV In 1 → Track 1 Reset
   - Set CV In 1 high
   - Change route target to different target
   - Change route back to Reset
   - Verify no spurious reset on route change

6. **MIDI trigger**
   - Route MIDI Note C4 → Track 1 Reset
   - Send MIDI Note On C4
   - Verify track resets
   - Send MIDI Note Off C4
   - Verify no additional reset

---

## Implementation Notes

- Reset is edge-triggered like PlayToggle/RecordToggle, not level-triggered like Play/Run
- Threshold is 0.5f (normalized) matching other toggle targets
- Per-track state array required (unlike global PlayToggle)
- Engine layer implementation required (can't be in model layer)
- Compatible with all shapers (Location, Envelope, etc.)
- Compatible with bias/depth/crease modulation
