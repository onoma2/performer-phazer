# trillCount System - Clean Architecture Implementation

## 🎯 Design Philosophy

**Goal:** Maximum rhythmic complexity with clean separation of concerns

**Hierarchy:**
```
MEASURE (bar)
    ↓ subdivided by clock divider
BEAT (quarter note, etc.)
    ↓ subdivided by divisor → steps
STEP (the trigger point where algorithm generates a note)
    ↓ subdivided by trillCount → gates per step
GATE (individual trigger with pitch)
```

---

## 🎛️ Parameter Roles

### **Flow & Ornament**
- **NOT universal controls**
- Algorithm-specific parameters
- Seed `_rng` (Flow) and `_extraRng` (Ornament)
- Each algorithm interprets differently

### **stepTrill (UI Parameter)**
- **Universal control** (0-100%)
- Probability control (like Glide)
- 0 = never fire trills
- 50 = fire 50% of trill opportunities
- 100 = always fire when algorithm enables it

### **trillCount (Algorithm Output)**
- **Set by algorithms** (1-4 gates)
- Defines micro-gate subdivision pattern
- Works with noteOffsets[] for pitch control

---

## 🔧 Implementation Pattern

### In TuesdayTickResult Struct:
```cpp
uint8_t trillCount = 1;  // 1-4 gates (algorithm's desired subdivision)
int8_t noteOffsets[8];   // Pitch pattern for each gate
```

### In Algorithm (e.g., CHIPARP1):
```cpp
// Algorithm decides WHEN and WHAT
if (chordpos == 0) {  // On beat start
    result.trillCount = 3;  // Request 3-gate chord stab
    result.noteOffsets[0] = 0;  // root
    result.noteOffsets[1] = 2;  // third
    result.noteOffsets[2] = 4;  // fifth
}
// Algorithm does NOT check stepTrill - just requests the pattern
```

### In tick() Layer:
```cpp
// UI parameter controls probability (follows Glide pattern)
if (result.trillCount > 1 && sequence.stepTrill() > 0) {
    int chance = sequence.stepTrill();  // 0-100
    if (_uiRng.nextRange(100) < chance) {
        result.polyCount = result.trillCount;  // Fire the trill
        result.isSpatial = false;  // Rapid fire mode
        // noteOffsets[] already set by algorithm
    }
    // else: single gate (trill not fired)
}
```

---

## ✅ Enhanced Algorithms

### 1. CHIPARP1
**When:** Beat starts (`chordpos == 0`)
**Pattern:** Triplet chord stabs
```cpp
trillCount = 3
noteOffsets = [0, 2, 4]  // root, third, fifth
```

### 2. CHIPARP2
**When:** Accent hits (`idx == chordLen`)
**Pattern:** 4-note arpeggio runs
```cpp
trillCount = 4
noteOffsets = [0, scaler, scaler*2, scaler*3]  // ascending arp
```

### 3. TRITRANCE
**When:** Phase 2 (accent phase)
**Pattern:** Grace notes → main note
```cpp
trillCount = 3
noteOffsets = [-2, -1, 0]  // grace, grace, MAIN
```

### 4. MARKOV
**When:** Random (~8% probability)
**Pattern:** Double-hit flam
```cpp
trillCount = 2
noteOffsets = [0, 0]  // same note twice (flam)
```

---

## 🎵 Musical Results

### Example Setup:

**Track 1 (Note Track):**
- 4-on-the-floor kick
- Standard timing

**Track 2 (Tuesday - Polyrhythmic Base):**
- Algorithm: STEPWAVE or SCALEWALKER
- Ornament: 9-12 → 5 steps per beat (quintuplet feel)
- stepTrill: 0 (single gates, clean polyrhythm)

**Track 3 (Tuesday - Trill Layer):**
- Algorithm: CHIPARP1
- Ornament: 1-4 (normal 4 steps per beat)
- stepTrill: 75 → fires trills 75% of the time
- Result: Beat starts get triplet chord stabs

