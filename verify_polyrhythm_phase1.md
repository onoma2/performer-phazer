# Phase 1 Polyrhythm Enhancement Verification

## Summary
Added polyrhythm/micro-sequencing support to 4 algorithms:
- **CHIPARP1**: Chord stab ratcheting on beat starts
- **CHIPARP2**: Arpeggio ratcheting on accent hits
- **TRITRANCE**: Triplet/tuplet swing with grace notes on phase 2
- **MARKOV**: Occasional flams (~8% probability)

## Backward Compatibility ✅

### Ornament Parameter Mapping (Global)
```cpp
ornament 1-4  → subdivisions = 4  (CLASSIC BEHAVIOR - no polyrhythm)
ornament 5-8  → subdivisions = 3  (Triplets)
ornament 9-12 → subdivisions = 5  (Quintuplets)
ornament 13+  → subdivisions = 7  (Septuplets)
```

**All enhancements activate ONLY when `ctx.subdivisions != 4`**

This means:
- **Existing patches with ornament 1-4 are unchanged**
- New features only activate when user sets ornament >= 5
- Zero breaking changes to current behavior

---

## Algorithm-Specific Enhancements

### 1. CHIPARP1 (Chiptune Arpeggiator 1)

**When**: Beat start (chordpos == 0) + ornament >= 5

**Behavior**:
- **Mode**: Ratchet (isSpatial = false) - rapid fire trill
- **Pattern**: Chord intervals [0, 2, 4, 6] = [root, 3rd, 5th, 7th]
- **Flow control**: `beatSpread = 20 + (flow * 5)` (timing window)

**Musical effect**: Classic chiptune "chord stab rolls"

**Location**: `TuesdayTrackEngine.cpp:691-703`

---

### 2. CHIPARP2 (Chiptune Arpeggiator 2)

**When**: Accent hit (idx == chordLen) + ornament >= 5

**Behavior**:
- **Mode**: Ratchet (isSpatial = false)
- **Pattern**: Ascending arpeggio scaled by `chordScaler`
- **Flow control**: `beatSpread = 20 + (flow * 5)`

**Musical effect**: Fast arpeggio runs on accent moments

**Location**: `TuesdayTrackEngine.cpp:759-770`

---

### 3. TRITRANCE (Acid Trance)

**When**: Phase 2 (accent phase) + ornament >= 5

**Behavior**:
- **Mode**: Spatial (isSpatial = true) - spread across time
- **Pattern**: Grace notes leading to main accent
  - Triplet: [grace-1, grace-2, MAIN]
  - Higher tuplets: alternating grace pattern
- **Flow control**: `beatSpread = 20 + (flow * 5)`

**Musical effect**: Swung triplet acid with grace note anticipation

**Location**: `TuesdayTrackEngine.cpp:480-496`

---

### 4. MARKOV (Markov Chain)

**When**: Random (~8% probability) + ornament >= 5

**Behavior**:
- **Mode**: Ratchet (isSpatial = false) - tight flam
- **Pattern**: [note, note] (same note twice)
- **Timing**: Very tight (beatSpread = 5)
- **Gates**: Shorter (gateRatio = 50)

**Musical effect**: Occasional note "flams" for humanization/texture

**Location**: `TuesdayTrackEngine.cpp:540-548`

---

## Testing Results

### Build Status
- ✅ Simulator (debug): **SUCCESS**
- ✅ All Tuesday tests: **PASSED**
- ✅ TestTuesdayAlgorithmBuffers: **12/12 PASSED**
- ✅ TestTuesdayPolyrhythm: **5/5 PASSED**

### Compatibility Verification
- ✅ Zero behavior change when ornament 1-4
- ✅ RNG determinism preserved
- ✅ No new compilation warnings
- ✅ Existing patches unaffected

---

## Usage Guide

### To Keep Classic Behavior:
**Set ornament to 1-4** (subdivisions = 4)

### To Enable New Features:

#### Triplet Feel (ornament 5-8):
- CHIPARP1: Triplet chord stabs every beat
- CHIPARP2: Triplet arpeggio rolls on accents
- TRITRANCE: Swung triplet acid with grace notes
- MARKOV: Occasional note flams

#### Quintuplet (ornament 9-12):
- All algorithms use 5-note tuplet subdivisions

#### Septuplet (ornament 13+):
- All algorithms use 7-note tuplet subdivisions

### Flow Parameter:
Controls timing window (20-100%)
- **Low flow (0-8)**: Tight timing (20-60%)
- **High flow (9-16)**: Loose spread (65-100%)

---

## Musical Genres Enhanced

| Algorithm | Genre | Enhancement | Ornament Setting |
|-----------|-------|-------------|------------------|
| CHIPARP1 | Chiptune, Game Music | Chord stab rolls | 5-8 (triplets) |
| CHIPARP2 | Chiptune, Game Music | Arpeggio runs | 5-8 (triplets) |
| TRITRANCE | Acid, Trance | Swung triplet acid | 5-8 (triplets) |
| MARKOV | Ambient, Experimental | Flam texture | 5+ (any tuplet) |

---

## Code Changes

**Files Modified:**
- `src/apps/sequencer/engine/TuesdayTrackEngine.cpp`

**Lines Changed:**
- CHIPARP1: Added 14 lines (691-703)
- CHIPARP2: Added 13 lines (759-770)
- TRITRANCE: Added 18 lines (480-496)
- MARKOV: Added 10 lines (540-548)

**Total**: ~55 lines of enhancement code

**Pattern**: All follow same structure as AUTECHRE/STEPWAVE/SCALEWALKER

---

## Next Steps (Future Phases)

**Phase 2** (Not implemented):
- STOMPER: Drum fill polyrhythms
- WOBBLE: Stutter bass ratcheting

**Phase 3** (Skipped):
- APHEX: Too complex, conflicts with pattern system

---

## Conclusion

✅ **Phase 1 complete**
✅ **Zero breaking changes**
✅ **4 algorithms enhanced**
✅ **All tests passing**
✅ **Backward compatible**

Ready for hardware testing! 🎹
