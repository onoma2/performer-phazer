# TDD Plan: Bug #2 - AccumulatorTrigger Layer Playhead

**Date**: 2025-11-18
**Branch**: `claude/analyze-bugs-tdd-plan-01K25Lt1hnPSeCH3PQjiNoSC`
**Status**: 📋 PLAN READY

---

## Bug Understanding

**Location**: `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp:139-148`

**What's Wrong:**
When viewing the **AccumulatorTrigger layer** in the STEPS page (press F3/Note button to cycle to it), the current playing step is NOT highlighted. All other layers show the playhead, but AccumulatorTrigger doesn't.

**Why It Happens:**
The `case Layer::AccumulatorTrigger:` rendering code doesn't check `currentStep` variable. It always uses the same colors regardless of playback position.

---

## Current Code (BROKEN)

```cpp
// Line 139-148 in NoteSequenceEditPage.cpp
case Layer::AccumulatorTrigger:
    // Draw accumulator trigger like gate - filled square when enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);  // ❌ Always bright
        canvas.fillRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    } else {
        canvas.setColor(Color::Medium);  // ❌ Always medium
        canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    }
    break;
```

**Problem**: No `stepIndex == currentStep` check

---

## Reference Code (WORKING)

The Gate layer (lines 116-122) shows the correct pattern:

```cpp
// step gate
canvas.setColor(stepIndex == currentStep ? Color::Bright : Color::Medium);  // ✅ Checks currentStep
canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
if (step.gate()) {
    canvas.setColor(_context.model.settings().userSettings().get<DimSequenceSetting>(SettingDimSequence)->getValue() ? Color::Low : Color::Bright);
    canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
}
```

---

## Proposed Fix

### Option 1: Draw outline with playhead highlighting (RECOMMENDED)

Similar to Gate layer - draw outline first, then fill:

```cpp
case Layer::AccumulatorTrigger:
    // Draw outline with playhead highlighting
    canvas.setColor(stepIndex == currentStep ? Color::Bright : Color::Medium);
    canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);

    // Fill if trigger enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);
        canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
    }
    break;
```

**Visual Result:**
- Current step: Bright outline
- Non-current steps: Medium outline
- Enabled triggers: Filled inner square
- Disabled triggers: Empty (just outline)

### Option 2: Brighter fill for current step

Keep filled square but make current step extra bright:

```cpp
case Layer::AccumulatorTrigger:
    if (step.isAccumulatorTrigger()) {
        // Extra bright for current step
        canvas.setColor((stepIndex == currentStep) ? Color::Bright : Color::Medium);
        canvas.fillRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    } else {
        // Outline brighter for current step
        canvas.setColor((stepIndex == currentStep) ? Color::Bright : Color::Low);
        canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    }
    break;
```

**Visual Result:**
- Current enabled: Bright fill
- Non-current enabled: Medium fill
- Current disabled: Bright outline
- Non-current disabled: Low outline

---

## Recommendation: Option 1

**Why?**
- Matches Gate layer pattern exactly
- Clear visual consistency
- Outline always shows playhead position
- Fill shows trigger state
- Two independent visual indicators (outline=position, fill=enabled)

---

## TDD Implementation Plan

### Phase 1: Manual Testing (Current Broken State)

**Setup:**
1. Build simulator: `cd build/sim/debug && make -j`
2. Run: `./src/apps/sequencer/sequencer`
3. Create sequence with steps 1-8
4. Enable accumulator triggers on steps 1, 3, 5, 7
5. Start playback (PLAY button)

**Test Current Behavior:**
1. Navigate to STEPS page
2. Press F3 (Note button) repeatedly to cycle layers
3. Find "ACCUMULATOR TRIGGER" layer
4. **Observe**: All enabled steps look the same (bright filled squares)
5. **Observe**: All disabled steps look the same (medium outline)
6. **Problem**: Can't tell which step is currently playing

**Compare with Gate Layer:**
1. Press F3 to cycle to GATE layer
2. **Observe**: Current step has bright outline, others have medium outline
3. **This is what AccumulatorTrigger should do**

### Phase 2: Write Failing Test (Optional)

Since this is UI rendering, unit tests are difficult. Manual testing is primary verification.

