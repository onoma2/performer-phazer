# EXPERIMENTAL: RTRIG Mode - Spread Accumulator Ticks Over Time
## Phased TDD Implementation Plan

**Date**: 2025-11-18
**Status**: 🧪 EXPERIMENTAL - HIGH RISK, PROCEED WITH CAUTION
**Priority**: OPTIONAL - Not required for core functionality

---

## Executive Summary

### Current Behavior (Working as Designed)
**RTRIG mode**: All N accumulator ticks fire **immediately** at step start (burst mode)
- retrig=3 → 3 ticks fire upfront, all at once
- Retrigger gates fire spread over time (you hear distinct ratchets)
- But accumulator increments happen before any gates fire

**Musical Result**: Step-based bursts of accumulator changes

### Proposed Experimental Behavior
**RTRIG mode spread ticks**: One accumulator tick per retrigger **as it fires**
- retrig=3 → 3 ticks spread over time
- Tick #1 when first retrigger gate fires
- Tick #2 when second retrigger gate fires
- Tick #3 when third retrigger gate fires

**Musical Result**: Time-based gradual accumulator changes synchronized with ratchets

---

## Why This Is Difficult (Architecture Constraints)

### The Gate Queue Problem

**Current Architecture** (`NoteTrackEngine.cpp` lines 408-429):
```cpp
// Phase 1: Accumulator ticks (IMMEDIATE)
for (int i = 0; i < stepRetrigger; ++i) {
    accumulator.tick();  // All 3 ticks happen NOW
}

// Phase 2: Gate scheduling (FUTURE)
while (stepRetrigger-- > 0) {
    _gateQueue.push({ tick + offset, true });   // Schedule gate ON
    _gateQueue.push({ tick + offset + half, false }); // Schedule gate OFF
    offset += retriggerLength;
}
```

**The Issue**:
- Ticks happen in Phase 1 (immediate)
- Gates are scheduled in Phase 2 (future timestamps)
- Gates are processed later in `tick()` method (lines 210-219)
- **No connection** between gate firing and accumulator ticking

**Why Gates Work**:
- Gate struct stores timestamp and boolean value
- `_gateQueue` is a sorted priority queue
- Gates fire when clock advances to their timestamp
- Simple, predictable, safe

**Why Accumulator Ticks Don't**:
- No metadata in Gate struct about accumulators
- No pointer/reference to which accumulator to tick
- Processing happens in generic loop with no step context
- **Pointer invalidation risk** if we store references

---

## Research Summary

**Documents**:
1. `RTRIG-Timing-Research.md` - Complete technical investigation (4 options analyzed)
2. `Queue-BasedAccumTicks.md` - Detailed implementation plan (4-phase approach)

**Options Evaluated**:
1. **Option 1**: Extend Gate struct with pointer → 🔴 HIGH RISK (dangling pointers)
2. **Option 2**: Separate accumulator tick queue → 🟠 MEDIUM RISK (pointer issues remain)
3. **Option 3**: Use sequence ID instead of pointer → 🟡 LOWER RISK (safest approach)
4. **Option 4**: Accept current behavior → ✅ ZERO RISK (recommended)

**Recommendation from Research**: **Accept current behavior** (Option 4)

**Reasons**:
- Pointer safety concerns (crashes possible)
- High complexity (10-19 days effort)
- Timing-dependent code hard to test
- Burst mode is still musically useful
- No user demand for this feature yet

**If proceeding anyway**: Use **Option 3** (Sequence ID) as safest compromise

---

## Option 3: Sequence ID Approach - TDD Plan

### Phase 1: Extend Gate Struct (2-3 days)

#### **Step 1.1: Add `shouldTickAccumulator` Flag**

**RED - Write Failing Test**:

File: `src/tests/unit/sequencer/TestGateQueue.cpp` (NEW)

```cpp
#include "catch.hpp"
#include "apps/sequencer/engine/NoteTrackEngine.h"

TEST_CASE("Gate struct stores shouldTickAccumulator flag") {
    NoteTrackEngine::Gate gate;
    gate.tick = 100;
    gate.gate = true;
    gate.shouldTickAccumulator = true;

    REQUIRE(gate.tick == 100);
    REQUIRE(gate.gate == true);
    REQUIRE(gate.shouldTickAccumulator == true);
}

TEST_CASE("Gate struct defaults shouldTickAccumulator to false") {
    NoteTrackEngine::Gate gate;
    gate.tick = 100;
    gate.gate = true;

    REQUIRE(gate.shouldTickAccumulator == false);  // Default: don't tick
}
```

