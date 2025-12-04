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

---

## Reference: Polyrhythm vs Trill Mode

### Technical Differences

#### POLYRHYTHM Mode (`isSpatial = true`)

**Timing:**
```cpp
uint32_t windowTicks = 4 * divisor;  // Spread across 1 BEAT (4 × 1/16 = 192 ticks)
uint32_t spacing = windowTicks / tupleN;
```

**Pitch Quantization:**
```cpp
// Scale-degree based (quantized to scale)
int noteWithOffset = result.note + result.noteOffsets[i];
volts = scaleToVolts(noteWithOffset, result.octave);
```

**Example (Quintuplet, divisor=48, 1/16 notes):**
- Window: 192 ticks (1 beat)
- Spacing: 192 / 5 = 38.4 ticks between gates
- Gates at: tick 0, 38, 76, 115, 153
- **5 gates spread evenly across the entire beat**

#### TRILL Mode (`isSpatial = false`)

**Timing:**
```cpp
uint32_t windowTicks = divisor;  // Compressed within 1 STEP (48 ticks)
uint32_t spacing = windowTicks / tupleN;
```

**Pitch Quantization:**
```cpp
// CHROMATIC (bypasses scale quantization)
float baseVolts = scaleToVolts(result.note, result.octave);
volts = baseVolts + (result.noteOffsets[i] / 12.f);  // Direct voltage offset
```

**Example (Quintuplet, divisor=48):**
- Window: 48 ticks (1 step = 1/16 note)
- Spacing: 48 / 5 = 9.6 ticks between gates
- Gates at: tick 0, 9, 19, 29, 38
- **5 gates fired rapidly within the first 1/16 note**

### Musical Consequences

#### POLYRHYTHM Mode (Spatial)

**Use Cases:**
- **African/Latin rhythms** - Quintuplets over 4/4 create "5 against 4" polyrhythms
- **Progressive rock/math rock** - Complex subdivision grooves
- **Melodic sequences** - Each gate plays a different scale degree
- **Generative music** - Creates evolving melodic patterns

**Sound Character:**
- ✅ **Rhythmic complexity** - Creates metric tension (5-feel over 4/4 grid)
- ✅ **Melodic content** - Each gate is a distinct melodic note
- ✅ **Scale-aware** - Respects current scale (e.g., minor pentatonic)
- ✅ **Groove-oriented** - Gates lock to subdivisions of the beat
- ✅ **Danceable** - Still feels grounded in the underlying meter

**Examples:**
- **Triplets**: Ta-ta-ta (swing feel, jazz)
- **Quintuplets**: 1-2-3-4-5 (odd subdivision, prog/IDM)
- **Septuplets**: 1-2-3-4-5-6-7 (very complex, experimental)

**SCALEWALKER with quintuplet:**
```
Beat 1: C-D-E-F-G (5 notes spread across 4/4 time)
        |....|....|....|....| (rhythmic positions)
```

#### TRILL Mode (Temporal)

**Use Cases:**
- **Baroque trills/ornaments** - Fast chromatic decorations
- **Drum rolls** - Rapid repeated hits
- **Glitch/IDM effects** - Stutter/ratchet effects
- **Chromatic runs** - Fast scalar passages

**Sound Character:**
- ✅ **Speed/urgency** - Extremely fast, compressed in time
- ✅ **Chromatic** - Semitone steps, not constrained by scale
- ✅ **Ornamental** - Decorates the main note
- ✅ **Percussive impact** - All gates fire within one step duration
- ✅ **Dramatic** - Creates tension/release through rapid movement

**Examples:**
- **Triplet trill**: tr-tr-tr (machine gun effect)
- **Quintuplet trill**: trrrr (drum roll)
- **Septuplet trill**: trrrrrr (snare drum flutter)

**STEPWAVE with quintuplet trill:**
```
Step 1: C-C#-D-D#-E (5 chromatic notes RAPIDLY)
        |....| (all within first 1/16 note)
Step 2: [next note]
        |....|
```

### Side-by-Side Comparison

**Quintuplet Example (ornament=9):**

| Aspect | POLYRHYTHM (Spatial) | TRILL (Temporal) |
|--------|---------------------|------------------|
| **Window** | 192 ticks (1 beat) | 48 ticks (1 step) |
| **Spacing** | 38 ticks apart | 9.6 ticks apart |
| **Speed** | Moderate (spread out) | **4× faster** (compressed) |
| **Pitch System** | Scale degrees (C→D→E→F→G) | Semitones (C→C#→D→D#→E) |
| **Rhythm Feel** | "5 against 4" polyrhythm | Rapid ornament/stutter |
| **Musical Function** | Melodic sequence | Decorative flourish |
| **Perception** | 5 distinct rhythmic events | One "rolled" event |

### Algorithm Usage

| Algorithm | Mode | Use Case |
|-----------|------|----------|
| **SCALEWALKER** | Polyrhythm | Melodic walking patterns with metric complexity |
| **APHEX** | Polyrhythm | Polyrhythmic melodic sequences |
| **AUTECHRE** | Polyrhythm | Transforming melodic polyrhythms |
| **STEPWAVE** | **Trill** | Chromatic climbing trills (ornamental) |

### Practical Example

**Setup: C Major Scale, Flow=9 (up), Ornament=9 (quintuplet)**

**SCALEWALKER (Polyrhythm):**
```
Beat pattern: C...D....E....F.....G
              |----|----|----|----| (1 beat = 4 sixteenths)
Result: Laid-back, groovy, "5 against 4" feel
Use for: Rhythmic bass lines, melodic sequences
```

**STEPWAVE (Trill):**
```
Step pattern: CDbDEbE rest rest rest
              |----| (compressed into first 1/16)
Result: Fast chromatic flutter, then silence
Use for: Snare rolls, glitch effects, ornaments
```

### When to Use Each Mode

**Use POLYRHYTHM when:**
- Building grooves with metric complexity
- Creating evolving melodic patterns
- Adding rhythmic interest while staying "in time"
- Developing harmonic/melodic content

**Use TRILL when:**
- Adding ornamental decoration to main notes
- Creating dramatic flourishes/accents
- Producing glitch/stutter effects
- Generating rapid scalar passages
- Emulating drum rolls

**Key Difference:** Polyrhythm is about **RHYTHM + MELODY**, while Trill is about **SPEED + DECORATION**.
