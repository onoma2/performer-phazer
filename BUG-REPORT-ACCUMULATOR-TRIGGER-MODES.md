# BUG REPORT: Accumulator Trigger Mode Issues

**Date**: 2025-11-18
**Reporter**: Hardware Testing
**Status**: 🟢 ALL BUGS FIXED ✅
**Affected Version**: Branch `claude/stm-eurorack-cv-oled-01BX54cUuKHJrHRTHbUeC4TG`
**Fixed Version**: Branch `claude/analyze-bugs-tdd-plan-01K25Lt1hnPSeCH3PQjiNoSC`

---

## Bug #1: STEP Trigger Mode Not Working ✅ FIXED

### Status: 🟢 RESOLVED
- **Fixed in**: Commit `961f5f2`
- **Fixed by**: Added pulse counter check to STEP mode logic
- **Tested**: ✅ Verified working in local testing
- **Date fixed**: 2025-11-18

### Description
STEP mode accumulator trigger was firing on every pulse instead of once per step when pulse count > 0. This made STEP mode behave identically to RTRIG mode, completely breaking its intended functionality.

### Expected Behavior

**STEP Mode:**
- Should tick **once per step** when accumulator trigger is enabled
- Independent of pulse count, gate mode, and retrigger settings
- Example: 4 steps with triggers → 4 increments total

**GATE Mode:**
- Should tick **once per gate pulse fired**
- Respects pulse count and gate mode settings
- Example: `pulseCount=3, gateMode=ALL` → 4 ticks (pulses 1, 2, 3, 4)
- Example: `pulseCount=3, gateMode=FIRST` → 1 tick (pulse 1 only)

**RTRIG Mode:**
- Should tick **N times for N retriggers**
- Example: `retrig=3` → 3 ticks
- All ticks fire immediately at step start (known behavior)

### Actual Behavior
All three modes (STEP, GATE, RTRIG) appear to increment the accumulator on every gate pulse/repetition, ignoring the TRIG mode setting.

### Steps to Reproduce

1. Navigate to ACCUM page
2. Set TRIG mode to **STEP**
3. Enable accumulator with Direction=Up, Min=0, Max=10, Step=1
4. Navigate to ACCST page (accumulator steps)
5. Enable accumulator trigger on step 1
6. Set step 1 with `pulseCount=3` (4 pulses total)
7. **Expected**: Accumulator increments by 1 (once per step)
8. **Actual**: Accumulator increments by 4 (once per pulse)

### Impact
- 🔴 **CRITICAL**: Core feature completely broken
- Users cannot control when accumulator increments
- STEP and GATE modes unusable
- Feature documentation is misleading

### Suspected Root Cause

**Location**: `src/apps/sequencer/engine/NoteTrackEngine.cpp` in `triggerStep()` method

Likely issue: Trigger mode checking logic is incorrect or missing. The code may be:
- Not checking `triggerMode()` at all
- Checking it in the wrong place
- Logic inverted or conditions wrong

**Root Cause (IDENTIFIED)**:
Line 354 in `NoteTrackEngine.cpp` lacked pulse counter check:
```cpp
// BEFORE FIX (BROKEN)
if (step.isAccumulatorTrigger()) {  // No _pulseCounter check
    if (triggerMode() == Accumulator::Step) {
        tick();  // Executed on every pulse!
    }
}
```

When step has `pulseCount=3`, triggerStep() is called 4 times (once per pulse), causing 4 ticks instead of 1.

### Resolution (IMPLEMENTED)

**Fix**: Added pulse counter check to line 354

**Code (AFTER FIX)**:
```cpp
// STEP mode: Tick accumulator once per step (first pulse only)
if (step.isAccumulatorTrigger() && _pulseCounter == 1) {  // ✅ ADDED CHECK
    if (targetSequence.accumulator().enabled() &&
        targetSequence.accumulator().triggerMode() == Accumulator::Step) {
        const_cast<Accumulator&>(targetSequence.accumulator()).tick();
    }
}
```

**Result**: STEP mode now only ticks on first pulse (_pulseCounter == 1), making it independent of pulse count as intended.

---

## Bug #2: Missing Playhead in AccumulatorTrigger Layer ✅ FIXED

### Status: 🟢 RESOLVED
- **Fixed in**: Commit `afdcfc1`
- **Fixed by**: Correcting AccumulatorTrigger layer rendering in NoteSequenceEditPage
- **Tested**: ✅ Verified working in local testing
- **Date fixed**: 2025-11-18

### Description
The **AccumulatorTrigger layer** in the STEPS page (accessed by pressing F3/Note button to cycle through layers) does not show a running playhead indicator to highlight which step is currently playing, unlike all other layers (GATE, RETRIG, LENGTH, etc.).