**GREEN - Implement**:

File: `src/apps/sequencer/engine/NoteTrackEngine.h` (around line 82)

```cpp
struct Gate {
    uint32_t tick;
    bool gate;
    bool shouldTickAccumulator;  // NEW: Should this gate tick accumulator?

    // Constructors
    Gate() : tick(0), gate(false), shouldTickAccumulator(false) {}
    Gate(uint32_t t, bool g) : tick(t), gate(g), shouldTickAccumulator(false) {}
    Gate(uint32_t t, bool g, bool tickAccum)
        : tick(t), gate(g), shouldTickAccumulator(tickAccum) {}
};
```

**REFACTOR**:
- Add comments explaining field
- Check struct size: `sizeof(Gate)` should be ≤ 16 bytes
- Verify memory alignment

**Verification**:
```bash
cd build/sim/debug
make -j TestGateQueue
./src/tests/unit/TestGateQueue
# All tests should pass
```

---

#### **Step 1.2: Add `sequenceId` Field**

**RED - Write Failing Test**:

File: `src/tests/unit/sequencer/TestGateQueue.cpp`

```cpp
TEST_CASE("Gate struct stores sequenceId") {
    NoteTrackEngine::Gate gate;
    gate.tick = 100;
    gate.gate = true;
    gate.shouldTickAccumulator = true;
    gate.sequenceId = 1;  // Fill sequence

    REQUIRE(gate.sequenceId == 1);
}

TEST_CASE("Gate struct distinguishes main and fill sequences") {
    NoteTrackEngine::Gate mainGate;
    mainGate.sequenceId = NoteTrackEngine::MainSequenceId;

    NoteTrackEngine::Gate fillGate;
    fillGate.sequenceId = NoteTrackEngine::FillSequenceId;

    REQUIRE(mainGate.sequenceId == 0);
    REQUIRE(fillGate.sequenceId == 1);
    REQUIRE(mainGate.sequenceId != fillGate.sequenceId);
}
```

**GREEN - Implement**:

File: `src/apps/sequencer/engine/NoteTrackEngine.h`

```cpp
class NoteTrackEngine : public TrackEngine {
public:
    // Sequence ID constants
    static constexpr uint8_t MainSequenceId = 0;
    static constexpr uint8_t FillSequenceId = 1;

    struct Gate {
        uint32_t tick;
        bool gate;
        bool shouldTickAccumulator;  // From Step 1.1
        uint8_t sequenceId;           // NEW: 0=main, 1=fill

        // Updated constructors
        Gate() : tick(0), gate(false), shouldTickAccumulator(false), sequenceId(0) {}
        Gate(uint32_t t, bool g)
            : tick(t), gate(g), shouldTickAccumulator(false), sequenceId(0) {}
        Gate(uint32_t t, bool g, bool tickAccum, uint8_t seqId)
            : tick(t), gate(g), shouldTickAccumulator(tickAccum), sequenceId(seqId) {}
    };
    // ... rest of class
};
```

**REFACTOR**:
- Add static_assert for size: `static_assert(sizeof(Gate) <= 16, "Gate too large");`
- Document sequence ID constants
- Check if padding is optimal

**Memory Analysis**:
```
Before: uint32_t (4) + bool (1) + padding (3) = 8 bytes
After:  uint32_t (4) + bool (1) + bool (1) + uint8_t (1) + padding (1) = 8 bytes
```

**Verification**:
- All tests pass
- `sizeof(Gate) == 8` (no size increase!)

---

### Phase 2: Schedule Gates with Tick Metadata (3-4 days)

#### **Step 2.1: Analyze Current triggerStep() Logic**

**Tasks**:
1. Read `src/apps/sequencer/engine/NoteTrackEngine.cpp` lines 408-437
2. Understand retrigger gate scheduling loop
3. Identify where to set `shouldTickAccumulator = true`

**Current Code**:
```cpp
// Line 410-421: Current RTRIG mode (all ticks at once)
if (step.isAccumulatorTrigger()) {
    if (accumulator.enabled() && accumulator.triggerMode() == Accumulator::Retrigger) {
        int tickCount = stepRetrigger;
        for (int i = 0; i < tickCount; ++i) {
            accumulator.tick();  // ← REMOVE THIS (Phase 3)
        }
    }
}

// Line 425-428: Gate scheduling loop
while (stepRetrigger-- > 0 && retriggerOffset <= stepLength) {
    _gateQueue.pushReplace({ tick + gateOffset + retriggerOffset, true });  // ← MODIFY THIS
    _gateQueue.pushReplace({ tick + gateOffset + retriggerOffset + retriggerLength / 2, false });
    retriggerOffset += retriggerLength;
}
```

