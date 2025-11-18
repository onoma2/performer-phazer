# TDD Fix Plan: Accumulator Trigger Mode Bugs

**Date**: 2025-11-18
**Branch**: `claude/analyze-bugs-tdd-plan-01K25Lt1hnPSeCH3PQjiNoSC`
**Status**: 📋 PLAN READY FOR EXECUTION

---

## Executive Summary

Two critical bugs identified in the accumulator trigger mode implementation:

1. **Bug #1 (P0-CRITICAL)**: STEP and GATE trigger modes not working - all modes behaving like RTRIG
2. **Bug #2 (P1-HIGH)**: Missing playhead indicator in ACCST page

Both bugs will be fixed using Test-Driven Development methodology:
1. Write failing tests that expose the bugs
2. Fix the implementation
3. Verify tests pass
4. Test on hardware

---

## Bug #1: Trigger Mode Logic Broken

### Root Cause Analysis

**Location**: `src/apps/sequencer/engine/NoteTrackEngine.cpp:353-361`

**The Problem**:
```cpp
// Line 353-361: STEP mode logic
if (step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Why It Fails**:
- When `pulseCount=3`, the step plays for 4 pulses (1+3)
- `triggerStep()` is called **4 times** (once per pulse)
- STEP mode code executes **all 4 times** because there's no pulse counter check
- **Expected**: 1 tick per step (only on first pulse)
- **Actual**: 4 ticks (once per pulse)

**The Fix**:
Add pulse counter check: `(_pulseCounter == 1)` to ensure STEP mode only ticks on the first pulse.

---

## Bug #2: Missing Playhead in ACCST Page

### Root Cause Analysis

**Location**: `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp:20-27`

**The Problem**:
```cpp
void AccumulatorStepsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ACCST");
    WindowPainter::drawActiveFunction(canvas, "ACCU STEPS");
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);  // No playhead rendering
}
```

**Why It Fails**:
- Does not query track engine for current step
- Does not render playhead indicator
- Unlike other pages (NOTE, GATE, RETRIG), which all show playhead

**The Fix**:
Add track engine query and playhead rendering similar to `NoteSequenceEditPage.cpp:79-83`.

---

## TDD Implementation Plan

### Phase 1: Write Failing Tests for Bug #1

#### Test 1.1: STEP Mode with Pulse Count
**File**: `src/tests/unit/sequencer/TestAccumulatorTriggerModes.cpp` (NEW)

```cpp
CASE("step_mode_with_pulse_count_ticks_once") {
    // Setup: STEP mode, pulseCount=3 (4 pulses total)
    NoteSequence sequence;
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Step);
    sequence.accumulator().setDirection(Accumulator::Up);
    sequence.accumulator().setStepValue(1);
    sequence.accumulator().setMinValue(0);
    sequence.accumulator().setMaxValue(100);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setAccumulatorTrigger(true);
    step.setPulseCount(3);  // 4 pulses total (1+3)

    // Create mock engine and simulate 4 pulse triggers
    // (Integration test needed - mocking complex)

    // Expected: accumulator ticks ONCE (only first pulse)
    // Actual (bug): accumulator ticks 4 TIMES
    expectEqual(sequence.accumulator().currentValue(), 1,
               "STEP mode should tick once regardless of pulse count");
}
```

#### Test 1.2: GATE Mode with FIRST Gate Mode
**File**: Same file

```cpp
CASE("gate_mode_with_first_gate_mode_ticks_once") {
    // Setup: GATE mode, pulseCount=3, gateMode=FIRST
    NoteSequence sequence;
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Gate);
    sequence.accumulator().setDirection(Accumulator::Up);
    sequence.accumulator().setStepValue(1);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setAccumulatorTrigger(true);
    step.setPulseCount(3);    // 4 pulses total
    step.setGateMode(1);       // FIRST mode

    // Expected: 1 tick (gate fires on first pulse only)
    expectEqual(sequence.accumulator().currentValue(), 1,
               "GATE mode with FIRST should tick once");
}
```

#### Test 1.3: GATE Mode with ALL Gate Mode
**File**: Same file

```cpp
CASE("gate_mode_with_all_gate_mode_ticks_four_times") {
    // Setup: GATE mode, pulseCount=3, gateMode=ALL
    NoteSequence sequence;
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Gate);
    sequence.accumulator().setDirection(Accumulator::Up);
    sequence.accumulator().setStepValue(1);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setAccumulatorTrigger(true);
    step.setPulseCount(3);    // 4 pulses total
    step.setGateMode(0);       // ALL mode

    // Expected: 4 ticks (gate fires on all 4 pulses)
    expectEqual(sequence.accumulator().currentValue(), 4,
               "GATE mode with ALL should tick on every pulse");
}
```

#### Test 1.4: RTRIG Mode with Retrigger Count
**File**: Same file

```cpp
CASE("rtrig_mode_with_retrigger_ticks_n_times") {
    // Setup: RTRIG mode, retrig=3
    NoteSequence sequence;
    sequence.accumulator().setEnabled(true);
    sequence.accumulator().setTriggerMode(Accumulator::Retrigger);
    sequence.accumulator().setDirection(Accumulator::Up);
    sequence.accumulator().setStepValue(1);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setAccumulatorTrigger(true);
    step.setRetrigger(3);

    // Expected: 3 ticks (one per retrigger)
    expectEqual(sequence.accumulator().currentValue(), 3,
               "RTRIG mode should tick N times for N retriggers");
}
```

**Challenge**: These tests require engine integration, not just model layer testing.

**Solution**: Create integration test or mock infrastructure.

---

### Phase 2: Write Failing Tests for Bug #2

#### Test 2.1: Playhead Rendering
**File**: `src/tests/unit/sequencer/ui/TestAccumulatorStepsPagePlayhead.cpp` (NEW)

```cpp
CASE("accst_page_shows_playhead_during_playback") {
    // Setup: Create mock context and page
    // Start playback
    // Set current step to 5
    // Render page

    // Verify: Playhead rendered at step 5
    // (Requires UI testing infrastructure)
}
```

**Challenge**: UI testing is complex and may not have good test infrastructure.

**Alternative Approach**: Manual verification in simulator after code fix.

---

### Phase 3: Fix Bug #1 - Trigger Mode Logic

#### Fix 3.1: STEP Mode Pulse Counter Check
**File**: `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Line 353-361 - BEFORE**:
```cpp
// STEP mode: Tick accumulator once per step
if (step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        // Tick the accumulator - using mutable allows modification through const ref
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Line 353-361 - AFTER**:
```cpp
// STEP mode: Tick accumulator once per step (first pulse only)
if (step.isAccumulatorTrigger() && _pulseCounter == 1) {  // ← ADD PULSE CHECK
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        // Tick the accumulator - using mutable allows modification through const ref
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Explanation**:
- `_pulseCounter` starts at 1 for the first pulse
- Adding `&& _pulseCounter == 1` ensures STEP mode only ticks once
- Subsequent pulses (2, 3, 4) skip the STEP mode block
- GATE and RTRIG modes are unaffected (they already have correct logic)

---

### Phase 4: Fix Bug #2 - Playhead Rendering

#### Fix 4.1: Add Track Engine Query and Playhead
**File**: `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp`

**Current draw() method**:
```cpp
void AccumulatorStepsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ACCST");
    WindowPainter::drawActiveFunction(canvas, "ACCU STEPS");
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);
}
```

**Fixed draw() method**:
```cpp
void AccumulatorStepsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ACCST");
    WindowPainter::drawActiveFunction(canvas, "ACCU STEPS");
    WindowPainter::drawFooter(canvas);

    // Get current step from track engine for playhead rendering
    const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
    const auto &sequence = _project.selectedNoteSequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    // Draw list with playhead support
    ListPage::draw(canvas);

    // Draw playhead indicator if playing
    if (currentStep >= 0) {
        // Calculate playhead position and render
        // (Implementation depends on ListPage layout)
        // May need to modify AccumulatorStepsListModel to support playhead
    }
}
```

**Alternative Approach**:
Modify `AccumulatorStepsListModel` to accept and render current step, similar to how other sequence pages work.

**Implementation Decision**:
Check how other list-based pages handle playhead. May need to:
1. Pass `currentStep` to `_listModel`
2. Add playhead rendering to `AccumulatorStepsListModel::cell()` method
3. Highlight the active step differently

---

### Phase 5: Verify Tests Pass

#### Step 5.1: Build and Run Unit Tests
```bash
cd build/sim/debug
make -j
cd src/tests/unit
./unit_tests --test "Accumulator"
```

**Expected Results**:
- All existing tests still pass ✅
- New tests for trigger modes pass ✅

#### Step 5.2: Build and Test in Simulator
```bash
cd build/sim/debug
make -j
./src/apps/sequencer/sequencer
```

**Manual Test Cases**:

**Test Case 1: STEP Mode with Pulse Count**
1. Create new sequence
2. Navigate to ACCUM page
3. Set: ENABLE=On, TRIG=STEP, DIR=Up, MIN=0, MAX=10, STEP=1
4. Navigate to ACCST page, enable trigger on step 1
5. Navigate to STEPS page, set step 1: pulseCount=3, gate=On
6. Press PLAY
7. **Expected**: After step 1 completes (4 pulses), accumulator value = 1
8. **Bug behavior**: accumulator value = 4

**Test Case 2: GATE Mode with FIRST**
1. Same setup, but TRIG=GATE
2. Set step 1: pulseCount=3, gateMode=FIRST
3. Press PLAY
4. **Expected**: accumulator value = 1 (one gate fired)

**Test Case 3: GATE Mode with ALL**
1. Same setup, TRIG=GATE
2. Set step 1: pulseCount=3, gateMode=ALL
3. Press PLAY
4. **Expected**: accumulator value = 4 (four gates fired)

**Test Case 4: ACCST Playhead**
1. Create sequence with 8 steps
2. Press PLAY
3. Navigate to NOTE page → playhead visible ✅
4. Navigate to ACCST page → playhead visible ✅ (AFTER FIX)

---

### Phase 6: Hardware Testing

#### Step 6.1: Build and Flash
```bash
cd build/stm32/release
make -j sequencer
make flash_sequencer
```

#### Step 6.2: Reproduce Original Bug Reports
- Follow exact steps from `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md`
- Verify both bugs are fixed

#### Step 6.3: Regression Testing
- Test all accumulator features
- Test pulse count feature
- Test gate mode feature
- Test retrigger feature
- Ensure no existing functionality broken

---

## Implementation Order

1. ✅ **Write this TDD plan document**
2. ⏳ **Create test file**: `TestAccumulatorTriggerModes.cpp`
3. ⏳ **Write failing tests** for Bug #1 (may require integration test infrastructure)
4. ⏳ **Fix Bug #1**: Add `_pulseCounter == 1` check to STEP mode logic
5. ⏳ **Verify tests pass** in simulator
6. ⏳ **Fix Bug #2**: Add playhead rendering to AccumulatorStepsPage
7. ⏳ **Manual verification** in simulator
8. ⏳ **Hardware testing**: Flash and verify on actual hardware
9. ⏳ **Commit and document**: Update CHANGELOG and TODO

---

## Test Coverage Matrix

| Feature | Model Test | Engine Test | UI Test | Simulator | Hardware |
|---------|-----------|-------------|---------|-----------|----------|
| STEP mode basic | ✅ Exists | ❌ Missing | N/A | ⏳ TODO | ⏳ TODO |
| STEP + pulse count | ❌ Missing | ❌ Missing | N/A | ⏳ TODO | ⏳ TODO |
| GATE mode basic | ✅ Exists | ❌ Missing | N/A | ⏳ TODO | ⏳ TODO |
| GATE + gate modes | ❌ Missing | ❌ Missing | N/A | ⏳ TODO | ⏳ TODO |
| RTRIG mode basic | ✅ Exists | ❌ Missing | N/A | ⏳ TODO | ⏳ TODO |
| ACCST playhead | N/A | N/A | ❌ Missing | ⏳ TODO | ⏳ TODO |

**Key Insight**: Existing unit tests only cover model layer (Accumulator class), not engine integration. This is why the bugs weren't caught.

---

## Risk Assessment

### Bug #1 Fix Risks

**Risk Level**: 🟢 LOW

**Reasoning**:
- Single line change (`&& _pulseCounter == 1`)
- Well-understood logic
- Clear expected behavior
- Doesn't affect GATE or RTRIG modes

**Mitigation**:
- Comprehensive testing in simulator
- Hardware verification before release

### Bug #2 Fix Risks

**Risk Level**: 🟡 MODERATE

**Reasoning**:
- UI changes can have subtle rendering issues
- May need to modify list model interface
- Playhead rendering needs to match other pages

**Mitigation**:
- Reference `NoteSequenceEditPage` implementation closely
- Test with various step counts and play modes
- Verify performance (no frame drops)

---

## Success Criteria

### Bug #1 Fixed ✅
- [ ] STEP mode ticks once per step (independent of pulse count)
- [ ] GATE mode ticks per gate fired (respects gate mode settings)
- [ ] RTRIG mode ticks N times for N retriggers (unchanged)
- [ ] All unit tests pass
- [ ] Manual simulator testing confirms correct behavior
- [ ] Hardware testing confirms correct behavior

### Bug #2 Fixed ✅
- [ ] ACCST page shows playhead during playback
- [ ] Playhead position updates in real-time
- [ ] Visual consistency with other sequence pages
- [ ] No performance degradation

---

## Timeline Estimate

| Phase | Estimated Time | Notes |
|-------|---------------|-------|
| Write tests | 1 hour | May need integration test infrastructure |
| Fix Bug #1 | 15 minutes | Single line change |
| Fix Bug #2 | 1 hour | UI work requires careful implementation |
| Simulator testing | 30 minutes | Manual test cases |
| Hardware testing | 30 minutes | Flash and verify |
| Documentation | 30 minutes | Update CHANGELOG, TODO |
| **Total** | **~3.5 hours** | Assumes no complications |

---

## Testing Infrastructure Gaps

### Current Gaps Identified

1. **No engine integration tests** for accumulator trigger modes
   - Existing tests mock at model layer
   - Need tests that exercise actual engine logic

2. **No UI testing infrastructure** for playhead rendering
   - Manual verification required
   - Consider adding screenshot comparison tests

3. **No automated hardware testing**
   - All hardware testing is manual
   - Consider CI/CD with hardware-in-the-loop

### Recommendations for Future

1. **Add integration test suite** for engine layer
   - Test engine + model interaction
   - Mock only platform layer (timers, I/O)

2. **Add UI snapshot testing**
   - Capture canvas renders
   - Compare against golden images
   - Detect visual regressions

3. **Expand unit test coverage**
   - Test all trigger mode combinations
   - Test edge cases (wrap, overflow, etc.)

---

## Files to Modify

### Bug #1 Fix
- ✏️ `src/apps/sequencer/engine/NoteTrackEngine.cpp` (line ~353)
- 📝 `src/tests/unit/sequencer/TestAccumulatorTriggerModes.cpp` (NEW)

### Bug #2 Fix
- ✏️ `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp` (draw method)
- ✏️ `src/apps/sequencer/ui/model/AccumulatorStepsListModel.h` (possibly)
- ✏️ `src/apps/sequencer/ui/model/AccumulatorStepsListModel.cpp` (possibly)

### Documentation
- 📝 `CHANGELOG.md`
- 📝 `TODO.md`
- 📝 `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md` (mark as FIXED)

---

## Code Review Checklist

Before marking as complete:

- [ ] Code follows TDD RED-GREEN-REFACTOR cycle
- [ ] All unit tests pass
- [ ] Integration/manual testing completed in simulator
- [ ] Hardware testing confirms fixes
- [ ] No regression in existing features
- [ ] Code is well-commented
- [ ] Performance impact measured (should be negligible)
- [ ] Documentation updated
- [ ] Commit messages are clear and reference bug numbers

---

## Alternative Approaches Considered

### Bug #1 Alternative: Refactor triggerStep()

**Idea**: Split `triggerStep()` into separate methods for first pulse vs. subsequent pulses.

**Pros**:
- Cleaner separation of concerns
- Easier to understand logic flow

**Cons**:
- Much larger change
- Higher risk of introducing new bugs
- Not necessary for the fix

**Decision**: ❌ Rejected - Single line fix is simpler and safer

### Bug #2 Alternative: Separate Playhead Component

**Idea**: Create reusable playhead rendering component for all pages.

**Pros**:
- DRY principle
- Consistent rendering across pages

**Cons**:
- Larger scope
- Not directly related to bug fix
- Each page has different layout needs

**Decision**: ❌ Deferred - Can refactor later if needed

---

## Conclusion

Both bugs have clear root causes and straightforward fixes:

**Bug #1**: Missing pulse counter check in STEP mode logic
- **Fix**: Add `&& _pulseCounter == 1` condition
- **Risk**: Low
- **Effort**: Minimal

**Bug #2**: Missing playhead rendering in ACCST page
- **Fix**: Add track engine query and playhead drawing
- **Risk**: Moderate (UI changes)
- **Effort**: Low-moderate

The TDD approach ensures:
1. Bugs are reproducible via tests
2. Fixes are verifiable
3. No regressions introduced
4. Future refactoring is safer

**Next step**: Begin implementation starting with Phase 2 (write failing tests).

---

**Document Version**: 1.0
**Author**: Claude (AI Assistant)
**Review Status**: Ready for implementation
**Last Updated**: 2025-11-18
