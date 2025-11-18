# Bug Analysis Summary

**Date**: 2025-11-18
**Analyzer**: Claude Code
**Status**: ✅ Analysis Complete - Ready for Implementation

---

## Quick Summary

Two bugs found in accumulator trigger mode implementation:

| Bug | Severity | Root Cause | Fix Complexity |
|-----|----------|------------|----------------|
| #1: Trigger modes broken | 🔴 CRITICAL | Missing pulse counter check | 🟢 Simple (1 line) |
| #2: Missing playhead | 🟡 HIGH | Missing rendering logic | 🟡 Moderate (UI work) |

**Estimated fix time**: 3.5 hours total

---

## Bug #1: Trigger Modes Not Working

### The Problem
**File**: `src/apps/sequencer/engine/NoteTrackEngine.cpp:353-361`

When a step has `pulseCount=3` (plays for 4 pulses total), the function `triggerStep()` is called **4 times** - once per pulse. The STEP mode logic executes on **every call**, causing the accumulator to tick 4 times instead of once.

### Why It Happens
```cpp
// Line 353-361 - CURRENT BROKEN CODE
if (step.isAccumulatorTrigger()) {
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();  // ← Ticks every pulse!
    }
}
```

**Missing**: Check if this is the first pulse (`_pulseCounter == 1`)