**If integration test infrastructure exists:**
```cpp
TEST("AccumulatorTrigger layer highlights current step") {
    // Setup sequence with accumulator triggers
    // Start playback at step 3
    // Render AccumulatorTrigger layer
    // Verify step 3 rendered differently than others
}
```

### Phase 3: Implement Fix

**File**: `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp`
**Lines**: 139-148

**Change:**
```cpp
case Layer::AccumulatorTrigger:
    // Draw outline with playhead highlighting (like Gate layer)
    canvas.setColor(stepIndex == currentStep ? Color::Bright : Color::Medium);
    canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);

    // Fill if trigger enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);
        canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
    }
    break;
```

**Lines changed**: 8 lines (2 removed, 6 added)

### Phase 4: Verify Fix (Manual Testing)

**Test in Simulator:**
1. Build: `cd build/sim/debug && make -j`
2. Run: `./src/apps/sequencer/sequencer`
3. Same setup as Phase 1
4. Navigate to AccumulatorTrigger layer
5. **Verify**: Current step has bright outline
6. **Verify**: Non-current steps have medium outline
7. **Verify**: Enabled triggers have filled inner square
8. **Verify**: Playhead moves as sequence plays

**Expected Visual:**
```
Step 1 (enabled, not playing):  [▪]  Medium outline, bright fill
Step 2 (disabled, playing):     [▫]  Bright outline, empty
Step 3 (enabled, playing):      [▪]  Bright outline, bright fill
Step 4 (disabled, not playing): [ ]  Medium outline, empty
```

### Phase 5: Test All Layers for Regression

Verify all other layers still work:

| Layer | Playhead Visible? | Test Result |
|-------|-------------------|-------------|
| Gate | ✓ Should work | ⏳ TODO |
| GateProbability | ✓ Should work | ⏳ TODO |
| GateOffset | ✓ Should work | ⏳ TODO |
| Slide | ✓ Should work | ⏳ TODO |
| GateMode | ✓ Should work | ⏳ TODO |
| Retrigger | ✓ Should work | ⏳ TODO |
| RetriggerProbability | ✓ Should work | ⏳ TODO |
| PulseCount | ✓ Should work | ⏳ TODO |
| Length | ✓ Should work | ⏳ TODO |
| LengthVariationRange | ✓ Should work | ⏳ TODO |
| LengthVariationProbability | ✓ Should work | ⏳ TODO |
| Note | ✓ Should work | ⏳ TODO |
| NoteVariationRange | ✓ Should work | ⏳ TODO |
| NoteVariationProbability | ✓ Should work | ⏳ TODO |
| **AccumulatorTrigger** | **✓ FIXED** | **⏳ TODO** |
| Condition | ✓ Should work | ⏳ TODO |

### Phase 6: Hardware Testing

1. Build release: `cd build/stm32/release && make -j sequencer`
2. Flash: `make flash_sequencer`
3. Repeat all manual tests from Phase 4
4. Verify no performance issues
5. Verify with fast tempos (>200 BPM)

---

## Visual Design Specification

### Before Fix ❌

**AccumulatorTrigger Layer (Playing step 3):**
```
[▪]  Step 1 - Enabled   (Bright fill, medium outline)
[ ]  Step 2 - Disabled  (Medium outline)
[▪]  Step 3 - Enabled   (Bright fill, medium outline) ← Can't tell it's playing!
[ ]  Step 4 - Disabled  (Medium outline)
```

### After Fix ✅

**AccumulatorTrigger Layer (Playing step 3):**
```
[▪]  Step 1 - Enabled   (Bright fill, medium outline)
[▫]  Step 2 - Disabled  (Bright outline) ← Current step visible!
[▪]  Step 3 - Enabled   (Bright fill, bright outline) ← Current + enabled!
[ ]  Step 4 - Disabled  (Medium outline)
```

---

## Code Comparison

### Current (Broken)
```cpp
case Layer::AccumulatorTrigger:
    // Draw accumulator trigger like gate - filled square when enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);
        canvas.fillRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    } else {
        canvas.setColor(Color::Medium);
        canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
    }
    break;
```