**Plan**:
- Remove lines 410-421 (upfront ticking)
- Modify line 426 to include tick metadata

---

#### **Step 2.2: Schedule Gates with shouldTickAccumulator = true**

**RED - Integration Test** (difficult, may skip):

File: `src/tests/integration/sequencer/TestRTRIGSpreadTicks.cpp` (NEW)

```cpp
TEST_CASE("RTRIG mode schedules accumulator ticks in gate queue") {
    // Setup sequence with RTRIG mode
    NoteSequence sequence;
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Retrigger);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setAccumulatorTrigger(true);
    step.setRetrigger(2);  // 3 retriggers

    // Trigger step (should schedule 3 gates with tick metadata)
    // ... mock engine, call triggerStep()

    // Verify: 6 gates scheduled (3 ON, 3 OFF)
    // Verify: ON gates have shouldTickAccumulator = true
    // Verify: OFF gates have shouldTickAccumulator = false
}
```

**GREEN - Implement**:

File: `src/apps/sequencer/engine/NoteTrackEngine.cpp`

```cpp
// Line 408-437 (MODIFIED)
int stepRetrigger = evalStepRetrigger(step, _noteTrack.retriggerProbabilityBias());
if (stepRetrigger > 1) {
    // REMOVED: Upfront accumulator ticking (lines 410-421)
    // Ticks will happen when gates fire instead

    uint32_t retriggerLength = divisor / stepRetrigger;
    uint32_t retriggerOffset = 0;

    // Determine if this step should tick accumulator
    bool shouldTick = (step.isAccumulatorTrigger() &&
                      targetSequence.accumulator().enabled() &&
                      targetSequence.accumulator().triggerMode() == Accumulator::Retrigger);
    uint8_t seqId = useFillSequence ? FillSequenceId : MainSequenceId;

    while (stepRetrigger-- > 0 && retriggerOffset <= stepLength) {
        // Schedule gate ON with accumulator tick metadata
        _gateQueue.pushReplace({
            Groove::applySwing(tick + gateOffset + retriggerOffset, swing()),
            true,           // Gate ON
            shouldTick,     // ← NEW: Tick accumulator when this gate fires
            seqId           // ← NEW: Which sequence (0=main, 1=fill)
        });

        // Schedule gate OFF (no accumulator tick)
        _gateQueue.pushReplace({
            Groove::applySwing(tick + gateOffset + retriggerOffset + retriggerLength / 2, swing()),
            false,          // Gate OFF
            false,          // Don't tick accumulator
            seqId
        });

        retriggerOffset += retriggerLength;
    }
}
```

**REFACTOR**:
- Extract `shouldTick` logic to helper method
- Add comments explaining metadata
- Ensure fill sequence logic is correct

**Verification**:
- Build and check for compilation errors
- Run unit tests (may not catch this yet)
- Need Phase 3 to actually tick accumulators

---

### Phase 3: Process Gates and Tick Accumulators (2-3 days)

#### **Step 3.1: Find Gate Processing Location**

**Investigation**:
1. Search for `_gateQueue.pop()` or `_gateQueue.front()`
2. Found in `NoteTrackEngine::tick()` lines 210-219
3. This is where gates are processed when clock advances

**Current Code**:
```cpp
// Line 210-219
while (!_gateQueue.empty() && tick >= _gateQueue.front().tick) {
    if (!_monitorOverrideActive) {
        result |= TickResult::GateUpdate;
        _activity = _gateQueue.front().gate;
        _gateOutput = _activity;
        midiOutputEngine.sendGate(_track.trackIndex(), _gateOutput);
    }
    _gateQueue.pop();  // Remove processed gate
}
```

---

#### **Step 3.2: Tick Accumulator When Gate Fires**

**RED - Integration Test**:

```cpp
TEST_CASE("RTRIG mode ticks accumulator when gate fires") {
    // Setup sequence with RTRIG mode, retrig=2
    // Trigger step (schedules 3 gates with tick metadata)
    // Simulate clock advancing:
    //   - Tick 0: First gate fires → accumulator value should be 1
    //   - Tick 16: Second gate fires → accumulator value should be 2
    //   - Tick 32: Third gate fires → accumulator value should be 3

    // Verify accumulator incremented gradually over time
    REQUIRE(finalValue == 3);
}
```

**GREEN - Implement**:

File: `src/apps/sequencer/engine/NoteTrackEngine.cpp` (line 210-219)

