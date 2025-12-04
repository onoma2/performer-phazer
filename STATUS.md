# Performer Phazer Project Status

## Current Session: SCALEWALKER Implementation & Subdivision Refactor (Dec 4, 2025)

### Completed Features

**✅ SCALEWALKER Algorithm (Algorithm ID 10)**
- Implemented complete scale-walking algorithm with polyrhythmic subdivision support
- Features:
  - **Flow parameter**: Controls direction (0-7=down, 8=static, 9-16=up)
  - **Ornament parameter**: Controls subdivision (0-4=straight, 5-8=triplets, 9-12=quintuplets, 13-16=septuplets)
  - **Continuous walker**: Advances through scale degrees (0-6) every step
  - **Polyrhythmic micro-gates**: Distributes N gates per beat with sequential scale intervals
  - **Spatial distribution**: Gates spread evenly across beat (not rapid-fire trill)

**✅ Critical Bug Fixes**
1. **Walker State Persistence (Loop Length Bug)**
   - **Problem**: Walker position reset at every loop boundary in finite mode, creating repeating 4-step patterns
   - **Root Cause**: `initAlgorithm()` was resetting `_scalewalker_pos = 0` at loop wrap
   - **Fix**: Removed walker reset from `initAlgorithm()` case 10 (line 124)
   - **Result**: Walker now persists across loop boundaries, providing continuous melodic evolution

2. **Beat Detection Formula**
   - **Problem**: Beat detection inverted, causing polyrhythm to only fire on beat 1
   - **Root Cause**: Formula was `stepsPerBeat = divisor / (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN)`
   - **Fix**: Changed to `stepsPerBeat = (CONFIG_PPQN / divisor)`
   - **Result**: All 4 beats now correctly trigger polyrhythm

3. **Gate Leakage in Polyrhythm Mode**
   - **Problem**: Non-beat steps generated straight gates, adding unwanted 4th gate to triplets
   - **Fix**: Mute non-beat steps in polyrhythm mode (`result.velocity = 0`)
   - **Result**: Clean triplets/quintuplets/septuplets without extra gates

4. **Micro-Gate CV Update Broken**
   - **Problem**: All polyrhythmic gates played same note (walker position at beat start)
   - **Root Cause**: `_retriggerArmed` flag cleared on every step, blocking CV updates for later micro-gates
   - **Fix**: Removed `_retriggerArmed` check from micro-gate CV update (line 966)
   - **Result**: Each micro-gate now plays its correct pitch

5. **CV Sliding in Free Mode**
   - **Problem**: CV updated on muted steps, creating unwanted portamento
   - **Fix**: Added `&& result.velocity > 0` check to Free mode CV update
   - **Result**: CV only updates on steps that actually fire gates

6. **Septuplet Queue Overflow**
   - **Problem**: 7-tuplets dropped gates every 8th beat
   - **Fix**: Increased micro-gate queue size from 16 to 32 entries
   - **Result**: Septuplets play cleanly without dropped gates

**✅ Code Refactoring: Subdivision Unification**
- Moved subdivision calculation to top of `generateStep()` (lines 362-370)
- Eliminated duplicate code from 4 algorithms:
  - **APHEX**: Removed 5 lines of duplicate subdivision calculation
  - **AUTECHRE**: Removed 7 lines, simplified to use global `subdivisions`
  - **STEPWAVE**: Removed 16 lines, removed duplicate ornament reads
  - **SCALEWALKER**: Removed 10 lines, cleaner implementation
- **Total savings**: ~38 lines of duplicated code eliminated
- **Benefit**: All algorithms now use consistent subdivision interpretation

### Algorithm Status Summary

| Algorithm | ID | Status | Subdivision Support | Notes |
|-----------|----|----|---------------------|-------|
| TEST | 0 | ✅ Working | No | Diagnostic algorithm |
| TRITRANCE | 1 | ✅ Working | No | Original Tuesday port |
| STOMPER | 2 | ✅ Working | No | Original Tuesday port |
| APHEX | 3 | ✅ Working | Yes (Polyrhythm) | Uses global subdivisions |
| AUTECHRE | 4 | ✅ Working | Yes (Polyrhythm) | Uses global subdivisions |
| STEPWAVE | 5 | ✅ Working | Yes (Trill) | Uses global subdivisions |
| MARKOV | 6 | ✅ Working | No | Original Tuesday port |
| CHIPARP | 7 | ✅ Working | No | Original Tuesday port |
| CHIPARP2 | 8 | ✅ Working | No | Original Tuesday port |
| WOBBLE | 9 | ✅ Working | No | Original Tuesday port |
| **SCALEWALKER** | **10** | **✅ Working** | **Yes (Polyrhythm)** | **New in this session** |