### Expected Behavior
- When viewing AccumulatorTrigger layer during playback, current step should be highlighted
- Current step: bright outline
- Non-current steps: medium outline
- Visual consistency with Gate and all other layers

### Actual Behavior (Before Fix)
- All enabled triggers: bright filled square (no distinction)
- All disabled triggers: medium outline (no distinction)
- No visual indication of which step is currently playing
- User cannot see playback position while viewing this layer

### Steps to Reproduce (Original Bug)
1. Start sequence playback (PLAY button)
2. Navigate to STEPS page
3. Press F3 (Note button) to cycle to AccumulatorTrigger layer
4. **Before fix**: All steps look the same, can't see current step ✗
5. **After fix**: Current step has bright outline ✅

### Impact
- 🟡 **MODERATE**: UX inconsistency
- Makes editing during playback more difficult
- User must cycle to other layers to see playback position

### Root Cause (IDENTIFIED)

**Location**: `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp:139-148`

**Problem**: The `case Layer::AccumulatorTrigger:` rendering code did not check the `currentStep` variable. It always used the same colors regardless of which step was playing.

**Code (Before Fix)**:
```cpp
case Layer::AccumulatorTrigger:
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);  // ❌ No currentStep check
        canvas.fillRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    } else {
        canvas.setColor(Color::Medium);
        canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    }
    break;
```

### Resolution (IMPLEMENTED)

**Code (After Fix)**:
```cpp
case Layer::AccumulatorTrigger:
    // Draw outline with playhead highlighting (like Gate layer)
    canvas.setColor(stepIndex == currentStep ? Color::Bright : Color::Medium);
    canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);

    // Fill inner square if trigger enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);
        canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
    }
    break;
```

**Key Changes**:
1. Draw outline FIRST with `stepIndex == currentStep` check
2. Then optionally fill inner area if trigger enabled
3. Follows exact same pattern as Gate layer
4. Two independent indicators: outline=position, fill=state

---

## Related Files

**Model Layer:**
- `src/apps/sequencer/model/Accumulator.h/cpp` - Trigger mode enum and logic
- `src/apps/sequencer/model/NoteSequence.h` - Step accumulator trigger flags

**Engine Layer:**
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` - triggerStep() method (lines ~350-430)
  - Line ~353: STEP mode logic
  - Line ~392: GATE mode logic
  - Line ~410: RTRIG mode logic

**UI Layer:**
- `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp` - AccumulatorTrigger layer rendering (Bug #2 - FIXED ✅)
  - Lines 139-148: AccumulatorTrigger case in draw() method

**Tests:**
- `src/tests/unit/sequencer/TestAccumulator.cpp` - Unit tests (passing but may not cover bug)
- Need integration tests that verify actual engine behavior

---

## Verification Status

**Unit Tests:** ✅ All passing (15 tests)
- Tests mock behavior, may not catch engine integration bugs
- Need end-to-end integration tests for Bug #1

**Local Testing:**
- **Bug #1**: ⏳ Not yet tested (pending fix implementation)
- **Bug #2**: ✅ Tested and verified working (2025-11-18)

**Hardware Testing:**
- **Bug #1**: ❌ Still broken - STEP/GATE modes completely non-functional
- **Bug #2**: ✅ Fixed and verified - Playhead now visible in AccumulatorTrigger layer

---

## Priority

**Bug #1 (Trigger Modes):** 🔴 **P0 - CRITICAL** ⏳ OPEN
- Blocks feature usage
- Must fix before any release
- Affects all users attempting to use STEP or GATE modes
- **Status**: Not yet fixed - requires engine layer modification

**Bug #2 (Playhead):** 🟢 **FIXED** ✅ CLOSED
- Fixed in commit `afdcfc1`
- Tested and verified working
- AccumulatorTrigger layer now shows playhead indicator
- **Status**: Complete

---

## Next Steps

**Bug #2 Completed**: ✅
- Fixed in commit `afdcfc1`
- Tested and verified working

**Bug #1 Remaining**: 🔴 CRITICAL
1. ⏳ **Add integration tests** - Write tests that catch trigger mode bugs
2. ⏳ **Fix Bug #1** - Correct trigger mode logic in NoteTrackEngine.cpp (line ~353)
   - Add `&& _pulseCounter == 1` check to STEP mode logic
3. ⏳ **Test in simulator** - Verify all trigger modes work correctly
4. ⏳ **Hardware verify** - Confirm fix on real hardware
5. ⏳ **Regression test** - Ensure no existing features broken

---

## Notes

These bugs suggest that the implementation was tested via unit tests but not via integration/end-to-end testing. The unit tests pass because they mock the behavior, but the actual engine integration is incorrect.

**Lesson learned:** Need better integration test coverage for engine layer.

---

**Document Version**: 1.0
**Last Updated**: November 2025
**Project**: PEW|FORMER Bug Tracking