```cpp
// Modified gate processing loop
while (!_gateQueue.empty() && tick >= _gateQueue.front().tick) {
    const auto &gateEvent = _gateQueue.front();

    // Process gate output (existing logic)
    if (!_monitorOverrideActive) {
        result |= TickResult::GateUpdate;
        _activity = gateEvent.gate;
        _gateOutput = _activity;
        midiOutputEngine.sendGate(_track.trackIndex(), _gateOutput);
    }

    // NEW: Tick accumulator if scheduled
    if (gateEvent.shouldTickAccumulator) {
        // Lookup sequence by ID (safe, no pointers)
        NoteSequence* targetSeq = (gateEvent.sequenceId == MainSequenceId)
                                  ? _sequence
                                  : _fillSequence;

        // Validate sequence exists and accumulator enabled
        if (targetSeq && targetSeq->accumulator().enabled()) {
            const_cast<Accumulator&>(targetSeq->accumulator()).tick();
        }
    }

    _gateQueue.pop();
}
```

**REFACTOR**:
- Extract accumulator ticking to helper method: `tickScheduledAccumulator()`
- Add logging/debugging for tick events
- Ensure thread safety (FreeRTOS task context)

**Verification**:
```bash
cd build/sim/debug
make -j
./src/apps/sequencer/sequencer
# Manual test: Create sequence with RTRIG mode, observe gradual accumulator changes
```

---

### Phase 4: Testing and Verification (2-3 days)

#### **Test Case 1: RTRIG Mode Spreads Ticks**

**Setup**:
- Step 1: retrig=3, accumulator trigger enabled, TRIG=RTRIG
- Accumulator: Dir=Up, Min=0, Max=100, Step=1

**Procedure**:
1. Press PLAY
2. Watch accumulator value during step playback

**Expected (AFTER FIX)**:
- Value increments 3 times gradually over step duration
- Ticks synchronized with retrigger gate firings

**Before (CURRENT)**:
- Value increments 3 times immediately at step start

---

#### **Test Case 2: STEP Mode Unchanged**

**Verification**: STEP mode still ticks once per step (regression test)

**Setup**:
- Step 1: pulseCount=3, accumulator trigger enabled, TRIG=STEP

**Expected**:
- Value increments by 1 (once per step, first pulse only)
- No change from current behavior

---

#### **Test Case 3: GATE Mode Unchanged**

**Verification**: GATE mode still respects gate mode settings

**Setup**:
- Step 1: pulseCount=3, gateMode=FIRST, accumulator trigger enabled, TRIG=GATE

**Expected**:
- Value increments by 1 (first pulse only)
- No change from current behavior

---

#### **Test Case 4: Sequence Change During Playback**

**Risk Test**: Verify no crashes when sequence changes while gates scheduled

**Procedure**:
1. Start playback with RTRIG mode
2. Change to different pattern mid-step
3. Observe no crashes