### Technical Implementation Details

**Subdivision Calculation (Global)**
```cpp
// Line 362-370 in TuesdayTrackEngine.cpp
int ornament = sequence.ornament();
int subdivisions = 4;  // Default: straight 16ths
if (ornament >= 5 && ornament <= 8) subdivisions = 3;      // Triplets (3:4)
else if (ornament >= 9 && ornament <= 12) subdivisions = 5; // Quintuplets (5:4)
else if (ornament >= 13) subdivisions = 7;                  // Septuplets (7:4)
```

**Micro-Gate Queue System**
- Queue size: 32 entries (line 122 in TuesdayTrackEngine.h)
- Supports 7-tuplets (14 ON/OFF pairs) with headroom
- Each gate stores: `{tick, gate_state, cvTarget}`
- CV applied unconditionally when gates fire (line 966)

**Walker Persistence**
- `_scalewalker_pos` initialized to 0 in header (default member initialization)
- No longer reset in `initAlgorithm()` at loop boundaries
- Only reset via:
  - `reset()` - called on track/algorithm change
  - `reseed()` - manual user action (Shift+F5)

### Testing Verification

**Manual Testing Completed:**
- ✅ Infinite loop mode: Walker advances continuously
- ✅ Finite loop mode (1-16 steps): Walker advances continuously across boundaries
- ✅ Mode switching: Walker position persists
- ✅ Polyrhythm: All 4 beats trigger correctly
- ✅ Micro-gates: Independent pitches per gate
- ✅ Triplets (3), Quintuplets (5), Septuplets (7): All working
- ✅ CV update modes: Free and Gated both working correctly

**Build Status:**
- ✅ Simulator builds successfully
- ✅ All changes compile cleanly (6 pre-existing warnings unrelated to changes)

### Files Modified

| File | Lines Changed | Description |
|------|---------------|-------------|
| `TuesdayTrackEngine.cpp` | ~50 | Subdivision refactor, walker persistence, micro-gate CV fixes |
| `TuesdayTrackEngine.h` | 1 | Queue size increase (16→32) |

### Behavior Changes

**User-Visible:**
1. SCALEWALKER now available as Algorithm 10
2. Finite loop mode no longer resets walker position
3. Polyrhythmic melodies evolve continuously instead of repeating

**Internal:**
1. Subdivision calculation centralized (DRY principle)
2. Micro-gate CV updates unconditional (more predictable)
3. Walker state persistent across parameter changes

---

## Previous Session Progress (Tuesday Refactor)

**Completed:**
*   **Architectural Overhaul of TuesdayTrackEngine:**
    *   Unified Generation Architecture: Replaced split `generateBuffer`/`tick` with `generateStep(tick)` for both Finite and Infinite loops.
    *   Fixed "Static Pattern" bug by correctly caching `loopLength` in `initAlgorithm`.
    *   Implemented "Virtual Loop" logic: Finite loops now reset RNG state at boundaries but execute live, preserving state machine continuity.
*   **Musical Integrity Restoration:**
    *   Restored Scale Pipeline: Algorithms generate abstract Scale Degrees, passed through `scaleToVolts` for proper quantization.
    *   Restored Dynamics: `Power` parameter now interacts with algorithm `Velocity` (Density gating).
    *   RNG Fix: Salted `ExtraRNG` to prevent stream collision when Flow == Ornament.
*   **Algorithm Set Expansion:**
    *   Restored missing algorithms: **Markov (6), ChipArp1 (7), ChipArp2 (8), Wobble (9)**.
    *   Fixed mapping logic that was defaulting new algorithms to Test (0).
    *   Enhanced **Aphex (3)**: Track 2 -> Ghost Notes/Ratchet, Track 3 -> Bass Accent.
    *   Enhanced **Autechre (4)**: Transformation events trigger High Velocity Accents.
    *   Enhanced **StepWave (5)**: Octave jumps are Accents; FX Layer handles "Climbing Trill".
    *   Implemented polyrhythmic time warping for Aphex, Autechre, and StepWave based on Ornament.
*   **UI & Model Updates:**
    *   Added `GateLength` parameter to `TuesdaySequence` model.
    *   Updated `TuesdayEditPage` (Quick Edit) to display **GATE** (Gate Length) instead of **GOFS** (Gate Offset).
    *   Updated `TuesdaySequenceListModel` (Edit List) to remove "Gate Offset" (GOFS) from the display, making "Gate Length" the visible control.
    *   Set default Glide, Trill, and GateLength/Offset to 50% in `TuesdaySequence::clear()`.