### The Fix
```cpp
// Line 353-361 - FIXED CODE
if (step.isAccumulatorTrigger() && _pulseCounter == 1) {  // ← ADD THIS CHECK
    const auto &targetSequence = useFillSequence ? *_fillSequence : sequence;
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

### Impact
- **Lines changed**: 1
- **Risk level**: 🟢 LOW (isolated change)
- **Side effects**: None (GATE and RTRIG modes already have correct logic)

---

## Bug #2: Missing Playhead in ACCST Page

### The Problem
**File**: `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp:20-27`

The ACCST page (accumulator steps) doesn't show which step is currently playing, unlike all other sequence editing pages (NOTE, GATE, RETRIG, etc.).

### Why It Happens
```cpp
// Current draw() method - NO PLAYHEAD
void AccumulatorStepsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ACCST");
    WindowPainter::drawActiveFunction(canvas, "ACCU STEPS");
    WindowPainter::drawFooter(canvas);

    ListPage::draw(canvas);  // ← No current step info passed
}
```

**Missing**:
1. Query track engine for current step
2. Pass current step to rendering logic
3. Draw playhead indicator

### The Fix
Add track engine query and playhead rendering (reference: `NoteSequenceEditPage.cpp:79-83`):

```cpp
void AccumulatorStepsPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ACCST");
    WindowPainter::drawActiveFunction(canvas, "ACCU STEPS");
    WindowPainter::drawFooter(canvas);

    // Get current step for playhead rendering
    const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
    const auto &sequence = _project.selectedNoteSequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    // TODO: Pass currentStep to list model or draw playhead indicator
    ListPage::draw(canvas);
}
```

May also need to modify `AccumulatorStepsListModel` to highlight the current step.

### Impact
- **Lines changed**: ~10-20
- **Risk level**: 🟡 MODERATE (UI changes need careful testing)
- **Side effects**: None (visual improvement only)

---

## Root Cause: Testing Gap

### Why Weren't These Caught?

**Unit tests exist and pass** ✅ but they only test the **model layer**:
- `TestAccumulator.cpp` tests the `Accumulator` class directly
- Does NOT test the engine integration in `NoteTrackEngine.cpp`

**Missing test coverage**:
- ❌ No engine integration tests for trigger modes
- ❌ No tests that exercise `triggerStep()` with pulse count
- ❌ No UI tests for playhead rendering

### Lesson Learned
> Unit tests that mock behavior can pass while the actual integration is broken. Need better **integration test coverage** for engine layer.

---

## Implementation Strategy (TDD)

### Phase 1: Write Failing Tests ⏳
Create `TestAccumulatorTriggerModes.cpp` with:
- Test STEP mode with pulse count (should tick once, currently ticks 4x)
- Test GATE mode with different gate modes
- Test RTRIG mode (should still work)

**Challenge**: May need integration test infrastructure (engine + model testing)

### Phase 2: Fix Bug #1 ⏳
- **File**: `NoteTrackEngine.cpp:353`
- **Change**: Add `&& _pulseCounter == 1` condition
- **Verify**: Tests now pass

### Phase 3: Fix Bug #2 ⏳
- **File**: `AccumulatorStepsPage.cpp`
- **Change**: Add current step tracking and playhead rendering
- **Verify**: Manual testing in simulator

### Phase 4: Hardware Testing ⏳
- Flash to STM32 hardware
- Reproduce original bug scenarios
- Verify both fixes work correctly

---

## Verification Checklist

### Bug #1 Fixed ✅
- [ ] STEP mode: Ticks once per step (regardless of pulse count)
- [ ] GATE mode + FIRST: Ticks once (first pulse only)
- [ ] GATE mode + ALL: Ticks 4 times (all 4 pulses)
- [ ] RTRIG mode: Ticks N times (unchanged behavior)
- [ ] Unit tests pass
- [ ] Simulator testing confirms fix
- [ ] Hardware testing confirms fix

### Bug #2 Fixed ✅
- [ ] ACCST page shows playhead indicator
- [ ] Playhead moves in real-time during playback
- [ ] Visual consistency with other pages
- [ ] No performance issues

---

## Files to Modify

### Code Changes
- ✏️ `src/apps/sequencer/engine/NoteTrackEngine.cpp` (Bug #1)
- ✏️ `src/apps/sequencer/ui/pages/AccumulatorStepsPage.cpp` (Bug #2)
- 📝 `src/tests/unit/sequencer/TestAccumulatorTriggerModes.cpp` (NEW)

### Documentation
- 📝 `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md` (mark as FIXED)
- 📝 `CHANGELOG.md`
- 📝 `TODO.md`

---

## Timeline

| Task | Duration | Status |
|------|----------|--------|
| Bug analysis | 30 min | ✅ Done |
| TDD plan creation | 30 min | ✅ Done |
| Write failing tests | 1 hour | ⏳ Next |
| Fix Bug #1 | 15 min | ⏳ TODO |
| Fix Bug #2 | 1 hour | ⏳ TODO |
| Simulator testing | 30 min | ⏳ TODO |
| Hardware testing | 30 min | ⏳ TODO |
| Documentation | 30 min | ⏳ TODO |
| **TOTAL** | **~4 hours** | |

---

## Next Steps

1. **Review this analysis** with team/maintainer
2. **Implement Phase 1**: Write failing tests
3. **Implement Phase 2**: Fix Bug #1 (1-line change)
4. **Implement Phase 3**: Fix Bug #2 (UI work)
5. **Test thoroughly** in simulator and hardware
6. **Commit and push** to branch

---

## Key Insights

### Bug #1 (Trigger Modes)
- **Symptom**: All trigger modes behave like RTRIG
- **Actual cause**: STEP mode has no pulse counter gate
- **Fix simplicity**: ⭐⭐⭐⭐⭐ (trivial 1-line fix)
- **Fix safety**: ⭐⭐⭐⭐⭐ (isolated, no side effects)

### Bug #2 (Playhead)
- **Symptom**: Can't see playback position in ACCST page
- **Actual cause**: Missing track engine query and rendering
- **Fix simplicity**: ⭐⭐⭐ (requires UI understanding)
- **Fix safety**: ⭐⭐⭐⭐ (visual only, low risk)

---

**Confidence Level**: 🟢 HIGH

Both bugs have been thoroughly analyzed and have clear, straightforward fixes. The TDD approach will ensure no regressions are introduced.

---

**Document Version**: 1.0
**References**:
- `TDD-FIX-PLAN-ACCUMULATOR-BUGS.md` (detailed implementation plan)
- `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md` (original bug report)