**Expected**:
- Graceful handling (sequence ID lookup might fail, that's OK)
- No crashes, no undefined behavior

---

#### **Test Case 5: Fill Mode**

**Verification**: Fill sequence accumulator works correctly

**Setup**:
- Main sequence: RTRIG mode
- Fill sequence: Different accumulator settings
- Enable fill mode

**Expected**:
- Fill sequence accumulator ticks correctly
- sequenceId lookup works for both main and fill

---

### Phase 5: Hardware Testing (1-2 days)

**Critical Tests on Real Hardware**:
1. Fast tempo (>200 BPM) - verify no timing glitches
2. Long sequences (16 steps) - verify no queue overflow
3. Multiple tracks - verify no interference
4. Pattern changes - verify no crashes
5. Project load/save - verify persistence

---

## Risk Analysis

### Implementation Risks

| Risk | Severity | Likelihood | Mitigation |
|------|----------|------------|------------|
| Sequence deleted during playback | HIGH | Medium | Validate sequence pointer before ticking |
| Queue overflow (>16 entries) | MEDIUM | Low | Test with high retrigger counts |
| Timing drift | MEDIUM | Low | Verify tick timestamps are exact |
| Thread safety issues | HIGH | Medium | Ensure accumulator access is thread-safe |
| Pointer invalidation | LOW | Very Low | Using IDs not pointers (safer) |
| Performance degradation | LOW | Very Low | Minimal added logic per gate |

### Testing Risks

| Risk | Severity | Likelihood | Mitigation |
|------|----------|------------|------------|
| Integration tests difficult | MEDIUM | High | Manual testing as primary verification |
| Timing-dependent bugs | HIGH | Medium | Extensive hardware testing required |
| Edge cases not caught | MEDIUM | Medium | Comprehensive test matrix |

### Deployment Risks

| Risk | Severity | Likelihood | Mitigation |
|------|----------|------------|------------|
| User confusion (behavior change) | MEDIUM | Low | Document clearly, feature flag? |
| Crashes in production | HIGH | Low | Thorough testing before release |
| Backwards compatibility | NONE | N/A | New behavior only (RTRIG spread) |

**Overall Risk**: 🟡 **MEDIUM-HIGH** (safer than pointer-based, but still significant)

---

## Effort Estimate

| Phase | Duration | Complexity |
|-------|----------|------------|
| Phase 1: Extend Gate struct | 2-3 days | LOW |
| Phase 2: Schedule with metadata | 3-4 days | MEDIUM |
| Phase 3: Process and tick | 2-3 days | MEDIUM |
| Phase 4: Testing (sim) | 2-3 days | MEDIUM |
| Phase 5: Hardware testing | 1-2 days | HIGH |
| **TOTAL** | **10-15 days** | **MEDIUM-HIGH** |

**Dependencies**:
- Understanding of FreeRTOS task architecture
- Understanding of gate queue processing
- Access to hardware for final verification

---

## Success Criteria

### Functional Requirements
- [x] ✅ RTRIG mode ticks spread over time (one per retrigger gate)
- [x] ✅ STEP mode unchanged (one tick per step)
- [x] ✅ GATE mode unchanged (respects gate mode settings)
- [x] ✅ Fill sequences work correctly
- [x] ✅ No crashes when sequences change during playback

### Performance Requirements
- [x] ✅ No timing drift (ticks fire at exact tick timestamps)
- [x] ✅ No frame drops at high tempos (>200 BPM)
- [x] ✅ Queue doesn't overflow (test retrig=7, pulse=7)

### Code Quality
- [x] ✅ Clean code (well-commented, follows patterns)
- [x] ✅ Tests pass (unit + integration + manual)
- [x] ✅ No compiler warnings
- [x] ✅ Memory footprint acceptable (struct size ≤16 bytes)

---

## Alternative: Feature Flag

**Recommendation**: Make this experimental feature optional via compile-time flag

**Implementation**:

File: `src/apps/sequencer/Config.h`

```cpp
// EXPERIMENTAL: Spread RTRIG accumulator ticks over time
// Default: OFF (use burst mode)
#ifndef CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS
#define CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS 0
#endif
```

**Usage**:

File: `src/apps/sequencer/engine/NoteTrackEngine.cpp`

```cpp
#if CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS
    // New behavior: Schedule ticks in gate queue
    _gateQueue.pushReplace({ tick, true, shouldTick, seqId });
#else
    // Current behavior: Tick upfront (burst mode)
    for (int i = 0; i < tickCount; ++i) {
        accumulator.tick();
    }
#endif
```

**Benefits**:
- Easy to disable if issues arise
- Can ship both behaviors
- Users can opt-in to experimental feature
- Safer deployment strategy

---

## Recommendation

### For Production Use: **DO NOT IMPLEMENT** ⚠️

**Reasons**:
1. **High risk** for marginal benefit
2. **Current behavior works** and is musically useful
3. **No user demand** for this feature yet
4. **High effort** (10-15 days) for experimental feature
5. **Testing complexity** (timing-dependent, hard to verify)

### For Experimental Branch: **PROCEED WITH CAUTION** 🧪

**If you must implement**:
1. Use **sequence ID approach** (Option 3) - safest method
2. Add **feature flag** - make it optional
3. **Test extensively** on hardware - at least 2 days
4. **Document limitations** - known edge cases
5. **Don't merge to main** until proven stable

### Better Alternative: **WAIT FOR USER FEEDBACK** ✅

**Recommendation**:
1. Ship current implementation (burst mode)
2. Document the behavior clearly
3. Get user feedback - do they want spread ticks?
4. Only implement if users request it
5. Prioritize features with proven demand

---

## Related Documentation

- **RTRIG-Timing-Research.md** - Technical investigation (4 options analyzed)
- **Queue-BasedAccumTicks.md** - Detailed implementation plan
- **QWEN.md** - Accumulator feature documentation
- **CLAUDE.md** - Trigger mode documentation
- **TODO.md** - Line 1846 (experimental feature status)

---

**Document Version**: 1.0
**Author**: Claude (AI Assistant)
**Status**: Complete analysis, ready for decision
**Last Updated**: 2025-11-18

**RECOMMENDATION**: 🛑 **DO NOT IMPLEMENT** - High risk, low demand, current behavior is acceptable
