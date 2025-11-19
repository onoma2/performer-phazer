# TODO - PEW|FORMER Development

## Active Tasks

### ✅ BUG FIX COMPLETE: Accumulator Serialization (Phases 1-2 Complete, Hardware Tested)

**Status**: Phases 1-2 complete, hardware tested and verified working ✅

**Remaining phases (3-5) skipped** - hardware testing confirmed fix works correctly.

**What was fixed:**
- Accumulator parameters now persist across save/load on hardware
- Version33 project format with backward compatibility
- All 4 unit tests passing

**Files Modified:**
- `src/apps/sequencer/model/Accumulator.h/cpp` - Serialization methods
- `src/apps/sequencer/model/NoteSequence.cpp` - Integration
- `src/apps/sequencer/model/ProjectVersion.h` - Version33
- `src/tests/unit/sequencer/TestAccumulatorSerialization.cpp` - Unit tests

---

### 🎯 FEATURE: Accumulator UX Improvements (TDD Implementation)

#### Overview
Three improvements discovered during hardware testing to enhance accumulator usability and behavior.

**Improvements:**
1. Default min value should be 0 (currently -7)
2. Counter should reset to 0 when pressing STOP
3. Counter should start incrementing after first step pass (not immediately on enable/load)

---

### Improvement 1: Change Default Min Value to 0

#### Test Plan
**Goal**: Default accumulator min value should be 0 instead of -7

**Step 1.1: Update Test Expectations (RED)**
- File: `src/tests/unit/sequencer/TestAccumulator.cpp`
- Modify test to expect minValue = 0 for new Accumulator
- Test will fail with current implementation

**Step 1.2: Update Accumulator Constructor (GREEN)**
- File: `src/apps/sequencer/model/Accumulator.cpp`
- Change `_minValue(-7)` to `_minValue(0)` in constructor
- Test should pass

**Expected Impact:**
- New projects will have accumulator min = 0
- Existing saved projects will load their saved min values unchanged
- Better default for most musical use cases (unipolar range 0-7)

---

### Improvement 2: Reset Counter on STOP

#### Test Plan
**Goal**: Accumulator currentValue resets to minValue when sequencer stops

**Step 2.1: Locate STOP Handler**
- Search for sequencer stop/reset logic
- Likely in: `src/apps/sequencer/engine/NoteTrackEngine.cpp` or `Engine.cpp`
- Document where STOP is handled

**Step 2.2: Write Test (RED)**
- Create test case: accumulator resets on stop
- Set accumulator to non-zero value
- Trigger stop
- Verify currentValue == minValue

**Step 2.3: Implement Reset Logic (GREEN)**
- Add method to Accumulator: `void reset() { _currentValue = _minValue; }`
- Call `accumulator.reset()` in stop handler
- Test should pass

**Expected Impact:**
- Pressing STOP resets accumulator to known state
- Predictable behavior when restarting sequence
- Counter starts fresh each playback session

---

### Improvement 3: Delay First Tick Until After First Step

#### Test Plan
**Goal**: Accumulator doesn't increment immediately on enable/load; waits for first step to pass

**Background:**
Currently, accumulator likely starts incrementing immediately when enabled. This causes the first note to already have accumulation applied, which is unexpected. Should start at minValue and only increment after the first step trigger.

**Step 3.1: Add "First Tick" Flag**
- Add to Accumulator.h: `bool _hasStarted` (default false)
- Track whether accumulator has seen its first trigger

**Step 3.2: Write Test (RED)**
- Create test: first tick is skipped
- Enable accumulator
- Call tick() once
- Verify currentValue == minValue (not minValue + stepValue)
- Call tick() again
- Verify currentValue == minValue + stepValue

**Step 3.3: Implement Delayed Start Logic (GREEN)**
```cpp
void Accumulator::tick() const {
    if (!_enabled) return;

    if (!_hasStarted) {
        const_cast<Accumulator*>(this)->_hasStarted = true;
        return; // Skip first tick
    }

    // Existing tick logic...
}
```

**Step 3.4: Reset Flag on Stop/Reset**
- Modify reset() method: `_hasStarted = false`
- Ensures next playback starts fresh

**Step 3.5: Serialization Update**
- Add `_hasStarted` to write() method
- Add `_hasStarted` to read() method
- Update TestAccumulatorSerialization to include this field
- Increment to Version34? (or keep in Version33 since not released yet)

**Expected Impact:**
- First step plays at base note value (no accumulation)
- Accumulation starts from second triggered step
- More intuitive behavior for users
- Counter always visible as it increments

---

### Implementation Checklist

#### Improvement 1: Default Min = 0 ✅ COMPLETE
- [x] Step 1.1: Update test expectations (RED)
- [x] Step 1.2: Update constructor (GREEN)
- [x] Commit: "Change default accumulator min to 0"

**Result**: Default min value changed from -7 to 0. Ready for simulator testing.

#### Improvement 2: Reset on STOP ✅ COMPLETE
- [x] Step 2.1: Locate STOP handler (NoteTrackEngine::reset())
- [x] Step 2.2: Write reset test (RED) - TestAccumulator.cpp
- [x] Step 2.3: Implement reset logic (GREEN) - Accumulator::reset() + engine integration
- [x] Ready for local testing

**Implementation:**
- Added `reset()` method to Accumulator.h/cpp
- Resets `_currentValue` to `_minValue`
- Resets `_pendulumDirection` to 1 (up)
- Integrated into NoteTrackEngine::reset() for both main and fill sequences
- Test case verifies reset behavior

**Expected behavior:**
- Pressing STOP resets accumulator to minValue
- Predictable behavior when restarting sequence
- Counter starts fresh each playback session