**Combined:** 5-over-4 polyrhythm + chiptune chord stabs = complex groove

---

## 🔒 Backward Compatibility

### With stepTrill = 0:
- **Zero change** to existing behavior
- Algorithms request trills but they never fire
- All patches work exactly as before

### With stepTrill > 0:
- Algorithms can fire trills probabilistically
- User controls density via stepTrill parameter
- Each algorithm defines its own trill patterns

---

## 🎯 Separation of Concerns

| Layer | Responsibility |
|-------|---------------|
| **Algorithm** | WHEN (beat start, accent, phase, probability)<br>WHAT (note pattern, trillCount) |
| **UI Parameter** | HOW OFTEN (probability 0-100%) |
| **tick() Layer** | Execution (probability check, polyCount activation) |

**No mixing:** Algorithms don't check UI params, tick() doesn't decide patterns

---

## 📊 Code Changes

**Files Modified:**
1. `TuesdayTrackEngine.h` - Added `trillCount` field to TuesdayTickResult
2. `TuesdayTrackEngine.cpp`:
   - CHIPARP1: Set trillCount on beat starts
   - CHIPARP2: Set trillCount on accents
   - TRITRANCE: Set trillCount on phase 2
   - MARKOV: Set trillCount randomly (~8%)
   - tick(): Added stepTrill probability logic (lines 1277-1290)

**Total Changes:** ~50 lines

---

## 🎹 Usage Guide

### To Keep Classic Behavior:
**Set stepTrill = 0** (no trills ever fire)

### To Enable Trills:

#### Light Trills (stepTrill 25):
- Algorithms request trills
- Only 25% fire
- Sparse texture

#### Medium Trills (stepTrill 50):
- 50% of trill opportunities fire
- Balanced complexity

#### Full Trills (stepTrill 75-100):
- Most/all trill opportunities fire
- Maximum rhythmic density

### Per-Algorithm Behavior:

| Algorithm | stepTrill 0 | stepTrill 100 |
|-----------|-------------|---------------|
| CHIPARP1 | Normal arp | Triplet chord stabs every beat |
| CHIPARP2 | Normal arp | 4-note runs on every accent |
| TRITRANCE | 3-phase acid | Grace notes on every phase 2 |
| MARKOV | Clean melody | Occasional flams (8% × 100% = 8%) |

---

## ✅ Advantages Over Previous Approach

### ❌ Old Approach (Conditional polyCount):
- Algorithms set polyCount directly
- Mixed algorithm logic with UI parameter checks
- ornament parameter misused as universal control
- "Normal" vs "special" steps
- Confusion about parameter roles

### ✅ New Approach (trillCount pattern):
- Clean separation: algorithm decides WHAT, UI decides WHEN
- Follows proven Glide pattern
- Flow/Ornament remain algorithm-specific
- No "normal" vs "special" - just trill opportunities
- Scalable to all algorithms

---

## 🚀 Future Extensions

### Easy to Add:
Any algorithm can request trills by setting:
```cpp
result.trillCount = 2-4;
result.noteOffsets[...] = pattern;
```

### Examples:
- WOBBLE: Stutter bass (trillCount = 2-4, same note)
- STOMPER: Drum fills (trillCount = 4, velocity pattern)
- STEPWAVE: Chromatic bursts (trillCount = 3, chromatic run)

All controlled by **same stepTrill parameter** - unified UX!

---

## 📝 Summary

✅ **Clean architecture** - proper separation of concerns
✅ **Glide pattern** - proven UI/algorithm interaction model
✅ **Backward compatible** - stepTrill=0 preserves existing behavior
✅ **Scalable** - any algorithm can add trills
✅ **Flexible** - 1-4 gates, any pitch pattern
✅ **Unified UX** - one parameter controls all trill features

**Ready for hardware testing!** 🎛️