### Fixed (Option 1 - RECOMMENDED)
```cpp
case Layer::AccumulatorTrigger:
    // Draw outline with playhead highlighting (like Gate layer)
    canvas.setColor(stepIndex == currentStep ? Color::Bright : Color::Medium);
    canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);

    // Fill if trigger enabled
    if (step.isAccumulatorTrigger()) {
        canvas.setColor(Color::Bright);
        canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
    }
    break;
```

**Changes:**
- ✅ Draw outline FIRST with playhead-aware color
- ✅ Then optionally fill inner area if enabled
- ✅ Matches Gate layer pattern exactly
- ✅ Two independent visual signals (outline=position, fill=state)

---

## Risk Assessment

**Risk Level**: 🟢 VERY LOW

**Reasoning:**
- Only affects one layer's rendering
- Proven pattern from Gate layer
- No logic changes, only visual
- No memory allocations
- No performance impact
- Local change, no side effects

**Potential Issues:**
- None identified

---

## Performance Impact

**Expected**: None

**Analysis:**
- Same number of draw calls (outline + optional fill)
- One additional conditional check (`stepIndex == currentStep`)
- Already computed in outer loop (no additional cost)
- No allocations, no loops

**Measured** (if profiler available):
- Frame time: Should remain ~20ms (50 FPS)
- CPU: No measurable increase

---

## Success Criteria

### Must Have ✅
- [ ] Current step visible when viewing AccumulatorTrigger layer
- [ ] Playhead moves in real-time during playback
- [ ] Visual consistency with Gate layer
- [ ] No regression in other layers

### Nice to Have
- [ ] Clear visual distinction between enabled/disabled triggers
- [ ] Clear visual distinction between current/non-current steps
- [ ] Both properties visible simultaneously (current+enabled)

---

## Timeline

| Task | Duration | Status |
|------|----------|--------|
| Understand bug | 15 min | ✅ Done |
| Write TDD plan | 30 min | ✅ Done |
| Implement fix | 5 min | ⏳ Next |
| Manual testing (sim) | 15 min | ⏳ TODO |
| Manual testing (hw) | 15 min | ⏳ TODO |
| Documentation | 10 min | ⏳ TODO |
| **TOTAL** | **~1.5 hours** | |

---

## Files to Modify

### Code Changes
- ✏️ `src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp` (lines 139-148)

### Documentation
- 📝 `BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md` (mark Bug #2 as FIXED)
- 📝 `CHANGELOG.md` (add fix note)

---

## Commit Message Template

```
Fix: Add playhead indicator to AccumulatorTrigger layer (Bug #2)

When viewing the AccumulatorTrigger layer in STEPS page (press F3/Note to
cycle layers), the current playing step is now highlighted with a bright
outline, matching the visual behavior of the Gate layer.

Changes:
- Draw step outline first with playhead-aware color
- Then optionally fill inner square if trigger enabled
- Uses same pattern as Gate layer for consistency

Visual:
- Current step: Bright outline
- Non-current steps: Medium outline
- Enabled triggers: Filled inner square
- Both indicators work together (outline + fill)

Bug details:
- Bug #2 from BUG-REPORT-ACCUMULATOR-TRIGGER-MODES.md
- Root cause: No currentStep check in AccumulatorTrigger rendering
- Impact: Users couldn't see playback position in this layer
- Fix: 8 lines changed in NoteSequenceEditPage.cpp

Testing:
- Manual verification in simulator required
- Hardware testing recommended
- Risk: VERY LOW (isolated visual change)

Files modified:
- src/apps/sequencer/ui/pages/NoteSequenceEditPage.cpp (lines 139-148)
```

---

## Next Steps

1. ⏳ **Implement fix** - Modify NoteSequenceEditPage.cpp line 139-148
2. ⏳ **Test in simulator** - Manual verification
3. ⏳ **Test on hardware** - Flash and verify
4. ⏳ **Commit and push** - Use template above
5. ⏳ **Update bug report** - Mark Bug #2 as FIXED
6. 📋 **Move to Bug #1** - Trigger mode logic (CRITICAL priority)

---

**Document Version**: 1.0
**Author**: Claude (AI Assistant)
**Status**: Ready for implementation
**Last Updated**: 2025-11-18