#### Improvement 3: Delay First Tick ✅ COMPLETE
- [x] Step 3.1: Add _hasStarted flag to model
- [x] Step 3.2: Write delayed start test (RED) - TestAccumulator.cpp
- [x] Step 3.3: Implement delayed tick logic (GREEN) - Accumulator::tick()
- [x] Step 3.4: Update reset() to clear flag
- [x] Step 3.5: Update serialization for _hasStarted
- [x] Ready for local testing

**Implementation:**
- Added `_hasStarted` mutable bool flag to Accumulator.h
- Initialized to false in constructor
- Modified `tick()` to skip first call (sets _hasStarted=true and returns early)
- Updated `reset()` to clear _hasStarted flag
- Added _hasStarted to serialization (write/read methods)
- Test case verifies delayed tick behavior

**Expected behavior:**
- First step plays at base note value (no accumulation applied)
- Accumulation starts from second triggered step
- More intuitive UX - counter visible as it increments
- Reset clears delay flag for next playback

---

### Testing Strategy

**Unit Tests:**
1. TestAccumulator.cpp - Update/add test cases for each improvement
2. TestAccumulatorSerialization.cpp - Update if serialization changes

**Hardware Testing:**
1. Test default min value on new project
2. Test STOP reset behavior
3. Test first-tick delay on enable and on load
4. Verify backward compatibility with existing projects

---

### Files to Modify

**Improvement 1:**
- `src/apps/sequencer/model/Accumulator.cpp` - Constructor default
- `src/tests/unit/sequencer/TestAccumulator.cpp` - Test expectations

**Improvement 2:**
- `src/apps/sequencer/model/Accumulator.h` - Add reset() method
- `src/apps/sequencer/model/Accumulator.cpp` - Implement reset()
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` (or Engine.cpp) - Call reset on STOP
- `src/tests/unit/sequencer/TestAccumulator.cpp` - Add reset test

**Improvement 3:**
- `src/apps/sequencer/model/Accumulator.h` - Add _hasStarted flag
- `src/apps/sequencer/model/Accumulator.cpp` - Implement delayed tick, update reset(), update serialization
- `src/tests/unit/sequencer/TestAccumulator.cpp` - Add delayed tick test
- `src/tests/unit/sequencer/TestAccumulatorSerialization.cpp` - Update serialization tests

---

### 🎯 NEW FEATURE: Accumulator Trigger Mode (TDD Implementation)

#### Overview
Add global setting to control when accumulator increments - by step, by gate pulse, or by ratchet subdivision.

**Three Trigger Modes:**
1. **STEP** - Increment once per step (current behavior)
2. **GATE** - Increment per gate pulse (respects pulse count and gate mode)
3. **RTRIG** - Increment per ratchet/retrigger subdivision

**Implementation:** Global setting on ACCUM page (Option A)

---

### Phase 1: Model Layer - Add TriggerMode Parameter

#### Goal
Add TriggerMode enum and field to Accumulator model with serialization.

#### Step 1.1: Define TriggerMode Enum and Add Field

**File:** `src/apps/sequencer/model/Accumulator.h`

**Actions:**
1. Add TriggerMode enum after existing enums:
   ```cpp
   enum TriggerMode { Step, Gate, Retrigger };
   ```
2. Add getter/setter methods:
   ```cpp
   TriggerMode triggerMode() const { return static_cast<TriggerMode>(_triggerMode); }
   void setTriggerMode(TriggerMode mode) { _triggerMode = mode; }
   ```
3. Add bitfield in private section (2 bits, allowing 4 modes for future):
   ```cpp
   uint8_t _triggerMode : 2;
   ```
4. Note: `_ratchetTriggerMode` already exists (3 bits) but unused - we're adding separate `_triggerMode`

**Expected Result:** Compiles with new field added

---

#### Step 1.2: Initialize TriggerMode in Constructor (RED→GREEN)

**File:** `src/apps/sequencer/model/Accumulator.cpp`

**Actions:**
1. Write test first (RED):
   - File: `src/tests/unit/sequencer/TestAccumulator.cpp`
   - Test case: `default_trigger_mode_is_step`
   - Verify new Accumulator has triggerMode() == Accumulator::Step

2. Implement (GREEN):
   - Add to constructor initializer list: `_triggerMode(Step)`

**Expected Result:** Test passes

---

#### Step 1.3: Update Serialization (RED→GREEN)

**File:** `src/apps/sequencer/model/Accumulator.cpp`

**Actions:**
1. Write test first (RED):
   - File: `src/tests/unit/sequencer/TestAccumulatorSerialization.cpp`
   - Add triggerMode to round-trip test
   - Set triggerMode to different values and verify serialization

2. Update write() method (GREEN):
   - Pack _triggerMode into flags byte (already has room):
   ```cpp
   uint8_t flags = (_mode << 0) | (_polarity << 2) |
                   (_direction << 3) | (_order << 5) |
                   (_enabled << 7);
   ```
   - Need to expand or use another byte - analyze current bitfield usage
   - Current flags: mode(2) + polarity(1) + direction(2) + order(2) + enabled(1) = 8 bits FULL
   - **Solution:** Write _triggerMode as separate byte OR pack with _hasStarted

3. Update read() method (GREEN):
   - Read and unpack _triggerMode from serialization

**Expected Result:** Serialization tests pass, triggerMode persists across save/load

---

### Phase 2: Engine Layer - Implement Trigger Mode Logic

#### Goal
Modify NoteTrackEngine to call accumulator.tick() based on selected trigger mode.

#### Step 2.1: Analyze Current Code

**File:** `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Current Implementation (lines 354-360):**
```cpp
// STEP mode - increment once per step
if (step.isAccumulatorTrigger()) {
    const_cast<Accumulator&>(targetSequence.accumulator()).tick();
}
```

**Target Locations:**
- **STEP mode:** Keep at line 354-360 (current location)
- **GATE mode:** Add inside `if (shouldFireGate)` block after line 390
- **RTRIG mode:** Add inside retrigger while loop at line 402

---

#### Step 2.2: Implement STEP Mode (Current Behavior)

**Actions:**
1. Modify existing code to check trigger mode:
   ```cpp
   if (step.isAccumulatorTrigger() && targetSequence.accumulator().enabled()) {
       if (targetSequence.accumulator().triggerMode() == Accumulator::Step) {
           const_cast<Accumulator&>(targetSequence.accumulator()).tick();
       }
   }
   ```

**Expected Result:** STEP mode works as before (backward compatible)

---

#### Step 2.3: Implement GATE Mode

**File:** `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Actions:**
1. Add after line 390, inside `if (shouldFireGate)` block:
   ```cpp
   if (shouldFireGate) {
       // Existing gate generation code...

       // GATE mode: increment per gate pulse
       if (step.isAccumulatorTrigger() && targetSequence.accumulator().enabled()) {
           if (targetSequence.accumulator().triggerMode() == Accumulator::Gate) {
               const_cast<Accumulator&>(targetSequence.accumulator()).tick();
           }
       }

       uint32_t stepLength = ...
   }
   ```

**Expected Result:**
- GATE mode increments based on gate mode and pulse count
- ALL mode with pulseCount=3 → 4 increments
- FIRST mode with pulseCount=3 → 1 increment
- HOLD mode with pulseCount=3 → 1 increment
- FIRSTLAST mode with pulseCount=3 → 2 increments

---

#### Step 2.4: Implement RETRIGGER Mode

**File:** `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Actions:**
1. Add inside while loop at line 402:
   ```cpp
   int stepRetrigger = evalStepRetrigger(step, _noteTrack.retriggerProbabilityBias());
   if (stepRetrigger > 1) {
       uint32_t retriggerLength = divisor / stepRetrigger;
       uint32_t retriggerOffset = 0;
       while (stepRetrigger-- > 0 && retriggerOffset <= stepLength) {
           // RETRIGGER mode: increment per ratchet subdivision
           if (step.isAccumulatorTrigger() && targetSequence.accumulator().enabled()) {
               if (targetSequence.accumulator().triggerMode() == Accumulator::Retrigger) {
                   const_cast<Accumulator&>(targetSequence.accumulator()).tick();
               }
           }

           _gateQueue.pushReplace({ ... });
           _gateQueue.pushReplace({ ... });
           retriggerOffset += retriggerLength;
       }
   }
   ```

**Expected Result:**
- RTRIG mode increments once per ratchet subdivision
- retrigger=3 (4 subdivisions) → 4 increments

---

#### Step 2.5: Build and Manual Testing

**Actions:**
1. Build simulator: `cd build/sim/debug && make -j`
2. Test each mode:
   - STEP: Enable accumulator trigger on step, set mode=STEP, verify 1 increment per step
   - GATE: Set pulseCount=3, gateMode=ALL, mode=GATE, verify 4 increments
   - RTRIG: Set retrigger=3, mode=RTRIG, verify 4 increments
3. Verify combinations work correctly

**Expected Result:** All three modes work as designed

---

### Phase 3: UI Layer - Add TriggerMode to ACCUM Page

#### Goal
Add TriggerMode parameter to ACCUM page for user control.

#### Step 3.1: Add TriggerMode to AccumulatorListModel

**File:** `src/apps/sequencer/ui/model/AccumulatorListModel.h`

**Actions:**
1. Add to Item enum (after existing parameters):
   ```cpp
   TriggerMode,
   ```
2. Update itemCount() if needed
3. Add case in text() method to return "Trigger Mode"
4. Add case in valueFmt() to return mode names: "STEP", "GATE", "RTRIG"
5. Add indexed value support (similar to Direction/Order)

**Expected Result:** TriggerMode appears in ACCUM page list

---

#### Step 3.2: Implement Encoder Control

**File:** `src/apps/sequencer/ui/model/AccumulatorListModel.h`

**Actions:**
1. Add to edit() method:
   ```cpp
   case TriggerMode:
       accumulator.setTriggerMode(
           static_cast<Accumulator::TriggerMode>(
               ModelUtils::adjustedByStep(
                   accumulator.triggerMode(), -1, 2, step, !shift
               )
           )
       );
       break;
   ```

**Expected Result:** Encoder cycles through STEP → GATE → RTRIG → STEP

---

#### Step 3.3: Test UI in Simulator

**Actions:**
1. Navigate to ACCUM page
2. Find "Trigger Mode" parameter
3. Use encoder to change value
4. Verify display shows: STEP, GATE, RTRIG
5. Save and load project to verify persistence

**Expected Result:** UI fully functional

---

### Phase 4: Testing & Documentation

#### Step 4.1: Unit Tests

**Files to Test:**
- `TestAccumulator.cpp` - TriggerMode getter/setter
- `TestAccumulatorSerialization.cpp` - TriggerMode serialization

**Test Cases:**
1. Default trigger mode is STEP
2. Set/get trigger mode for all three values
3. Serialization round-trip preserves trigger mode
4. Invalid values clamp correctly

---

#### Step 4.2: Integration Testing

**Scenarios:**
1. **STEP mode + pulse count:**
   - Set pulseCount=3, triggerMode=STEP
   - Verify: 1 increment per step (not 4)

2. **GATE mode + gate modes:**
   - Set pulseCount=3, gateMode=ALL, triggerMode=GATE
   - Verify: 4 increments
   - Set pulseCount=3, gateMode=FIRST, triggerMode=GATE
   - Verify: 1 increment
   - Set pulseCount=3, gateMode=FIRSTLAST, triggerMode=GATE
   - Verify: 2 increments

3. **RTRIG mode + retrigger:**
   - Set retrigger=3, triggerMode=RTRIG
   - Verify: 4 increments

4. **Combined scenarios:**
   - pulseCount=2, retrigger=1, gateMode=ALL, triggerMode=GATE
   - Verify: 3 increments (3 gate pulses)
   - Same setup but triggerMode=RTRIG
   - Verify: 6 increments (3 pulses × 2 retriggers each)

---

#### Step 4.3: Update Documentation

**Files to Update:**
1. **TODO.md** - Mark feature complete
2. **CLAUDE.md** - Add Trigger Mode section to Accumulator Feature documentation
3. **CHANGELOG.md** - Add to unreleased section

---

### Implementation Checklist

#### Phase 1: Model Layer
- [ ] Step 1.1: Define TriggerMode enum and add field
- [ ] Step 1.2: Initialize in constructor with test (RED→GREEN)
- [ ] Step 1.3: Update serialization with test (RED→GREEN)

#### Phase 2: Engine Layer
- [ ] Step 2.1: Analyze current code locations
- [ ] Step 2.2: Implement STEP mode (refactor existing)
- [ ] Step 2.3: Implement GATE mode
- [ ] Step 2.4: Implement RTRIG mode
- [ ] Step 2.5: Build and manual testing

#### Phase 3: UI Layer
- [ ] Step 3.1: Add to AccumulatorListModel
- [ ] Step 3.2: Implement encoder control
- [ ] Step 3.3: Test UI in simulator

#### Phase 4: Testing & Documentation
- [ ] Step 4.1: Unit tests (model + serialization)
- [ ] Step 4.2: Integration testing (all mode combinations)
- [ ] Step 4.3: Update documentation

---

### Expected File Changes

**Model Layer:**
- `src/apps/sequencer/model/Accumulator.h` - Add enum, field, getters/setters
- `src/apps/sequencer/model/Accumulator.cpp` - Constructor, serialization
- `src/tests/unit/sequencer/TestAccumulator.cpp` - Add tests
- `src/tests/unit/sequencer/TestAccumulatorSerialization.cpp` - Update tests

**Engine Layer:**
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` - Implement 3 trigger modes

**UI Layer:**
- `src/apps/sequencer/ui/model/AccumulatorListModel.h` - Add TriggerMode parameter

**Documentation:**
- `TODO.md` - This plan and completion tracking
- `CLAUDE.md` - Feature documentation
- `CHANGELOG.md` - Release notes

---

### Technical Notes

**Bitfield Usage Analysis:**
Current Accumulator bitfields (line 57-62 in Accumulator.h):
```cpp
uint8_t _mode : 2;              // 2 bits
uint8_t _polarity : 1;          // 1 bit
uint8_t _direction : 2;         // 2 bits
uint8_t _order : 2;             // 2 bits
uint8_t _enabled : 1;           // 1 bit
uint8_t _ratchetTriggerMode : 3; // 3 bits (UNUSED - can repurpose)
// Total: 13 bits across 2 bytes
```

**Serialization Size:**
- Current: 10 bytes (1 flags + 2 minValue + 2 maxValue + 1 stepValue + 2 currentValue + 1 pendulumDirection + 1 hasStarted)
- After TriggerMode: 10 bytes (no change - pack into existing structure or reuse _ratchetTriggerMode bits)

**Option:** Repurpose `_ratchetTriggerMode` (3 bits) for `_triggerMode` (2 bits) since it's currently unused.

---

### ✅ IMPLEMENTATION COMPLETE (2025-11-17)

**Status**: Fully implemented and tested

**Commits:**
- `f041a6c` - Engine: Implement accumulator trigger mode logic
- `8216350` - UI: Add TriggerMode parameter to ACCUM page
- `ff6e1ac` - Fix: RETRIGGER mode now works with retrigger=1
- `97deac0` - Fix: RETRIGGER mode now ticks N times for N retriggers

**Verification:**
- ✅ All 15 unit tests pass (TestAccumulator.cpp)
- ✅ All 4 serialization tests pass (TestAccumulatorSerialization.cpp)
- ✅ Simulator testing complete: STEP, GATE, RTRIG modes work correctly
- ✅ Documentation updated: CLAUDE.md, CHANGELOG.md, QWEN.md, RTRIG-Timing-Research.md, Queue-BasedAccumTicks.md

**Known Behavior:**
- RTRIG mode ticks N times immediately at step start (not spread over time)
- This is an architectural limitation due to minimal gate queue structure
- Gates fire spread over time (you hear ratchets), but accumulator increments are upfront
- See `RTRIG-Timing-Research.md` for technical investigation and workaround analysis
- See `Queue-BasedAccumTicks.md` for detailed implementation plan if future enhancement needed
- Recommendation: Accept current behavior (pointer invalidation risks in queue-based approaches)

---

## 🐛 BUG FIXES: Accumulator Trigger Modes (CRITICAL)

**Bug Report**: See `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md`
**Status**: 🔴 CRITICAL - Found on hardware testing 2025-11-18
**Priority**: P0 - Must fix before any release

### Bug #1: STEP and GATE Trigger Modes Not Working

**Issue**: Accumulator increments on every gate pulse regardless of TRIG mode setting. All three modes (STEP, GATE, RTRIG) behave identically.

**Expected vs Actual**:
- STEP mode: Should tick once per step → Actually ticks on every pulse
- GATE mode: Should tick per gate fired → Actually ticks on every pulse
- RTRIG mode: Works correctly (ticks on each retrigger)

---

### TDD Fix Plan - Bug #1: Trigger Mode Logic

#### Phase 1: Reproduce Bug in Tests (RED - Write Failing Tests)

**Goal**: Create integration tests that expose the bug

##### Test 1.1: STEP Mode Integration Test (RED)

**File**: `src/tests/integration/sequencer/TestAccumulatorTriggerModes.cpp` (NEW)

```cpp
#include "catch.hpp"
#include "apps/sequencer/engine/NoteTrackEngine.h"
#include "apps/sequencer/model/NoteSequence.h"
#include "apps/sequencer/model/NoteTrack.h"

TEST_CASE("STEP mode ticks once per step with pulseCount=3") {
    // Setup: Step with pulseCount=3 (4 pulses total)
    NoteTrack track;
    NoteSequence sequence;

    // Configure accumulator for STEP mode
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Step);
    sequence.accumulator().setDirection(Accumulator::Up);
    sequence.accumulator().setMin(0);
    sequence.accumulator().setMax(100);
    sequence.accumulator().setStep(1);

    // Configure step
    sequence.step(0).setGate(true);
    sequence.step(0).setPulseCount(3);  // 4 pulses total (0-3)
    sequence.step(0).setAccumulatorTrigger(true);

    // Create engine and trigger step 4 times (simulating pulse counter)
    NoteTrackEngine engine(track, &sequence, /* ... */);

    int valueBefore = sequence.accumulator().value();  // Should be 0

    // Simulate 4 pulses for the same step
    for (int pulse = 1; pulse <= 4; pulse++) {
        engine.triggerStep(/* tick */, /* divisor */, /* ... */);
    }

    int valueAfter = sequence.accumulator().value();

    // EXPECTED: Increment by 1 (once per step, not once per pulse)
    REQUIRE(valueAfter == valueBefore + 1);  // This will FAIL (bug)

    // ACTUAL (buggy): valueAfter == valueBefore + 4 (ticks on every pulse)
}

TEST_CASE("STEP mode ignores pulse count variations") {
    // Test with pulseCount=0 (1 pulse) - should tick once
    // Test with pulseCount=7 (8 pulses) - should tick once
    // Verify STEP mode truly independent of pulse count
}
```

##### Test 1.2: GATE Mode Integration Test (RED)

```cpp
TEST_CASE("GATE mode ticks per gate fired with gateMode=FIRST") {
    // Setup: pulseCount=3, gateMode=FIRST
    NoteSequence sequence;
    sequence.accumulator().setTriggerMode(Accumulator::Gate);
    sequence.step(0).setPulseCount(3);  // 4 pulses
    sequence.step(0).setGateMode(NoteSequence::First);  // Gate fires once (first pulse only)

    // Simulate 4 pulses
    // EXPECTED: Increment by 1 (gate fires only on first pulse)
    // ACTUAL (buggy): Increment by 4 (ticks on every pulse)

    REQUIRE(sequence.accumulator().value() == 1);  // Will FAIL
}

TEST_CASE("GATE mode ticks per gate fired with gateMode=ALL") {
    // Setup: pulseCount=3, gateMode=ALL
    // Gates fire on all 4 pulses
    // EXPECTED: Increment by 4
    // This should PASS even with bug (coincidentally correct)

    REQUIRE(sequence.accumulator().value() == 4);  // Should PASS
}

TEST_CASE("GATE mode respects gate mode FIRSTLAST") {
    // Setup: pulseCount=3, gateMode=FIRSTLAST
    // Gates fire on first and last pulse (2 gates)
    // EXPECTED: Increment by 2
    // ACTUAL (buggy): Increment by 4

    REQUIRE(sequence.accumulator().value() == 2);  // Will FAIL
}
```

##### Test 1.3: Verify RTRIG Mode Still Works (GREEN)

```cpp
TEST_CASE("RTRIG mode works correctly") {
    // Setup: retrig=3
    // EXPECTED: Increment by 3 (all at once at step start)
    // This should PASS (RTRIG mode is working)

    REQUIRE(sequence.accumulator().value() == 3);  // Should PASS
}
```

**Verification**: All tests should FAIL except RTRIG test
```bash
cd build/sim/debug
make -j TestAccumulatorTriggerModes
./src/tests/integration/TestAccumulatorTriggerModes
# Expected: 4+ failures
```

---

#### Phase 2: Investigate Root Cause (Analysis)

**Goal**: Find where the trigger mode logic is broken

##### Step 2.1: Code Review of triggerStep()

**File**: `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Lines to investigate**:
- Line ~353: STEP mode logic
- Line ~392: GATE mode logic
- Line ~410: RTRIG mode logic

**Questions to answer**:
1. Is trigger mode being checked at all?
2. Are all three conditions evaluating to true?
3. Is the mode enum comparison correct?
4. Is fill sequence logic interfering?
5. Is `triggerStep()` being called multiple times per step?

##### Step 2.2: Add Debug Logging (Temporary)

```cpp
// In triggerStep() - add at top
#ifdef DEBUG_ACCUMULATOR_TRIG
    DBG("triggerStep: pulseCounter=%d, triggerMode=%d, shouldTick=%d",
        _pulseCounter,
        sequence.accumulator().triggerMode(),
        step.isAccumulatorTrigger());
#endif
```

Build and run simulator with logging to observe behavior.

##### Step 2.3: Hypothesis Formation

**Likely causes** (in order of probability):

1. **Missing mode check**: Code ticks accumulator without checking `triggerMode()`
2. **Logic error**: Mode check inverted or incorrect comparison
3. **Multiple calls**: `triggerStep()` called once per pulse instead of conditionally
4. **Wrong check location**: Mode checked but at wrong point in execution flow

---

#### Phase 3: Fix the Bug (GREEN - Minimal Fix)

**Goal**: Make the failing tests pass with minimal code changes

##### Suspected Fix (Hypothesis 1): Add Missing Mode Checks

**File**: `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Current code** (around line 353, STEP mode):
```cpp
// STEP mode: Tick once per step (existing logic)
if (step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Problem**: This code likely gets executed on EVERY call to `triggerStep()`, which happens once per pulse.

**Fix**: Only execute when `_pulseCounter == 1` (first pulse of step):

```cpp
// STEP mode: Tick once per step (FIXED)
if (_pulseCounter == 1 &&  // NEW: Only on first pulse
    step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Current code** (around line 392, GATE mode):
```cpp
// GATE mode: Tick per gate fired
if (step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Gate) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Problem**: This code likely gets executed regardless of whether gate actually fires.

**Fix**: Only execute when gate actually fires (inside `if (shouldFireGate)` block):

```cpp
// GATE mode: Tick per gate fired (FIXED)
if (shouldFireGate &&  // NEW: Only when gate fires
    step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Gate) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

##### Implementation Steps

1. **Locate exact buggy code** via Step 2 investigation
2. **Apply minimal fix** based on root cause
3. **Verify tests pass**:
   ```bash
   cd build/sim/debug
   make -j TestAccumulatorTriggerModes
   ./src/tests/integration/TestAccumulatorTriggerModes
   # Expected: All tests PASS
   ```

---

#### Phase 4: Refactor (Clean Up Code)

**Goal**: Improve code clarity after fix works

##### Step 4.1: Extract Helper Method

```cpp
// In NoteTrackEngine.cpp
void NoteTrackEngine::tickAccumulatorIfNeeded(
    const NoteSequence::Step &step,
    const NoteSequence &sequence,
    bool useFillSequence,
    Accumulator::TriggerMode mode,
    bool condition) {

    if (!condition || !step.isAccumulatorTrigger()) {
        return;
    }

    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == mode) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}

// Usage:
tickAccumulatorIfNeeded(step, sequence, useFillSequence,
                        Accumulator::Step, _pulseCounter == 1);

tickAccumulatorIfNeeded(step, sequence, useFillSequence,
                        Accumulator::Gate, shouldFireGate);
```

##### Step 4.2: Add Inline Comments

```cpp
// STEP mode: Tick once per step (only on first pulse)
tickAccumulatorIfNeeded(...);

// GATE mode: Tick per gate fired (respects gate mode)
tickAccumulatorIfNeeded(...);

// RTRIG mode: Tick N times for N retriggers (all at step start)
// [existing RTRIG code]
```

---

#### Phase 5: Regression Testing (Verify No Breakage)

**Goal**: Ensure fix doesn't break existing functionality

##### Test 5.1: All Existing Unit Tests

```bash
cd build/sim/debug
make -j test_all
# All 15+ accumulator tests should still pass
```

##### Test 5.2: Manual Simulator Testing

Test scenarios:
1. **STEP mode** with various pulse counts (0-7)
2. **GATE mode** with all gate modes (ALL/FIRST/HOLD/FIRSTLAST)
3. **RTRIG mode** with various retrigger counts (1-7)
4. **Combinations**: pulse count + gate mode + retrigger
5. **Fill sequences**: Test with fill mode active

##### Test 5.3: Hardware Testing

Flash to hardware and verify:
- STEP mode ticks once per step ✓
- GATE mode respects gate mode setting ✓
- RTRIG mode still works ✓
- No crashes or unexpected behavior ✓

---

### Bug #2: Missing Playhead in ACCST Page

**Issue**: AccumulatorStepsPage does not show running playhead indicator during playback.

---

### TDD Fix Plan - Bug #2: Add Playhead to ACCST Page

#### Phase 1: Research Existing Playhead Implementation (Analysis)

**Goal**: Understand how other pages implement playhead

##### Step 1.1: Study NoteSequenceEditPage

**File**: `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp`

**Find**:
1. How playhead position is obtained (e.g., `_engine.state().currentStep()`)
2. How playhead is rendered (drawing code)
3. How page subscribes to step updates
4. Refresh rate / update mechanism

**Key code to extract**:
```cpp
// Playhead rendering (example)
int currentStep = _sequence.currentStep();  // Or similar
canvas.setColor(Color::Bright);
canvas.drawRect(stepX, stepY, stepWidth, stepHeight);
```

##### Step 1.2: List Required Components

From research, playhead likely needs:
- [ ] Access to current step index
- [ ] Real-time update subscription
- [ ] Drawing code in `draw()` method
- [ ] Visual style (color, shape)

---

#### Phase 2: Implement Playhead (TDD)

##### Test 2.1: Playhead Position Test (Conceptual)

```cpp
// Note: UI tests are harder to automate - may be manual verification
TEST_CASE("AccumulatorStepsPage shows current step") {
    // Setup: Sequence playing at step 3
    // Open ACCST page
    // Verify: Step 3 has visual indicator
    // (Manual test or UI automation framework needed)
}
```

##### Step 2.2: Add Playhead Rendering (GREEN)

**File**: `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp`

**Modify `draw()` method**:

```cpp
void AccumulatorStepsPage::draw(Canvas &canvas) {
    // ... existing drawing code ...

    // NEW: Draw playhead indicator
    int currentStep = /* get current step from engine or sequence state */;

    if (currentStep >= 0 && currentStep < 16) {
        // Highlight current step
        int x = /* calculate x position for step */;
        int y = /* calculate y position */;

        canvas.setColor(Color::Bright);  // Or appropriate color
        canvas.fillRect(x, y, width, height);
        // Or: canvas.drawRect(...) for outline
    }

    // ... rest of drawing ...
}
```

##### Step 2.3: Subscribe to Updates

Ensure page refreshes in real-time:

```cpp
// In update() method or event handler
void AccumulatorStepsPage::update() {
    // Check if step changed
    // Mark page as dirty / request redraw
}
```

---

#### Phase 3: Visual Testing (Manual)

**Goal**: Verify playhead looks correct and moves smoothly

##### Test Scenarios:

1. **Start playback** → Playhead appears
2. **Stop playback** → Playhead disappears or freezes
3. **Step forward** → Playhead advances
4. **Loop sequence** → Playhead wraps around
5. **Change pattern** → Playhead resets correctly

**Visual QA**:
- Playhead color distinct from other elements ✓
- Animation smooth (not flickering) ✓
- Consistent with other pages ✓

---

### Implementation Checklist

**Bug #1: Trigger Modes** (Priority: 🔴 P0)
- [ ] Phase 1: Write failing integration tests
- [ ] Phase 2: Investigate root cause via code review
- [ ] Phase 3: Fix trigger mode logic
- [ ] Phase 4: Refactor code for clarity
- [ ] Phase 5: Regression test (sim + hardware)

**Bug #2: Playhead** (Priority: 🟡 P1)
- [ ] Phase 1: Research existing playhead implementation
- [ ] Phase 2: Implement playhead rendering
- [ ] Phase 3: Manual visual testing
- [ ] Hardware verification

---

### Success Criteria

**Bug #1 Fixed When:**
- ✅ STEP mode ticks once per step (verified in tests)
- ✅ GATE mode ticks per gate fired (respects gate mode)
- ✅ RTRIG mode still works correctly
- ✅ All integration tests pass
- ✅ Hardware testing confirms fix

**Bug #2 Fixed When:**
- ✅ ACCST page shows playhead indicator
- ✅ Playhead moves in real-time during playback
- ✅ Visual style consistent with other pages
- ✅ Hardware testing confirms UX improvement

---

### Estimated Effort

**Bug #1 (Trigger Modes)**:
- Investigation: 2-4 hours
- Fix + tests: 3-5 hours
- Testing: 2-3 hours
- **Total: 7-12 hours (1-2 days)**

**Bug #2 (Playhead)**:
- Research: 1-2 hours
- Implementation: 2-3 hours
- Testing: 1-2 hours
- **Total: 4-7 hours (0.5-1 day)**

**Combined: 1.5-3 days** (with testing on hardware)

---

### Key Files

**Bug #1:**
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` - Fix trigger logic
- `src/tests/integration/sequencer/TestAccumulatorTriggerModes.cpp` - NEW integration tests

**Bug #2:**
- `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp` - Add playhead
- `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp` - Reference implementation

**Documentation:**
- `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md` - Detailed bug report
- `CHANGELOG.md` - Update with bug fixes

---

## Pending Features

### To brainstorm

-   **Harmony Feature Next Steps**:
    -   Expose Inversion parameter (0-3) in UI.
    -   Expose Voicing parameter (Close/Drop2/Drop3/Spread) in UI.
    -   (Both are already supported by the underlying `HarmonyEngine`.)

---

## 🧪 EXPERIMENTAL: RTRIG Mode - Spread Accumulator Ticks Over Time (Option 3)

**Status**: ✅ PHASES 0-4 COMPLETE & FULLY TESTED - ⚠️ TESTING IN PROGRESS (flag=1 enabled)

### 🔬 Active Research & Known Issues

**Issue #1: RTRIG Spread Mode Not Applying to Note Pitch**
- **Status**: 🔴 ROOT CAUSE IDENTIFIED - FIX IN PROGRESS
- **Symptom**: Accumulator counter increments correctly, but note pitch does not change during playback
- **Observed**: Simulator testing shows accumulator value updating but CV output remains constant
- **Expected**: CV output should update on EACH retrigger firing (spread mode design intent)

**Root Cause Analysis** (NoteTrackEngine.cpp):
1. ✅ **Line 511 (triggerStep)**: CV calculated ONCE with `evalStepNote()` and pushed to `_cvQueue`
2. ✅ **Lines 466-477 (triggerStep)**: Multiple retrigger gates scheduled with `shouldTickAccum=true`
3. ✅ **Line 236 (tick)**: Accumulator ticks when gate fires from queue (WORKING)
4. ❌ **MISSING**: CV recalculation after accumulator tick - CV never updates!

**The Problem**:
- CV is calculated at step start with OLD accumulator value (line 511)
- Accumulator ticks correctly on each retrigger gate (line 236)
- BUT CV is NEVER recalculated after accumulator ticks
- Result: Pitch doesn't change despite counter incrementing

**Design Intent**:
- SPREAD MODE: CV should recalculate on EACH retrigger firing
- BURST MODE: CV calculated once upfront (all ticks happen immediately)
- User specifically requested spread mode to update CV progressively

**Fix Requirements**:
- After line 236 (accumulator tick), need to:
  1. Recalculate CV using `evalStepNote()` with new accumulator value
  2. Push new CV value to `_cvQueue` with current tick timing
  3. Requires step data (note, scale, transpose, etc.) to call `evalStepNote()`
  4. **Challenge**: Gate struct doesn't store step info, need to add or find alternative

**Next Steps**:
- [ ] Design: Add step data to Gate struct OR store current step reference
- [ ] Implement: CV recalculation logic after accumulator tick (line 237+)
- [ ] Test: Verify CV updates on each retrigger in spread mode
- [ ] Hardware: Test on actual hardware to verify timing correctness

- **Priority**: 🔴 HIGH - Core feature not working as designed

**Issue #2: Accumulator Status Indication on Note Layer Page**
- **Status**: 🟡 DESIGN RESEARCH NEEDED
- **Current**: No visual indication when viewing AccumulatorTrigger layer on STEPS page
- **Problem**: User cannot tell if accumulator is enabled/active without switching to ACCUM page
- **Proposal**: Research alternative indication methods:
  - Option A: Status icon/character in page header
  - Option B: Brightness/color change when accumulator active
  - Option C: Brief on-screen notification when toggling triggers
  - Option D: Footer status indicator (like other pages)
- **Constraints**: OLED display space limitations, noise reduction concerns
- **Next Steps**:
  - Review similar status indicators in other pages
  - Test visual approaches on actual hardware
  - Consider noise reduction impact of additional display elements
- **Priority**: 🟡 MEDIUM - Quality of life improvement

---

**Status**: ✅ PHASES 0-4 IMPLEMENTATION COMPLETE - ⚠️ BEHAVIOR VERIFICATION IN PROGRESS (flag=1 enabled)

**Current Behavior (Working & Stable)**:
- RTRIG mode with retrig=3 → All 3 accumulator ticks fire immediately at step start
- Retrigger gates fire spread over time (you hear 3 distinct ratchets)
- Result: "Burst mode" - step-based accumulator jumps

**Proposed Experimental Behavior**:
- RTRIG mode with retrig=3 → 3 accumulator ticks spread over time
- Tick #1 when first retrigger gate fires
- Tick #2 when second retrigger gate fires
- Tick #3 when third retrigger gate fires
- Result: "Gradual mode" - time-based accumulator changes synchronized with ratchets

**Approach**: Weak Reference with Sequence ID (Option 3 from RTRIG-Timing-Research.md)
- Uses sequence ID (0=main, 1=fill) instead of pointers
- Adds metadata to Gate struct: `shouldTickAccumulator`, `sequenceId`
- Validates sequence on every tick (safe lookup)
- Feature flag: `CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS`

**Risk Assessment**: 🟡 LOWER RISK (Option 3)
- ✅ No dangling pointer crashes (uses ID, not pointer)
- ⚠️ Sequence might be deleted between schedule and fire (validated)
- ⚠️ Memory overhead: Gate struct grows from 8 to 12 bytes
- ⚠️ Requires extensive testing on hardware
- 🔒 Feature flag allows instant rollback if issues arise

**Effort Estimate**: 8-12 days (6 phases)

**Prerequisites**:
- ✅ Read RTRIG-Timing-Research.md (complete technical investigation)
- ✅ Read Queue-BasedAccumTicks.md (full implementation plan with risk analysis)
- ✅ Understand gate queue architecture (NoteTrackEngine.cpp:210-219, 408-437)
- ✅ Understand sequence invalidation edge cases
- ⚠️ Hardware available for stress testing (Phase 5)

---

### 📊 Implementation Summary (Phases 0-4 Complete)

**What's Been Implemented:**
- ✅ **Phase 0**: Feature flag `CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS` in Config.h (default 0)
- ✅ **Phase 1**: Gate struct extended with `shouldTickAccumulator` and `sequenceId` fields (feature-flagged)
- ✅ **Phase 1**: Sequence ID constants `MainSequenceId=0`, `FillSequenceId=1` added
- ✅ **Phase 1**: TestGateStruct.cpp created with comprehensive test coverage
- ✅ **Phase 2**: triggerStep() modified to schedule gates with metadata when flag=1
- ✅ **Phase 2**: Burst mode preserved when flag=0 (backward compatible)
- ✅ **Phase 3**: tick() modified to process accumulator ticks when gates fire (flag=1)
- ✅ **Phase 3**: Sequence ID lookup with validation (prevents crashes)
- ✅ **Phase 4**: changePattern() clears gate queue when flag=1 (prevents stale ticks)
- ✅ **Phase 4**: Edge case validation (null pointers, invalid IDs, sequence changes)

**Current State:**
- Feature flag = 0 (BURST MODE, stable, backward compatible)
- All code changes guarded by `#if CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS`
- Zero impact on existing functionality when flag=0
- Ready for Phase 5 testing when simulator/hardware available

**Testing Results (Comprehensive Unit Tests):**

✅ **TestGateStruct** (Phase 2 verification):
- flag=0: 4/4 tests pass (basic gate struct functionality)
- flag=1: 9/9 tests pass (all experimental fields validated)
- Validates: Gate struct extensions, sequence ID constants, 2/4-arg construction

✅ **TestRTrigAccumTicking** (Phase 3 verification):
- flag=0: 8/8 tests pass (backward compatibility)
- flag=1: 10/10 tests pass (accumulator ticking logic)
- Validates: Gate metadata conditions, tick() processing, delayed first tick, wrap mode

✅ **TestRTrigEdgeCases** (Phase 4 verification):
- flag=0: 2/2 tests pass (basic edge cases)
- flag=1: 12/12 tests pass (all safety checks)
- Validates: Null sequence handling, invalid IDs, queue clearing, memory constraints

**Total Test Coverage**: 31 tests pass with flag=1, all tests pass with flag=0
**Zero Regressions**: Backward compatibility fully verified
**Compilation**: Clean with both flag=0 and flag=1 (no warnings)

**To Enable Spread Mode (Experimental):**
1. Edit `src/apps/sequencer/Config.h`
2. Change `#define CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS 0` → `1`
3. Rebuild firmware
4. Test thoroughly before deploying to hardware

**Commits:**
- `380e02e`: Phase 0-1 - Feature flag and Gate struct extensions
- `39c483a`: Phase 2-3 - Gate scheduling and accumulator ticking
- `608693f`: Phase 4 - Edge case handling and queue management

---

### 📅 Phased TDD Implementation Plan (6 Phases, Feature-Flagged)
