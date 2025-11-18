# WORKING-TDD-HARMONY-PLAN.md - Phased TDD Implementation Plan

**Date**: 2025-11-18
**Status**: ✅ READY FOR IMPLEMENTATION
**Approach**: Option 4 - Phased Hybrid (from HARMONIAG-FEASIBILITY.md)
**Total Estimated Time**: 7-11 weeks (to Phase 3 production-ready)

---

## Executive Summary

This plan implements Harmonàig-style harmonic sequencing through **incremental phases**, each deliverable and testable independently. Following strict TDD methodology with RED-GREEN-REFACTOR cycles.

**Key Principles**:
- ✅ Test-first development for ALL features
- ✅ Each phase delivers working functionality
- ✅ Gate at phase completion before proceeding
- ✅ Can stop at any phase with usable feature
- ✅ Addresses all critical architectural constraints

**Critical Fixes Applied**:
1. ✅ Scope aligned with MVP-first approach
2. ✅ Track position constraints (T1/T5 only) architected
3. ✅ Scale override per HarmonyRoot added
4. ✅ Master/follower tick ordering specified
5. ✅ Parametrized testing for combination explosion
6. ✅ Voicing/inversion as dual (global + per-step) parameters
7. ✅ Accumulator integration with all trigger modes

---

## Phase Overview

| Phase | Scope | Duration | Deliverable | Risk |
|-------|-------|----------|-------------|------|
| **1 MVP** | Ionian, diatonic, root pos, close voice | 2-3 weeks | Basic harmony working | 🟢 LOW |
| **2 Modes** | +6 Ionian modes, +1st inversion, transpose | 2-3 weeks | All major/minor modes | 🟢 LOW |
| **3 Polish** | +Drop2 voicing, slew, per-step params | 1-2 weeks | Production-ready lite | 🟡 MEDIUM |
| **4 Advanced** | +4 qualities, CV control, perf mode | 2-3 weeks | Full-featured | 🟡 MEDIUM |
| **5+ Future** | Harmonic Minor, all inversions/voicings | Optional | Complete Harmonàig | 🔴 HIGH |

**Gate at Phase 1**: Review effort vs estimate, decide to proceed or stop.

---

## Architecture Constraints (CRITICAL)

### 1. Track Position Restrictions

**HARD CONSTRAINT**: Only Tracks 1 and 5 can be HarmonyRoot (Master).

**Rationale** (from HARMONY-IDEATION.md):
- Track 1 → Followers: Tracks 2, 3, 4 (Harm3, Harm5, Harm7)
- Track 5 → Followers: Tracks 6, 7, 8 (Harm3, Harm5, Harm7)

**Implementation**:
```cpp
// HarmonySequence must know its track index
class HarmonySequence : public NoteSequence {
public:
    HarmonySequence(uint8_t trackIndex); // NEW: Constructor takes track index

    bool canBeHarmonyRoot() const {
        return (_trackIndex == 0 || _trackIndex == 4); // Tracks 1 or 5
    }

    void setHarmonyRole(HarmonyEngine::HarmonyRole role) {
        if (role == HarmonyEngine::HarmonyRole::Master && !canBeHarmonyRoot()) {
            // Reject or auto-revert to Follower
            _harmonyRole = HarmonyEngine::HarmonyRole::Follower3rd; // Default follower
            return;
        }
        _harmonyRole = role;
    }

private:
    uint8_t _trackIndex; // 0-7 (T1-T8)
};
```

### 2. Scale Override per HarmonyRoot

**CONSTRAINT**: HarmonyRoot tracks use their own scale setting, NOT the project scale.

**Implementation**:
```cpp
class HarmonySequence : public NoteSequence {
public:
    // HarmonyRoot-specific scale (only valid if harmonyRole == Master)
    void setScale(HarmonyEngine::Mode mode) {
        if (_harmonyRole == HarmonyEngine::HarmonyRole::Master) {
            _scale = mode;
        }
    }

    HarmonyEngine::Mode scale() const {
        return _scale; // Used by harmony engine
    }

    bool usesOwnScale() const {
        return (_harmonyRole == HarmonyEngine::HarmonyRole::Master);
    }

private:
    HarmonyEngine::Mode _scale; // Default: Ionian
};
```

### 3. Master/Follower Tick Ordering

**CONSTRAINT**: Master tracks MUST tick before follower tracks in Engine::tick() loop.

**Implementation**:
```cpp
void Engine::tick(uint32_t tick) {
    // CRITICAL: Three-phase ticking to prevent race conditions

    // PHASE 1: Tick all HarmonyRoot (Master) tracks
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isMaster()) {
            _harmonyTrackEngines[i].tick(tick);
            // Master now has _currentChord calculated
        }
    }

    // PHASE 2: Tick all Follower tracks
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isFollower()) {
            _harmonyTrackEngines[i].tick(tick);
            // Follower reads master's _currentChord
        }
    }

    // PHASE 3: Tick all Independent/other tracks
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isIndependent()) {
            _harmonyTrackEngines[i].tick(tick);
        }
    }

    // Then tick other engine types (Curve, MidiCv, etc.)
    // ...
}
```

### 4. Voicing/Inversion Dual Model

**CONSTRAINT**: Voicing and Inversion exist as BOTH global defaults AND per-step overrides.

**Model**:
- Global defaults on HarmonyGlobalPage (HARM)
- Per-step overrides via F3 NOTE button cycling
- Step inherits global if not explicitly set

**Implementation**:
```cpp
class HarmonyEngine {
    // Global defaults
    Voicing _globalVoicing = Voicing::Close;
    uint8_t _globalInversion = 0; // Root position
};

class NoteSequence::Step {
    // Per-step overrides (stored in _data1 bitfields)
    bool _hasLocalVoicing : 1;
    Voicing _voicing : 2; // 0-3 (4 voicings)
    bool _hasLocalInversion : 1;
    uint8_t _inversion : 2; // 0-3 (4 inversions)

    Voicing effectiveVoicing(const HarmonyEngine& engine) const {
        return _hasLocalVoicing ? _voicing : engine.globalVoicing();
    }

    uint8_t effectiveInversion(const HarmonyEngine& engine) const {
        return _hasLocalInversion ? _inversion : engine.globalInversion();
    }
};
```

---

## PHASE 1: MVP (Ionian-Only Harmony) - 2-3 Weeks

**Goal**: Prove core concept with minimal scope - working harmony in C Major scale.

**Deliverable**: Master track sequences notes, followers output harmonized 3rd/5th/7th.

### Scope Limitations (STRICT)

✅ **Included**:
- 1 mode only: **Ionian (C Major)**
- Diatonic chord quality only (automatic, based on scale degree)
- Root position only (no inversions)
- Close voicing only
- Track position restrictions (T1/T5 only can be HarmonyRoot)
- Master/follower architecture
- Basic UI (track role selection)

❌ **Excluded** (deferred to later phases):
- Other modes (Dorian, Phrygian, etc.)
- Manual chord quality selection
- Inversions
- Other voicings (Drop2, etc.)
- Transpose
- Slew
- CV control
- Performance mode
- Per-step voicing/inversion

---

### Phase 1 Task Breakdown

---

#### Task 1.1: HarmonyEngine Foundation (Week 1, Days 1-2)

**RED**: Write failing test for basic HarmonyEngine construction

```cpp
// File: src/tests/unit/sequencer/TestHarmonyEngine.cpp

TEST(HarmonyEngine, DefaultConstruction) {
    HarmonyEngine engine;

    // Default to Ionian mode (C Major)
    ASSERT_EQ(engine.mode(), HarmonyEngine::Mode::Ionian);

    // Default to diatonic quality mode (automatic)
    ASSERT_TRUE(engine.diatonicMode());

    // Default to root position
    ASSERT_EQ(engine.inversion(), 0);

    // Default to close voicing
    ASSERT_EQ(engine.voicing(), HarmonyEngine::Voicing::Close);

    // Default transpose is 0
    ASSERT_EQ(engine.transpose(), 0);
}
```

**GREEN**: Implement HarmonyEngine.h/cpp skeleton

```cpp
// File: src/apps/sequencer/model/HarmonyEngine.h

class HarmonyEngine {
public:
    enum class Mode {
        Ionian,     // C Major - PHASE 1 ONLY
        Dorian,     // Phase 2
        Phrygian,   // Phase 2
        Lydian,     // Phase 2
        Mixolydian, // Phase 2
        Aeolian,    // Phase 2
        Locrian,    // Phase 2
        // Harmonic Minor modes: Phase 5+
        Last
    };

    enum class ChordQuality {
        // Phase 1: Auto-selected via diatonic mode
        Minor7,      // -7
        Dominant7,   // 7
        Major7,      // ∆7
        HalfDim7,    // ø
        // Phase 4: Manual selection
        Last
    };

    enum class Voicing {
        Close,    // Phase 1
        Drop2,    // Phase 3
        Drop3,    // Phase 5+
        Spread,   // Phase 5+
        Last
    };

    enum class HarmonyRole {
        Independent,    // Normal track, no harmony
        Master,         // Provides root for harmony (T1/T5 only)
        Follower3rd,    // Outputs 3rd
        Follower5th,    // Outputs 5th
        Follower7th,    // Outputs 7th
        Last
    };

    HarmonyEngine()
        : _mode(Mode::Ionian)
        , _diatonicMode(true)
        , _inversion(0)
        , _voicing(Voicing::Close)
        , _transpose(0)
    {}

    // Getters
    Mode mode() const { return _mode; }
    bool diatonicMode() const { return _diatonicMode; }
    uint8_t inversion() const { return _inversion; }
    Voicing voicing() const { return _voicing; }
    int8_t transpose() const { return _transpose; }

    // Setters (limited in Phase 1)
    void setMode(Mode mode) { _mode = mode; }
    void setDiatonicMode(bool diatonic) { _diatonicMode = diatonic; }
    void setInversion(uint8_t inv) { _inversion = std::min(inv, uint8_t(3)); }
    void setVoicing(Voicing v) { _voicing = v; }
    void setTranspose(int8_t t) { _transpose = std::clamp(t, int8_t(-24), int8_t(24)); }

private:
    Mode _mode;
    bool _diatonicMode;
    uint8_t _inversion;      // 0-3
    Voicing _voicing;
    int8_t _transpose;       // -24 to +24
};
```

**REFACTOR**: Clean up includes, add documentation.

---

#### Task 1.2: Ionian Scale Lookup Table (Week 1, Days 2-3)

**RED**: Write test for Ionian mode scale intervals

```cpp
TEST(HarmonyEngine, IonianScaleIntervals) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);

    // Ionian intervals: 0-2-4-5-7-9-11 (W-W-H-W-W-W-H)
    const int16_t expectedIntervals[7] = {0, 2, 4, 5, 7, 9, 11};

    for (int degree = 0; degree < 7; degree++) {
        int16_t interval = engine.getScaleInterval(degree);
        ASSERT_EQ(interval, expectedIntervals[degree])
            << "Scale degree " << degree << " failed";
    }
}
```

**GREEN**: Implement scale interval lookup

```cpp
// In HarmonyEngine.h
public:
    int16_t getScaleInterval(uint8_t degree) const {
        if (degree >= 7) degree %= 7; // Wrap to 0-6
        return ScaleIntervals[static_cast<int>(_mode)][degree];
    }

private:
    // Lookup table: [mode][degree] = semitone offset from root
    static const uint8_t ScaleIntervals[7][7];
```

```cpp
// In HarmonyEngine.cpp
const uint8_t HarmonyEngine::ScaleIntervals[7][7] = {
    // Ionian (W-W-H-W-W-W-H)
    {0, 2, 4, 5, 7, 9, 11},

    // Dorian - Phase 2
    {0, 2, 3, 5, 7, 9, 10},

    // Phrygian - Phase 2
    {0, 1, 3, 5, 7, 8, 10},

    // Lydian - Phase 2
    {0, 2, 4, 6, 7, 9, 11},

    // Mixolydian - Phase 2
    {0, 2, 4, 5, 7, 9, 10},

    // Aeolian - Phase 2
    {0, 2, 3, 5, 7, 8, 10},

    // Locrian - Phase 2
    {0, 1, 3, 5, 6, 8, 10}
};
```

**REFACTOR**: Add comments documenting scale formulas.

---

#### Task 1.3: Diatonic Chord Quality Lookup (Week 1, Day 3)

**RED**: Test diatonic chord quality for all Ionian degrees

```cpp
TEST(HarmonyEngine, IonianDiatonicChordQualities) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);
    engine.setDiatonicMode(true);

    // Ionian diatonic 7th chords: I∆7, ii-7, iii-7, IV∆7, V7, vi-7, viiø
    ASSERT_EQ(engine.getDiatonicQuality(0), HarmonyEngine::ChordQuality::Major7);   // I
    ASSERT_EQ(engine.getDiatonicQuality(1), HarmonyEngine::ChordQuality::Minor7);   // ii
    ASSERT_EQ(engine.getDiatonicQuality(2), HarmonyEngine::ChordQuality::Minor7);   // iii
    ASSERT_EQ(engine.getDiatonicQuality(3), HarmonyEngine::ChordQuality::Major7);   // IV
    ASSERT_EQ(engine.getDiatonicQuality(4), HarmonyEngine::ChordQuality::Dominant7); // V
    ASSERT_EQ(engine.getDiatonicQuality(5), HarmonyEngine::ChordQuality::Minor7);   // vi
    ASSERT_EQ(engine.getDiatonicQuality(6), HarmonyEngine::ChordQuality::HalfDim7); // vii
}
```

**GREEN**: Implement diatonic quality lookup table

```cpp
// In HarmonyEngine.h
public:
    ChordQuality getDiatonicQuality(uint8_t scaleDegree) const {
        if (scaleDegree >= 7) scaleDegree %= 7;
        return DiatonicChords[static_cast<int>(_mode)][scaleDegree];
    }

private:
    static const ChordQuality DiatonicChords[7][7];
```

```cpp
// In HarmonyEngine.cpp
const HarmonyEngine::ChordQuality HarmonyEngine::DiatonicChords[7][7] = {
    // Ionian: I∆7, ii-7, iii-7, IV∆7, V7, vi-7, viiø
    {
        ChordQuality::Major7,     // I
        ChordQuality::Minor7,     // ii
        ChordQuality::Minor7,     // iii
        ChordQuality::Major7,     // IV
        ChordQuality::Dominant7,  // V
        ChordQuality::Minor7,     // vi
        ChordQuality::HalfDim7    // vii
    },

    // Dorian - Phase 2
    {ChordQuality::Minor7, ChordQuality::Minor7, ChordQuality::Major7,
     ChordQuality::Dominant7, ChordQuality::Minor7, ChordQuality::HalfDim7,
     ChordQuality::Major7},

    // Phrygian - Phase 2
    {ChordQuality::Minor7, ChordQuality::Major7, ChordQuality::Dominant7,
     ChordQuality::Minor7, ChordQuality::HalfDim7, ChordQuality::Major7,
     ChordQuality::Minor7},

    // Lydian - Phase 2
    {ChordQuality::Major7, ChordQuality::Dominant7, ChordQuality::Minor7,
     ChordQuality::HalfDim7, ChordQuality::Major7, ChordQuality::Minor7,
     ChordQuality::Minor7},

    // Mixolydian - Phase 2
    {ChordQuality::Dominant7, ChordQuality::Minor7, ChordQuality::HalfDim7,
     ChordQuality::Major7, ChordQuality::Minor7, ChordQuality::Minor7,
     ChordQuality::Major7},

    // Aeolian - Phase 2
    {ChordQuality::Minor7, ChordQuality::HalfDim7, ChordQuality::Major7,
     ChordQuality::Minor7, ChordQuality::Minor7, ChordQuality::Major7,
     ChordQuality::Dominant7},

    // Locrian - Phase 2
    {ChordQuality::HalfDim7, ChordQuality::Major7, ChordQuality::Minor7,
     ChordQuality::Minor7, ChordQuality::Major7, ChordQuality::Dominant7,
     ChordQuality::Minor7}
};
```

**REFACTOR**: Add chord symbol comments (I∆7, ii-7, etc.).

---

#### Task 1.4: Chord Interval Lookup (Week 1, Day 4)

**RED**: Test chord intervals for all 4 chord qualities used in Phase 1

```cpp
TEST(HarmonyEngine, ChordIntervals) {
    HarmonyEngine engine;

    // Major7: R-3-5-7 (0-4-7-11)
    auto maj7 = engine.getChordIntervals(HarmonyEngine::ChordQuality::Major7);
    ASSERT_EQ(maj7[0], 0);  // Root
    ASSERT_EQ(maj7[1], 4);  // Major 3rd
    ASSERT_EQ(maj7[2], 7);  // Perfect 5th
    ASSERT_EQ(maj7[3], 11); // Major 7th

    // Minor7: R-♭3-5-♭7 (0-3-7-10)
    auto min7 = engine.getChordIntervals(HarmonyEngine::ChordQuality::Minor7);
    ASSERT_EQ(min7[0], 0);
    ASSERT_EQ(min7[1], 3);  // Minor 3rd
    ASSERT_EQ(min7[2], 7);
    ASSERT_EQ(min7[3], 10); // Minor 7th

    // Dominant7: R-3-5-♭7 (0-4-7-10)
    auto dom7 = engine.getChordIntervals(HarmonyEngine::ChordQuality::Dominant7);
    ASSERT_EQ(dom7[0], 0);
    ASSERT_EQ(dom7[1], 4);
    ASSERT_EQ(dom7[2], 7);
    ASSERT_EQ(dom7[3], 10);

    // HalfDim7: R-♭3-♭5-♭7 (0-3-6-10)
    auto halfDim = engine.getChordIntervals(HarmonyEngine::ChordQuality::HalfDim7);
    ASSERT_EQ(halfDim[0], 0);
    ASSERT_EQ(halfDim[1], 3);
    ASSERT_EQ(halfDim[2], 6);  // Diminished 5th
    ASSERT_EQ(halfDim[3], 10);
}
```

**GREEN**: Implement chord interval table

```cpp
// In HarmonyEngine.h
public:
    struct ChordIntervals {
        uint8_t intervals[4]; // R, 3rd, 5th, 7th
        uint8_t& operator[](int i) { return intervals[i]; }
        const uint8_t& operator[](int i) const { return intervals[i]; }
    };

    ChordIntervals getChordIntervals(ChordQuality quality) const {
        return ChordIntervalTable[static_cast<int>(quality)];
    }

private:
    static const ChordIntervals ChordIntervalTable[4]; // Phase 1: 4 qualities
```

```cpp
// In HarmonyEngine.cpp
const HarmonyEngine::ChordIntervals HarmonyEngine::ChordIntervalTable[4] = {
    {{0, 3, 7, 10}},  // Minor7
    {{0, 4, 7, 10}},  // Dominant7
    {{0, 4, 7, 11}},  // Major7
    {{0, 3, 6, 10}}   // HalfDim7
};
```

**REFACTOR**: Order enum to match table index for clarity.

---

#### Task 1.5: Core Harmonization Method (Week 1, Day 5)

**RED**: Test basic chord generation from root note

```cpp
TEST(HarmonyEngine, BasicHarmonization) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);
    engine.setDiatonicMode(true);

    // C Major 7 (I in C Ionian): C(60) - E(64) - G(67) - B(71)
    auto cChord = engine.harmonize(60, 0); // Root note C, scale degree 0 (I)
    ASSERT_EQ(cChord.root, 60);
    ASSERT_EQ(cChord.third, 64);
    ASSERT_EQ(cChord.fifth, 67);
    ASSERT_EQ(cChord.seventh, 71);

    // D minor 7 (ii in C Ionian): D(62) - F(65) - A(69) - C(72)
    auto dChord = engine.harmonize(62, 1); // Root note D, scale degree 1 (ii)
    ASSERT_EQ(dChord.root, 62);
    ASSERT_EQ(dChord.third, 65);
    ASSERT_EQ(dChord.fifth, 69);
    ASSERT_EQ(dChord.seventh, 72);

    // G Dominant 7 (V in C Ionian): G(67) - B(71) - D(74) - F(77)
    auto gChord = engine.harmonize(67, 4); // Root note G, scale degree 4 (V)
    ASSERT_EQ(gChord.root, 67);
    ASSERT_EQ(gChord.third, 71);
    ASSERT_EQ(gChord.fifth, 74);
    ASSERT_EQ(gChord.seventh, 77);
}
```

**GREEN**: Implement harmonize() method

```cpp
// In HarmonyEngine.h
public:
    struct ChordNotes {
        int16_t root;
        int16_t third;
        int16_t fifth;
        int16_t seventh;
    };

    ChordNotes harmonize(int16_t rootNote, uint8_t scaleDegree) const;

private:
    int16_t applyInterval(int16_t baseNote, uint8_t interval) const {
        int16_t result = baseNote + interval;
        // Clamp to MIDI range 0-127
        return std::clamp(result, int16_t(0), int16_t(127));
    }
```

```cpp
// In HarmonyEngine.cpp
HarmonyEngine::ChordNotes HarmonyEngine::harmonize(int16_t rootNote, uint8_t scaleDegree) const {
    ChordNotes chord;

    // Get chord quality for this scale degree (diatonic mode in Phase 1)
    ChordQuality quality = getDiatonicQuality(scaleDegree);

    // Get chord intervals for this quality
    ChordIntervals intervals = getChordIntervals(quality);

    // Apply intervals to root note
    chord.root = applyInterval(rootNote, intervals[0]);   // Always 0
    chord.third = applyInterval(rootNote, intervals[1]);
    chord.fifth = applyInterval(rootNote, intervals[2]);
    chord.seventh = applyInterval(rootNote, intervals[3]);

    // Phase 1: No inversion, no voicing, no transpose
    // Phase 2: Add inversion
    // Phase 3: Add voicing

    return chord;
}
```

**REFACTOR**: Extract interval application logic.

---

#### Task 1.6: Track Position Constraint Architecture (Week 2, Day 1)

**RED**: Test track position restrictions

```cpp
// File: src/tests/unit/sequencer/TestHarmonySequence.cpp

TEST(HarmonySequence, TrackPositionRestrictions) {
    // Track 1 (index 0): CAN be HarmonyRoot ✅
    HarmonySequence track1(0);
    ASSERT_TRUE(track1.canBeHarmonyRoot());
    track1.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    ASSERT_EQ(track1.harmonyRole(), HarmonyEngine::HarmonyRole::Master);

    // Track 2 (index 1): CANNOT be HarmonyRoot ❌
    HarmonySequence track2(1);
    ASSERT_FALSE(track2.canBeHarmonyRoot());
    track2.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    ASSERT_NE(track2.harmonyRole(), HarmonyEngine::HarmonyRole::Master);
    ASSERT_EQ(track2.harmonyRole(), HarmonyEngine::HarmonyRole::Follower3rd); // Auto-reverted

    // Track 3 (index 2): CANNOT be HarmonyRoot ❌
    HarmonySequence track3(2);
    ASSERT_FALSE(track3.canBeHarmonyRoot());

    // Track 4 (index 3): CANNOT be HarmonyRoot ❌
    HarmonySequence track4(3);
    ASSERT_FALSE(track4.canBeHarmonyRoot());

    // Track 5 (index 4): CAN be HarmonyRoot ✅
    HarmonySequence track5(4);
    ASSERT_TRUE(track5.canBeHarmonyRoot());
    track5.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    ASSERT_EQ(track5.harmonyRole(), HarmonyEngine::HarmonyRole::Master);

    // Track 6 (index 5): CANNOT be HarmonyRoot ❌
    HarmonySequence track6(5);
    ASSERT_FALSE(track6.canBeHarmonyRoot());

    // Track 7 (index 6): CANNOT be HarmonyRoot ❌
    HarmonySequence track7(6);
    ASSERT_FALSE(track7.canBeHarmonyRoot());

    // Track 8 (index 7): CANNOT be HarmonyRoot ❌
    HarmonySequence track8(7);
    ASSERT_FALSE(track8.canBeHarmonyRoot());
}
```

**GREEN**: Implement HarmonySequence with track constraints

```cpp
// File: src/apps/sequencer/model/HarmonySequence.h

class HarmonySequence : public NoteSequence {
public:
    HarmonySequence(uint8_t trackIndex)
        : NoteSequence()
        , _trackIndex(trackIndex)
        , _harmonyRole(HarmonyEngine::HarmonyRole::Independent)
        , _masterTrackIndex(-1)
        , _scale(HarmonyEngine::Mode::Ionian)
    {}

    // Track position check
    bool canBeHarmonyRoot() const {
        return (_trackIndex == 0 || _trackIndex == 4); // Tracks 1 or 5
    }

    // Harmony role with constraint enforcement
    void setHarmonyRole(HarmonyEngine::HarmonyRole role) {
        if (role == HarmonyEngine::HarmonyRole::Master && !canBeHarmonyRoot()) {
            // Reject Master role for non-T1/T5 tracks
            // Auto-assign sensible follower role based on position
            if (_trackIndex == 1) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower3rd;
            } else if (_trackIndex == 2) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower5th;
            } else if (_trackIndex == 3) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower7th;
            } else if (_trackIndex == 5) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower3rd;
            } else if (_trackIndex == 6) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower5th;
            } else if (_trackIndex == 7) {
                _harmonyRole = HarmonyEngine::HarmonyRole::Follower7th;
            } else {
                _harmonyRole = HarmonyEngine::HarmonyRole::Independent;
            }
            return;
        }
        _harmonyRole = role;
    }

    HarmonyEngine::HarmonyRole harmonyRole() const {
        return _harmonyRole;
    }

    // Master track reference (for followers)
    void setMasterTrackIndex(int8_t masterIndex) {
        _masterTrackIndex = masterIndex;
    }

    int8_t masterTrackIndex() const {
        return _masterTrackIndex;
    }

    // Scale override (only for HarmonyRoot tracks)
    void setScale(HarmonyEngine::Mode mode) {
        if (_harmonyRole == HarmonyEngine::HarmonyRole::Master) {
            _scale = mode;
        }
    }

    HarmonyEngine::Mode scale() const {
        return _scale;
    }

    bool usesOwnScale() const {
        return (_harmonyRole == HarmonyEngine::HarmonyRole::Master);
    }

    // Serialization
    void write(VersionedSerializedWriter &writer) const override {
        NoteSequence::write(writer);
        writer.write(static_cast<uint8_t>(_harmonyRole));
        writer.write(_masterTrackIndex);
        writer.write(static_cast<uint8_t>(_scale));
    }

    void read(VersionedSerializedReader &reader) override {
        NoteSequence::read(reader);
        uint8_t role;
        reader.read(role);
        _harmonyRole = static_cast<HarmonyEngine::HarmonyRole>(role);
        reader.read(_masterTrackIndex);
        uint8_t mode;
        reader.read(mode);
        _scale = static_cast<HarmonyEngine::Mode>(mode);
    }

private:
    uint8_t _trackIndex; // 0-7 (T1-T8)
    HarmonyEngine::HarmonyRole _harmonyRole;
    int8_t _masterTrackIndex;  // -1 if not follower, 0-7 for T1-T8
    HarmonyEngine::Mode _scale; // Only used if Master role
};
```

**REFACTOR**: Add validation helpers, error logging for constraint violations.

---

#### Task 1.7: Scale Override Testing (Week 2, Day 1)

**RED**: Test scale override per HarmonyRoot

```cpp
TEST(HarmonySequence, HarmonyRootScaleOverride) {
    HarmonySequence track1(0); // Track 1 can be HarmonyRoot
    track1.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);

    // Set track-specific scale (overrides project scale)
    track1.setScale(HarmonyEngine::Mode::Dorian);
    ASSERT_EQ(track1.scale(), HarmonyEngine::Mode::Dorian);
    ASSERT_TRUE(track1.usesOwnScale());

    // Follower tracks don't use own scale
    HarmonySequence track2(1); // Track 2 is follower
    track2.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);
    ASSERT_FALSE(track2.usesOwnScale());

    // Trying to set scale on follower has no effect
    track2.setScale(HarmonyEngine::Mode::Phrygian);
    ASSERT_NE(track2.scale(), HarmonyEngine::Mode::Phrygian); // Unchanged
}

TEST(HarmonySequence, IndependentTrackScaleUsage) {
    HarmonySequence track2(1); // Track 2 in independent mode
    track2.setHarmonyRole(HarmonyEngine::HarmonyRole::Independent);

    // Independent tracks don't use harmony scale
    ASSERT_FALSE(track2.usesOwnScale());
}
```

**GREEN**: Implementation already complete in Task 1.6.

**REFACTOR**: Add documentation explaining scale override semantics.

---

#### Task 1.8: HarmonyTrackEngine Foundation (Week 2, Days 2-3)

**RED**: Test basic HarmonyTrackEngine initialization

```cpp
// File: src/tests/unit/sequencer/TestHarmonyTrackEngine.cpp (DISABLED by default)

TEST(HarmonyTrackEngine, DISABLED_Initialization) {
    // NOTE: This test is DISABLED like TestNoteTrackEngine due to hardware dependencies
    // It serves as documentation and can be enabled for integration testing

    Engine engine; // Mock or real
    Model model;
    Track track;

    HarmonyTrackEngine harmonyEngine(engine, model, track, 0); // trackIndex=0

    ASSERT_FALSE(harmonyEngine.isMaster());
    ASSERT_FALSE(harmonyEngine.isFollower());
    ASSERT_TRUE(harmonyEngine.isIndependent());
}
```

**GREEN**: Implement HarmonyTrackEngine skeleton

```cpp
// File: src/apps/sequencer/engine/HarmonyTrackEngine.h

class HarmonyTrackEngine : public TrackEngine {
public:
    HarmonyTrackEngine(Engine &engine, Model &model, Track &track, uint8_t trackIndex)
        : TrackEngine(engine, model, track)
        , _trackIndex(trackIndex)
        , _sequence(nullptr)
        , _masterEngine(nullptr)
        , _currentRootNote(60) // Middle C
    {}

    // TrackEngine interface
    virtual void reset() override {}
    virtual void restart() override {}
    virtual void update(uint32_t tick) override {}
    virtual void tick(uint32_t tick) override; // Implemented below
    virtual void changePattern() override {}

    // Harmony-specific
    void setHarmonySequence(HarmonySequence *sequence) {
        _sequence = sequence;
    }

    // Master/Follower coordination
    void setMasterEngine(HarmonyTrackEngine *master) {
        _masterEngine = master;
    }

    HarmonyTrackEngine* masterEngine() const {
        return _masterEngine;
    }

    // Role checks
    bool isMaster() const {
        return _sequence && _sequence->harmonyRole() == HarmonyEngine::HarmonyRole::Master;
    }

    bool isFollower() const {
        return _sequence && (
            _sequence->harmonyRole() == HarmonyEngine::HarmonyRole::Follower3rd ||
            _sequence->harmonyRole() == HarmonyEngine::HarmonyRole::Follower5th ||
            _sequence->harmonyRole() == HarmonyEngine::HarmonyRole::Follower7th
        );
    }

    bool isIndependent() const {
        return !_sequence || _sequence->harmonyRole() == HarmonyEngine::HarmonyRole::Independent;
    }

    // Current harmony state
    int16_t currentRootNote() const {
        return _currentRootNote;
    }

    const HarmonyEngine::ChordNotes& currentChord() const {
        return _currentChord;
    }

private:
    uint8_t _trackIndex; // 0-7
    HarmonySequence *_sequence;
    HarmonyTrackEngine *_masterEngine;

    // Current state
    int16_t _currentRootNote;
    HarmonyEngine::ChordNotes _currentChord;
};
```

**REFACTOR**: Add null checks, defensive programming.

---

#### Task 1.9: Master Track Tick Logic (Week 2, Day 3)

**RED**: Test master track chord calculation

```cpp
TEST(HarmonyTrackEngine, DISABLED_MasterChordCalculation) {
    Engine engine;
    Model model;
    Track track;
    HarmonySequence sequence(0); // Track 1
    sequence.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    sequence.setScale(HarmonyEngine::Mode::Ionian);

    // Set up simple sequence: C-D-E-F
    sequence.step(0).setNote(60); // C
    sequence.step(1).setNote(62); // D
    sequence.step(2).setNote(64); // E
    sequence.step(3).setNote(65); // F

    HarmonyTrackEngine masterEngine(engine, model, track, 0);
    masterEngine.setHarmonySequence(&sequence);

    // Tick at step 0 (C note, I degree)
    masterEngine.tick(48); // First step at 192 PPQN
    ASSERT_EQ(masterEngine.currentRootNote(), 60); // C

    auto chord = masterEngine.currentChord();
    ASSERT_EQ(chord.root, 60);   // C
    ASSERT_EQ(chord.third, 64);  // E (Major 3rd)
    ASSERT_EQ(chord.fifth, 67);  // G (Perfect 5th)
    ASSERT_EQ(chord.seventh, 71); // B (Major 7th)
}
```

**GREEN**: Implement master tick logic

```cpp
// In HarmonyTrackEngine.cpp
void HarmonyTrackEngine::tick(uint32_t tick) {
    if (!_sequence) return;

    if (isMaster()) {
        // Master track: Evaluate sequence step, calculate chord

        // Get current step (simplified - real impl uses sequence state)
        uint32_t stepTick = tick % (48 * 16); // 16 steps at 48 ticks each
        uint32_t stepIndex = stepTick / 48;

        const auto &step = _sequence->step(stepIndex);
        _currentRootNote = step.note(); // Base note from sequence

        // Get scale degree (simplified - assumes in-scale notes)
        uint8_t scaleDegree = stepIndex % 7; // TEMP: Use step index as degree

        // Get harmony engine from model
        // TEMP: Create local engine for Phase 1
        HarmonyEngine engine;
        engine.setMode(_sequence->scale()); // Use track's scale override
        engine.setDiatonicMode(true);

        // Calculate harmonized chord
        _currentChord = engine.harmonize(_currentRootNote, scaleDegree);

        // TODO Phase 1: Output root note to CV
        // TODO Phase 2: Add gate logic
    }
    else if (isIndependent()) {
        // Independent tracks behave like normal NoteTrack
        // TODO: Delegate to NoteTrackEngine logic
    }
    // Follower logic in Task 1.10
}
```

**REFACTOR**: Extract sequence evaluation, add proper step lookup.

---

#### Task 1.10: Follower Track Tick Logic (Week 2, Day 4)

**RED**: Test follower track harmonization

```cpp
TEST(HarmonyTrackEngine, DISABLED_FollowerHarmonization) {
    Engine engine;
    Model model;
    Track masterTrack, followerTrack;

    // Master setup
    HarmonySequence masterSeq(0); // Track 1
    masterSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    masterSeq.step(0).setNote(60); // C

    HarmonyTrackEngine masterEngine(engine, model, masterTrack, 0);
    masterEngine.setHarmonySequence(&masterSeq);

    // Follower setup (3rd follower)
    HarmonySequence followerSeq(1); // Track 2
    followerSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);
    followerSeq.setMasterTrackIndex(0); // Follow Track 1

    HarmonyTrackEngine followerEngine(engine, model, followerTrack, 1);
    followerEngine.setHarmonySequence(&followerSeq);
    followerEngine.setMasterEngine(&masterEngine);

    // CRITICAL: Master ticks BEFORE follower
    masterEngine.tick(48); // Calculate C Major 7 chord
    followerEngine.tick(48); // Read master's chord

    // Follower should output 3rd of C Major 7 = E (64)
    ASSERT_EQ(followerEngine.currentNote(), 64);
}

TEST(HarmonyTrackEngine, DISABLED_FollowerRoles) {
    // Test all three follower roles
    Engine engine;
    Model model;
    Track masterTrack, track3rd, track5th, track7th;

    HarmonySequence masterSeq(0);
    masterSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    masterSeq.step(0).setNote(60); // C Major 7

    HarmonyTrackEngine masterEngine(engine, model, masterTrack, 0);
    masterEngine.setHarmonySequence(&masterSeq);
    masterEngine.tick(48); // Calculate C-E-G-B

    // 3rd follower
    HarmonySequence seq3rd(1);
    seq3rd.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);
    HarmonyTrackEngine engine3rd(engine, model, track3rd, 1);
    engine3rd.setHarmonySequence(&seq3rd);
    engine3rd.setMasterEngine(&masterEngine);
    engine3rd.tick(48);
    ASSERT_EQ(engine3rd.currentNote(), 64); // E

    // 5th follower
    HarmonySequence seq5th(2);
    seq5th.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower5th);
    HarmonyTrackEngine engine5th(engine, model, track5th, 2);
    engine5th.setHarmonySequence(&seq5th);
    engine5th.setMasterEngine(&masterEngine);
    engine5th.tick(48);
    ASSERT_EQ(engine5th.currentNote(), 67); // G

    // 7th follower
    HarmonySequence seq7th(3);
    seq7th.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower7th);
    HarmonyTrackEngine engine7th(engine, model, track7th, 3);
    engine7th.setHarmonySequence(&seq7th);
    engine7th.setMasterEngine(&masterEngine);
    engine7th.tick(48);
    ASSERT_EQ(engine7th.currentNote(), 71); // B
}
```

**GREEN**: Implement follower logic

```cpp
// In HarmonyTrackEngine.h
public:
    int16_t currentNote() const { return _currentNote; }

private:
    int16_t _currentNote; // Current output note (for followers)
```

```cpp
// In HarmonyTrackEngine.cpp
void HarmonyTrackEngine::tick(uint32_t tick) {
    if (!_sequence) return;

    if (isMaster()) {
        // Master logic from Task 1.9
        // ...
    }
    else if (isFollower()) {
        // Follower track: Read master's chord, output appropriate degree

        if (!_masterEngine) {
            // No master assigned - output silence or root note
            _currentNote = 60;
            return;
        }

        // Get master's current chord
        const auto &chord = _masterEngine->currentChord();

        // Extract appropriate note based on follower role
        switch (_sequence->harmonyRole()) {
            case HarmonyEngine::HarmonyRole::Follower3rd:
                _currentNote = chord.third;
                break;
            case HarmonyEngine::HarmonyRole::Follower5th:
                _currentNote = chord.fifth;
                break;
            case HarmonyEngine::HarmonyRole::Follower7th:
                _currentNote = chord.seventh;
                break;
            default:
                _currentNote = chord.root;
        }

        // TODO Phase 1: Output note to CV
        // TODO Phase 3: Apply slew
    }
    else {
        // Independent track logic
        _currentNote = 60; // TEMP
    }
}
```

**REFACTOR**: Add master validity checks, defensive null handling.

---

#### Task 1.11: Master/Follower Tick Ordering (Week 2, Day 5)

**RED**: Test that tick ordering prevents stale data

```cpp
TEST(HarmonyEngine, DISABLED_MasterFollowerTickOrdering) {
    Engine engine;
    Model model;
    Track masterTrack, followerTrack;

    // Setup master (T1) and follower (T2)
    HarmonySequence masterSeq(0);
    masterSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    masterSeq.step(0).setNote(60); // C (step 0)
    masterSeq.step(1).setNote(62); // D (step 1)

    HarmonyTrackEngine masterEngine(engine, model, masterTrack, 0);
    masterEngine.setHarmonySequence(&masterSeq);

    HarmonySequence followerSeq(1);
    followerSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);

    HarmonyTrackEngine followerEngine(engine, model, followerTrack, 1);
    followerEngine.setHarmonySequence(&followerSeq);
    followerEngine.setMasterEngine(&masterEngine);

    // Step 0: C Major 7 chord
    masterEngine.tick(48);
    followerEngine.tick(48);
    ASSERT_EQ(followerEngine.currentNote(), 64); // E (3rd of C)

    // Step 1: D minor 7 chord
    masterEngine.tick(96);
    ASSERT_EQ(masterEngine.currentRootNote(), 62); // D

    followerEngine.tick(96);
    ASSERT_EQ(followerEngine.currentNote(), 65); // F (3rd of D minor)
    // NOT 64 (stale E from previous step) ❌
}
```

**GREEN**: Modify Engine::tick() to enforce ordering

```cpp
// File: src/apps/sequencer/engine/Engine.cpp (modifications)

void Engine::tick(uint32_t tick) {
    // PHASE 1: Tick all HarmonyRoot (Master) tracks FIRST
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isMaster()) {
            _harmonyTrackEngines[i].tick(tick);
            // Master now has fresh _currentChord
        }
    }

    // PHASE 2: Tick all Follower tracks SECOND
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isFollower()) {
            _harmonyTrackEngines[i].tick(tick);
            // Follower reads master's fresh chord
        }
    }

    // PHASE 3: Tick all Independent harmony tracks
    for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
        if (_harmonyTrackEngines[i].isIndependent()) {
            _harmonyTrackEngines[i].tick(tick);
        }
    }

    // Then tick other engine types (NoteTrack, CurveTrack, etc.)
    // ...
}
```

**REFACTOR**: Document ordering rationale, add assertions.

---

#### Task 1.12: Basic UI - Track Role Selection (Week 3, Days 1-2)

**RED**: Test HarmonyTrackConfigPage parameter access

```cpp
// File: src/tests/unit/sequencer/TestHarmonyUI.cpp

TEST(HarmonyTrackConfigPage, ParameterAccess) {
    Model model;
    HarmonySequence sequence(0); // Track 1
    HarmonyTrackConfigPage page(model, sequence);

    // Test ROLE parameter
    ASSERT_EQ(page.parameterCount(), 2); // ROLE, MASTER_TRK (Phase 1 only has 2 params)

    // Set to Master role
    page.setParameter(0); // ROLE parameter
    page.setValue(static_cast<int>(HarmonyEngine::HarmonyRole::Master));
    ASSERT_EQ(sequence.harmonyRole(), HarmonyEngine::HarmonyRole::Master);

    // Set to Follower3rd role
    page.setValue(static_cast<int>(HarmonyEngine::HarmonyRole::Follower3rd));
    ASSERT_EQ(sequence.harmonyRole(), HarmonyEngine::HarmonyRole::Follower3rd);
}

TEST(HarmonyTrackConfigPage, TrackConstraintEnforcement) {
    Model model;

    // Track 2 (index 1) - CANNOT be Master
    HarmonySequence track2(1);
    HarmonyTrackConfigPage page2(model, track2);

    page2.setParameter(0); // ROLE
    page2.setValue(static_cast<int>(HarmonyEngine::HarmonyRole::Master));

    // Should auto-revert to follower role
    ASSERT_NE(track2.harmonyRole(), HarmonyEngine::HarmonyRole::Master);
}
```

**GREEN**: Implement HarmonyTrackConfigPage

```cpp
// File: src/apps/sequencer/ui/pages/HarmonyTrackConfigPage.h

class HarmonyTrackConfigPage : public ListPage {
public:
    HarmonyTrackConfigPage(PageManager &manager, Model &model, HarmonySequence &sequence)
        : ListPage(manager)
        , _model(model)
        , _sequence(sequence)
    {}

    virtual void enter() override {
        ListPage::enter();
    }

    virtual void draw(Canvas &canvas) override {
        // Draw parameter list
        WindowPainter::drawActiveFunction(canvas, Function::Track);
        WindowPainter::drawHeader(canvas, _model, _model.project(), "HARMONY CONFIG");

        // Draw parameters (ROLE, MASTER_TRK)
        // ...
    }

    // ListPage interface
    virtual int parameterCount() const override { return 2; } // Phase 1

    virtual const char* parameterName(int index) const override {
        switch (index) {
            case 0: return "ROLE";
            case 1: return "MASTER TRK";
            default: return "";
        }
    }

    virtual int parameterValue(int index) const override {
        switch (index) {
            case 0: return static_cast<int>(_sequence.harmonyRole());
            case 1: return _sequence.masterTrackIndex();
            default: return 0;
        }
    }

    virtual void setParameterValue(int index, int value) override {
        switch (index) {
            case 0: // ROLE
                _sequence.setHarmonyRole(static_cast<HarmonyEngine::HarmonyRole>(value));
                break;
            case 1: // MASTER_TRK
                _sequence.setMasterTrackIndex(value);
                break;
        }
    }

    virtual const char* parameterValueText(int index, int value) const override {
        switch (index) {
            case 0: return harmonyRoleText(value);
            case 1: return trackIndexText(value);
            default: return "";
        }
    }

private:
    Model &_model;
    HarmonySequence &_sequence;

    const char* harmonyRoleText(int role) const {
        switch (static_cast<HarmonyEngine::HarmonyRole>(role)) {
            case HarmonyEngine::HarmonyRole::Independent: return "Independent";
            case HarmonyEngine::HarmonyRole::Master: return "HarmonyRoot";
            case HarmonyEngine::HarmonyRole::Follower3rd: return "3rd";
            case HarmonyEngine::HarmonyRole::Follower5th: return "5th";
            case HarmonyEngine::HarmonyRole::Follower7th: return "7th";
            default: return "Unknown";
        }
    }

    const char* trackIndexText(int index) const {
        static char buf[8];
        if (index < 0) return "None";
        snprintf(buf, sizeof(buf), "T%d", index + 1);
        return buf;
    }
};
```

**REFACTOR**: Add encoder editing, visual feedback for constraints.

---

#### Task 1.13: Integration with TopPage Navigation (Week 3, Day 2)

**RED**: Test navigation flow

```cpp
TEST(TopPage, HarmonyNavigation) {
    // Test that pressing Sequence button cycles to harmony config
    // (Manual test in simulator - no automated test for UI flow)
}
```

**GREEN**: Add to TopPage

```cpp
// File: src/apps/sequencer/ui/pages/TopPage.cpp (modifications)

void TopPage::functionShortPress(Function function) {
    switch (function) {
        case Function::Sequence:
            // Check if current track is harmony track
            auto &track = _project.selectedTrack();
            if (track.trackMode() == Track::TrackMode::Harmony) {
                _manager.pages().harmonyTrackConfig.show();
            } else {
                // Existing logic for NoteTrack, CurveTrack, etc.
            }
            break;
        // ...
    }
}
```

**REFACTOR**: Add HarmonyTrackConfigPage to PageManager.

---

#### Task 1.14: End-to-End Integration Test (Week 3, Day 3)

**RED**: Write integration test for complete workflow

```cpp
TEST(HarmonyIntegration, DISABLED_BasicChordProgression) {
    // Full integration test: Master + 3 followers play C-F-G-Am progression

    Engine engine;
    Model model;

    // Master track (T1): Play C-F-G-Am
    HarmonySequence masterSeq(0);
    masterSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    masterSeq.setScale(HarmonyEngine::Mode::Ionian);
    masterSeq.step(0).setNote(60); // C (I)
    masterSeq.step(1).setNote(65); // F (IV)
    masterSeq.step(2).setNote(67); // G (V)
    masterSeq.step(3).setNote(69); // A (vi)

    // Follower tracks
    HarmonySequence follower3rd(1), follower5th(2), follower7th(3);
    follower3rd.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);
    follower5th.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower5th);
    follower7th.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower7th);

    // Setup engines
    Track tracks[4];
    HarmonyTrackEngine engines[4] = {
        HarmonyTrackEngine(engine, model, tracks[0], 0),
        HarmonyTrackEngine(engine, model, tracks[1], 1),
        HarmonyTrackEngine(engine, model, tracks[2], 2),
        HarmonyTrackEngine(engine, model, tracks[3], 3)
    };

    engines[0].setHarmonySequence(&masterSeq);
    engines[1].setHarmonySequence(&follower3rd);
    engines[2].setHarmonySequence(&follower5th);
    engines[3].setHarmonySequence(&follower7th);

    engines[1].setMasterEngine(&engines[0]);
    engines[2].setMasterEngine(&engines[0]);
    engines[3].setMasterEngine(&engines[0]);

    // Step 0: C Major 7 (C-E-G-B)
    engines[0].tick(48);
    engines[1].tick(48);
    engines[2].tick(48);
    engines[3].tick(48);

    ASSERT_EQ(engines[0].currentRootNote(), 60); // C
    ASSERT_EQ(engines[1].currentNote(), 64);     // E
    ASSERT_EQ(engines[2].currentNote(), 67);     // G
    ASSERT_EQ(engines[3].currentNote(), 71);     // B

    // Step 1: F Major 7 (F-A-C-E)
    engines[0].tick(96);
    engines[1].tick(96);
    engines[2].tick(96);
    engines[3].tick(96);

    ASSERT_EQ(engines[0].currentRootNote(), 65); // F
    ASSERT_EQ(engines[1].currentNote(), 69);     // A
    ASSERT_EQ(engines[2].currentNote(), 72);     // C
    ASSERT_EQ(engines[3].currentNote(), 76);     // E

    // etc. for steps 2-3
}
```

**GREEN**: Fix any integration issues discovered.

**REFACTOR**: Clean up, optimize, document.

---

### Phase 1 Success Criteria

✅ **Must Have**:
- [ ] Master track sequences notes in Ionian mode
- [ ] Followers output correct diatonic chord tones (3rd, 5th, 7th)
- [ ] Track position constraints enforced (T1/T5 only can be HarmonyRoot)
- [ ] No audio glitches or crashes
- [ ] All unit tests pass (HarmonyEngine, HarmonySequence)
- [ ] User can create simple chord progressions in C Major
- [ ] Serialization works (save/load projects)

✅ **Testing**:
- [ ] 28 unit tests passing (7 Ionian degrees × 4 qualities)
- [ ] Integration test demonstrates C-F-G-Am progression
- [ ] Manual simulator testing confirms correct notes

📊 **Metrics**:
- Unit test coverage: >90% for HarmonyEngine
- Zero crashes during 5-minute continuous playback
- Memory overhead: <1KB per track

---

## PHASE 2: Modes & Inversions - 2-3 Weeks

**Goal**: Expand to all 7 Ionian modes, add 1st inversion, add transpose.

**Deliverable**: Users can sequence in any major/minor mode with inversions.

### Scope Additions

✅ **Added in Phase 2**:
- +6 Ionian modes (Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian)
- 1st inversion (in addition to root position)
- Transpose control (-24 to +24 semitones)
- Global HarmonyGlobalPage (HARM) for mode/inversion/transpose

❌ **Still Excluded**:
- Harmonic Minor modes (Phase 5+)
- 2nd/3rd inversions (Phase 5+)
- Drop2 voicing (Phase 3)
- Slew (Phase 3)
- Per-step voicing/inversion (Phase 3)

---

### Phase 2 Task Breakdown

---

#### Task 2.1: Parametrized Mode Testing (Week 4, Day 1)

**RED**: Test all 7 Ionian modes with parametrized testing

```cpp
// File: src/tests/unit/sequencer/TestHarmonyEngine.cpp

class HarmonyEngineModeTest : public ::testing::TestWithParam<HarmonyEngine::Mode> {};

TEST_P(HarmonyEngineModeTest, ScaleIntervalsCorrect) {
    HarmonyEngine engine;
    engine.setMode(GetParam());

    // Validate that all 7 scale degrees produce valid intervals
    for (int degree = 0; degree < 7; degree++) {
        int16_t interval = engine.getScaleInterval(degree);
        ASSERT_GE(interval, 0);
        ASSERT_LE(interval, 11); // Within octave
    }
}

TEST_P(HarmonyEngineModeTest, DiatonicQualitiesValid) {
    HarmonyEngine engine;
    engine.setMode(GetParam());

    // Validate that all 7 degrees have valid chord qualities
    for (int degree = 0; degree < 7; degree++) {
        auto quality = engine.getDiatonicQuality(degree);
        ASSERT_LT(static_cast<int>(quality), static_cast<int>(HarmonyEngine::ChordQuality::Last));
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllIonianModes,
    HarmonyEngineModeTest,
    ::testing::Values(
        HarmonyEngine::Mode::Ionian,
        HarmonyEngine::Mode::Dorian,
        HarmonyEngine::Mode::Phrygian,
        HarmonyEngine::Mode::Lydian,
        HarmonyEngine::Mode::Mixolydian,
        HarmonyEngine::Mode::Aeolian,
        HarmonyEngine::Mode::Locrian
    )
);
// Runs 7 modes × 2 tests = 14 tests automatically
```

**GREEN**: Scale intervals and diatonic tables already implemented in Phase 1 Task 1.3.

**REFACTOR**: Verify lookup tables match music theory (manual verification).

---

#### Task 2.2: Inversion Logic Implementation (Week 4, Days 2-3)

**RED**: Test root position and 1st inversion

```cpp
TEST(HarmonyEngine, RootPositionInversion) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);
    engine.setInversion(0); // Root position

    // C Major 7 in root position: C(60)-E(64)-G(67)-B(71)
    auto chord = engine.harmonize(60, 0);
    ASSERT_EQ(chord.root, 60);
    ASSERT_EQ(chord.third, 64);
    ASSERT_EQ(chord.fifth, 67);
    ASSERT_EQ(chord.seventh, 71);

    // Lowest note should be root
    int16_t lowestNote = std::min({chord.root, chord.third, chord.fifth, chord.seventh});
    ASSERT_EQ(lowestNote, chord.root);
}

TEST(HarmonyEngine, FirstInversion) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);
    engine.setInversion(1); // 1st inversion

    // C Major 7 in 1st inversion: E(64)-G(67)-B(71)-C(72)
    // 3rd moved to bass, root moved up an octave
    auto chord = engine.harmonize(60, 0);

    // Lowest note should be 3rd (E)
    int16_t lowestNote = std::min({chord.root, chord.third, chord.fifth, chord.seventh});
    ASSERT_EQ(lowestNote, chord.third);

    // Root should be up an octave
    ASSERT_EQ(chord.root, 72); // C up octave
    ASSERT_EQ(chord.third, 64); // E (now bass note)
    ASSERT_EQ(chord.fifth, 67); // G
    ASSERT_EQ(chord.seventh, 71); // B
}

TEST(HarmonyEngine, InversionBoundary) {
    HarmonyEngine engine;

    // Inversion values beyond 1 should clamp to 1 (Phase 2 limit)
    engine.setInversion(5);
    ASSERT_EQ(engine.inversion(), 1); // Clamped to max for Phase 2
}
```

**GREEN**: Implement inversion logic

```cpp
// In HarmonyEngine.cpp
HarmonyEngine::ChordNotes HarmonyEngine::harmonize(int16_t rootNote, uint8_t scaleDegree) const {
    ChordNotes chord;

    // Get chord quality and intervals (existing Phase 1 code)
    ChordQuality quality = getDiatonicQuality(scaleDegree);
    ChordIntervals intervals = getChordIntervals(quality);

    // Apply intervals to root note (existing Phase 1 code)
    chord.root = applyInterval(rootNote, intervals[0]);
    chord.third = applyInterval(rootNote, intervals[1]);
    chord.fifth = applyInterval(rootNote, intervals[2]);
    chord.seventh = applyInterval(rootNote, intervals[3]);

    // NEW Phase 2: Apply inversion
    applyInversion(chord);

    // Phase 3: Apply voicing
    // Phase 2: Apply transpose
    applyTranspose(chord);

    return chord;
}

void HarmonyEngine::applyInversion(ChordNotes &chord) const {
    if (_inversion == 0) return; // Root position, no change

    // Phase 2: Only support 1st inversion
    if (_inversion >= 1) {
        // 1st inversion: Move 3rd to bass, raise root by octave
        chord.root += 12; // Root up an octave
        // 3rd stays at original pitch (becomes bass)
    }

    // Phase 5+: Support 2nd and 3rd inversions
}
```

**REFACTOR**: Generalize for future 2nd/3rd inversions.

---

#### Task 2.3: Transpose Implementation (Week 4, Day 3)

**RED**: Test transpose across full range

```cpp
TEST(HarmonyEngine, TransposeUp) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Ionian);
    engine.setTranspose(12); // Up 1 octave

    // C Major 7 transposed up 1 octave
    auto chord = engine.harmonize(60, 0);
    ASSERT_EQ(chord.root, 72);   // C5
    ASSERT_EQ(chord.third, 76);  // E5
    ASSERT_EQ(chord.fifth, 79);  // G5
    ASSERT_EQ(chord.seventh, 83); // B5
}

TEST(HarmonyEngine, TransposeDown) {
    HarmonyEngine engine;
    engine.setTranspose(-12); // Down 1 octave

    auto chord = engine.harmonize(60, 0);
    ASSERT_EQ(chord.root, 48);   // C3
    ASSERT_EQ(chord.third, 52);  // E3
    ASSERT_EQ(chord.fifth, 55);  // G3
    ASSERT_EQ(chord.seventh, 59); // B3
}

TEST(HarmonyEngine, TransposeClamping) {
    HarmonyEngine engine;
    engine.setTranspose(30); // Beyond +24 limit
    ASSERT_EQ(engine.transpose(), 24); // Clamped to max

    engine.setTranspose(-30); // Beyond -24 limit
    ASSERT_EQ(engine.transpose(), -24); // Clamped to min
}

TEST(HarmonyEngine, TransposeMIDIRangeClamping) {
    HarmonyEngine engine;
    engine.setTranspose(24); // +2 octaves

    // High root note that would exceed MIDI 127
    auto chord = engine.harmonize(120, 0); // G#8
    // All notes should clamp to 127
    ASSERT_LE(chord.root, 127);
    ASSERT_LE(chord.third, 127);
    ASSERT_LE(chord.fifth, 127);
    ASSERT_LE(chord.seventh, 127);
}
```

**GREEN**: Implement transpose

```cpp
// In HarmonyEngine.cpp
void HarmonyEngine::applyTranspose(ChordNotes &chord) const {
    if (_transpose == 0) return;

    // Apply transpose to all chord notes
    chord.root = applyInterval(chord.root, _transpose);
    chord.third = applyInterval(chord.third, _transpose);
    chord.fifth = applyInterval(chord.fifth, _transpose);
    chord.seventh = applyInterval(chord.seventh, _transpose);

    // applyInterval() already clamps to MIDI range 0-127
}
```

**REFACTOR**: Refactor applyInterval to handle negative offsets.

---

#### Task 2.4: HarmonyGlobalPage Implementation (Week 5, Days 1-3)

**RED**: Test global harmony page parameter access

```cpp
TEST(HarmonyGlobalPage, ParameterAccess) {
    Model model;
    HarmonyEngine &engine = model.project().harmonyEngine(); // Add to Project
    HarmonyGlobalPage page(manager, model);

    // Phase 2: 3 parameters (MODE, INVERSION, TRANSPOSE)
    ASSERT_EQ(page.parameterCount(), 3);

    // Test MODE parameter
    page.setParameter(0); // MODE
    page.setValue(static_cast<int>(HarmonyEngine::Mode::Dorian));
    ASSERT_EQ(engine.mode(), HarmonyEngine::Mode::Dorian);

    // Test INVERSION parameter
    page.setParameter(1); // INVERSION
    page.setValue(1); // 1st inversion
    ASSERT_EQ(engine.inversion(), 1);

    // Test TRANSPOSE parameter
    page.setParameter(2); // TRANSPOSE
    page.setValue(12); // +1 octave
    ASSERT_EQ(engine.transpose(), 12);
}
```

**GREEN**: Implement HarmonyGlobalPage

```cpp
// File: src/apps/sequencer/ui/pages/HarmonyGlobalPage.h

class HarmonyGlobalPage : public ListPage {
public:
    HarmonyGlobalPage(PageManager &manager, Model &model)
        : ListPage(manager)
        , _model(model)
    {}

    virtual void draw(Canvas &canvas) override {
        WindowPainter::drawActiveFunction(canvas, Function::Sequence);
        WindowPainter::drawHeader(canvas, _model, _model.project(), "HARMONY GLOBAL");

        // Draw parameter list
        // ...
    }

    virtual int parameterCount() const override {
        return 3; // Phase 2: MODE, INVERSION, TRANSPOSE
        // Phase 3: +1 (VOICING)
        // Phase 4: +2 (QUALITY, DIATONIC)
    }

    virtual const char* parameterName(int index) const override {
        switch (index) {
            case 0: return "MODE";
            case 1: return "INVERSION";
            case 2: return "TRANSPOSE";
            default: return "";
        }
    }

    virtual int parameterValue(int index) const override {
        auto &engine = _model.project().harmonyEngine();
        switch (index) {
            case 0: return static_cast<int>(engine.mode());
            case 1: return engine.inversion();
            case 2: return engine.transpose();
            default: return 0;
        }
    }

    virtual void setParameterValue(int index, int value) override {
        auto &engine = _model.project().harmonyEngine();
        switch (index) {
            case 0: engine.setMode(static_cast<HarmonyEngine::Mode>(value)); break;
            case 1: engine.setInversion(value); break;
            case 2: engine.setTranspose(value); break;
        }
    }

    virtual const char* parameterValueText(int index, int value) const override {
        switch (index) {
            case 0: return modeText(value);
            case 1: return inversionText(value);
            case 2: return transposeText(value);
            default: return "";
        }
    }

private:
    Model &_model;

    const char* modeText(int mode) const {
        const char* names[] = {"Ionian", "Dorian", "Phrygian", "Lydian",
                               "Mixolydian", "Aeolian", "Locrian"};
        if (mode < 7) return names[mode];
        return "Unknown";
    }

    const char* inversionText(int inv) const {
        const char* names[] = {"Root", "1st"};
        if (inv < 2) return names[inv];
        return "Root"; // Default
    }

    const char* transposeText(int transpose) const {
        static char buf[8];
        snprintf(buf, sizeof(buf), "%+d", transpose);
        return buf;
    }
};
```

**REFACTOR**: Add encoder editing, visual chord display.

---

#### Task 2.5: Mode-Specific Chord Progression Tests (Week 5, Day 4)

**RED**: Test musically correct progressions in each mode

```cpp
TEST(HarmonyEngine, DorianProgression) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Dorian);

    // i-ii-IV progression in D Dorian
    // D minor 7 (i)
    auto dMin = engine.harmonize(62, 0);
    ASSERT_EQ(engine.getDiatonicQuality(0), HarmonyEngine::ChordQuality::Minor7);

    // E minor 7 (ii)
    auto eMin = engine.harmonize(64, 1);
    ASSERT_EQ(engine.getDiatonicQuality(1), HarmonyEngine::ChordQuality::Minor7);

    // G Major 7 (IV)
    auto gMaj = engine.harmonize(67, 3);
    ASSERT_EQ(engine.getDiatonicQuality(3), HarmonyEngine::ChordQuality::Major7);
}

TEST(HarmonyEngine, PhrygianProgression) {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Mode::Phrygian);

    // i-bII progression in E Phrygian (Spanish Phrygian sound)
    // E minor 7 (i)
    auto eMin = engine.harmonize(64, 0);
    ASSERT_EQ(engine.getDiatonicQuality(0), HarmonyEngine::ChordQuality::Minor7);

    // F Major 7 (bII)
    auto fMaj = engine.harmonize(65, 1);
    ASSERT_EQ(engine.getDiatonicQuality(1), HarmonyEngine::ChordQuality::Major7);
}

// Similar tests for Lydian, Mixolydian, Aeolian, Locrian
```

**GREEN**: Verify existing diatonic tables produce correct results.

**REFACTOR**: Add music theory comments explaining modal characteristics.

---

### Phase 2 Success Criteria

✅ **Must Have**:
- [ ] All 7 Ionian modes produce correct diatonic harmonies
- [ ] 1st inversion works correctly for all modes
- [ ] Transpose works across ±2 octave range
- [ ] HarmonyGlobalPage allows mode/inversion/transpose selection
- [ ] Smooth transitions between modes during playback
- [ ] Users can create progressions in any key/mode
- [ ] All Phase 1 functionality still works

✅ **Testing**:
- [ ] 7 modes × 7 degrees = 49 diatonic quality tests passing
- [ ] Inversion tests for all modes
- [ ] Transpose boundary tests

📊 **Metrics**:
- Test count: ~70 unit tests passing
- No regressions from Phase 1

---

## PHASE 3: Voicings, Slew & Per-Step Parameters - 1-2 Weeks

**Goal**: Add Drop2 voicing, configurable slew, per-step voicing/inversion layers.

**Deliverable**: Production-ready lite version with polished UX.

### Scope Additions

✅ **Added in Phase 3**:
- Drop2 voicing (in addition to Close voicing)
- Configurable slew per follower track
- Per-step voicing layer (F3 NOTE button)
- Per-step inversion layer (F3 NOTE button)
- Global voicing on HarmonyGlobalPage
- UI polish and visual feedback

❌ **Still Excluded**:
- Drop3/Spread voicings (Phase 5+)
- Manual chord quality selection (Phase 4)
- CV control (Phase 4)
- Performance mode (Phase 4)

---

### Phase 3 Task Breakdown

---

#### Task 3.1: Drop2 Voicing Implementation (Week 6, Days 1-2)

**RED**: Test Drop2 voicing algorithm

```cpp
TEST(HarmonyEngine, CloseVoicing) {
    HarmonyEngine engine;
    engine.setVoicing(HarmonyEngine::Voicing::Close);

    // C Major 7 in close voicing: All notes within octave
    // C(60)-E(64)-G(67)-B(71)
    auto chord = engine.harmonize(60, 0);

    int16_t span = std::max({chord.root, chord.third, chord.fifth, chord.seventh}) -
                   std::min({chord.root, chord.third, chord.fifth, chord.seventh});
    ASSERT_LE(span, 12); // Within one octave
}

TEST(HarmonyEngine, Drop2Voicing) {
    HarmonyEngine engine;
    engine.setVoicing(HarmonyEngine::Voicing::Drop2);

    // C Major 7 in Drop2: 2nd highest note dropped an octave
    // Standard close: C(60)-E(64)-G(67)-B(71)
    // Drop2: C(60)-G(55)-B(59)-E(64)
    //        root - 5th↓ - 7th↓ - 3rd
    auto chord = engine.harmonize(60, 0);

    // Expected Drop2 voicing from bass up: G(55)-B(59)-C(60)-E(64)
    // (or equivalent octave displacement)

    // Verify wider span than close voicing
    int16_t span = std::max({chord.root, chord.third, chord.fifth, chord.seventh}) -
                   std::min({chord.root, chord.third, chord.fifth, chord.seventh});
    ASSERT_GT(span, 12); // Wider than one octave
}
```

**GREEN**: Implement Drop2 voicing

```cpp
// In HarmonyEngine.cpp
void HarmonyEngine::applyVoicing(ChordNotes &chord) const {
    if (_voicing == Voicing::Close) return; // Default, no change

    if (_voicing == Voicing::Drop2) {
        // Drop2: Take 2nd-highest note and drop it an octave
        // Standard close voicing from low to high: root < third < fifth < seventh
        // Drop2: Move 2nd-highest (fifth in this case) down an octave

        // Assuming close voicing input: root(60)-third(64)-fifth(67)-seventh(71)
        // Drop the fifth down: fifth(67) -> fifth(55)
        chord.fifth -= 12;

        // Result: root(60), fifth(55), seventh(71), third(64)
        // Actual bass-up order: fifth(55) - seventh(59... wait, needs adjustment

        // BETTER: Sort notes, drop 2nd highest
        std::array<int16_t*, 4> notes = {&chord.root, &chord.third, &chord.fifth, &chord.seventh};
        std::sort(notes.begin(), notes.end(), [](int16_t* a, int16_t* b) { return *a < *b; });

        // Drop 2nd-highest (index 2 in sorted array)
        *notes[2] -= 12;
    }

    // Phase 5+: Drop3, Spread
}
```

**REFACTOR**: Optimize voicing algorithm, add more voicings in Phase 5+.

---

#### Task 3.2: Slew Implementation (Week 6, Days 2-3)

**RED**: Test slew smooths pitch transitions

```cpp
TEST(HarmonyTrackEngine, DISABLED_SlewBasic) {
    Engine engine;
    Model model;
    Track masterTrack, followerTrack;

    HarmonySequence masterSeq(0);
    masterSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Master);
    masterSeq.step(0).setNote(60); // C
    masterSeq.step(1).setNote(67); // G (large jump)

    HarmonySequence followerSeq(1);
    followerSeq.setHarmonyRole(HarmonyEngine::HarmonyRole::Follower3rd);
    followerSeq.setSlewAmount(64); // Medium slew

    HarmonyTrackEngine masterEngine(engine, model, masterTrack, 0);
    HarmonyTrackEngine followerEngine(engine, model, followerTrack, 1);
    masterEngine.setHarmonySequence(&masterSeq);
    followerEngine.setHarmonySequence(&followerSeq);
    followerEngine.setMasterEngine(&masterEngine);

    // Step 0: E (3rd of C)
    masterEngine.tick(48);
    followerEngine.tick(48);
    float cv0 = followerEngine.currentCV(); // E (64)

    // Step 1: B (3rd of G) - but slew should smooth the transition
    masterEngine.tick(96);
    followerEngine.tick(96);
    float cv1_immediate = followerEngine.currentCV();

    // CV shouldn't jump instantly to B (71), should be between E (64) and B (71)
    float targetCV = noteToCV(71);
    ASSERT_LT(cv1_immediate, targetCV); // Not yet at target
    ASSERT_GT(cv1_immediate, cv0);      // Moving towards target
}
```

**GREEN**: Implement slew in HarmonyTrackEngine

```cpp
// In HarmonyTrackEngine.h
private:
    float _currentCV;  // Current CV output (with slew applied)
    float _targetCV;   // Target CV (harmonized note)
    uint32_t _lastSlewTick;
```

```cpp
// In HarmonyTrackEngine.cpp
void HarmonyTrackEngine::tick(uint32_t tick) {
    // ... existing master/follower logic ...

    if (isFollower()) {
        // Get harmonized note (existing code)
        // ...

        // Convert note to CV
        _targetCV = noteToCV(_currentNote);

        // Apply slew
        applySlewToCV(tick);

        // Output slewed CV
        setCV(_currentCV);
    }
}

void HarmonyTrackEngine::applySlewToCV(uint32_t tick) {
    if (!_sequence) return;

    uint8_t slewAmount = _sequence->slewAmount();
    if (slewAmount == 0) {
        // No slew - instant transition
        _currentCV = _targetCV;
        return;
    }

    // Calculate slew rate based on slewAmount (0-127)
    float slewRate = slewAmount / 127.0f * 0.1f; // Tuning constant

    // Smooth transition towards target
    float delta = _targetCV - _currentCV;
    _currentCV += delta * slewRate;

    // Clamp when very close
    if (std::abs(delta) < 0.001f) {
        _currentCV = _targetCV;
    }
}

float HarmonyTrackEngine::noteToCV(int16_t midiNote) const {
    // Convert MIDI note to CV (1V/octave, C4=0V)
    return (midiNote - 60) / 12.0f;
}
```

**REFACTOR**: Tune slew algorithm for musical feel.

---

#### Task 3.3: Per-Step Voicing/Inversion Layers (Week 6, Days 4-5)

**RED**: Test per-step parameter storage

```cpp
TEST(NoteSequence, PerStepVoicingStorage) {
    NoteSequence sequence;

    // Set per-step voicing (stored in _data1 bitfield)
    sequence.step(0).setVoicing(HarmonyEngine::Voicing::Drop2);
    sequence.step(0).setHasLocalVoicing(true);

    ASSERT_TRUE(sequence.step(0).hasLocalVoicing());
    ASSERT_EQ(sequence.step(0).voicing(), HarmonyEngine::Voicing::Drop2);

    // Steps without local voicing use global default
    ASSERT_FALSE(sequence.step(1).hasLocalVoicing());
}

TEST(NoteSequence, PerStepInversionStorage) {
    NoteSequence sequence;

    sequence.step(0).setInversion(1); // 1st inversion
    sequence.step(0).setHasLocalInversion(true);

    ASSERT_TRUE(sequence.step(0).hasLocalInversion());
    ASSERT_EQ(sequence.step(0).inversion(), 1);
}

TEST(NoteSequence, GlobalAndLocalParameterInteraction) {
    NoteSequence sequence;
    HarmonyEngine engine;
    engine.setGlobalVoicing(HarmonyEngine::Voicing::Close);
    engine.setGlobalInversion(0);

    // Step without local override uses global
    ASSERT_EQ(sequence.step(0).effectiveVoicing(engine), HarmonyEngine::Voicing::Close);
    ASSERT_EQ(sequence.step(0).effectiveInversion(engine), 0);

    // Step with local override uses local
    sequence.step(1).setVoicing(HarmonyEngine::Voicing::Drop2);
    sequence.step(1).setHasLocalVoicing(true);
    ASSERT_EQ(sequence.step(1).effectiveVoicing(engine), HarmonyEngine::Voicing::Drop2);
}
```

**GREEN**: Extend NoteSequence::Step with harmony parameters

```cpp
// In NoteSequence.h
class Step {
public:
    // ... existing step data ...

    // Harmony parameters (Phase 3)
    void setVoicing(HarmonyEngine::Voicing v) {
        _data1.harmonyVoicing = static_cast<uint8_t>(v);
    }

    HarmonyEngine::Voicing voicing() const {
        return static_cast<HarmonyEngine::Voicing>(_data1.harmonyVoicing);
    }

    void setHasLocalVoicing(bool has) {
        _data1.hasLocalVoicing = has;
    }

    bool hasLocalVoicing() const {
        return _data1.hasLocalVoicing;
    }

    void setInversion(uint8_t inv) {
        _data1.harmonyInversion = inv & 0x3; // 2 bits, 0-3
    }

    uint8_t inversion() const {
        return _data1.harmonyInversion;
    }

    void setHasLocalInversion(bool has) {
        _data1.hasLocalInversion = has;
    }

    bool hasLocalInversion() const {
        return _data1.hasLocalInversion;
    }

    // Effective values (local if set, else global)
    HarmonyEngine::Voicing effectiveVoicing(const HarmonyEngine &engine) const {
        return hasLocalVoicing() ? voicing() : engine.globalVoicing();
    }

    uint8_t effectiveInversion(const HarmonyEngine &engine) const {
        return hasLocalInversion() ? inversion() : engine.globalInversion();
    }

private:
    union Data1 {
        struct {
            // ... existing fields ...
            uint32_t hasLocalVoicing : 1;
            uint32_t harmonyVoicing : 2;  // 0-3 (4 voicings)
            uint32_t hasLocalInversion : 1;
            uint32_t harmonyInversion : 2; // 0-3 (4 inversions)
            // Remaining bits for future use
        };
        uint32_t raw;
    } _data1;
};
```

**REFACTOR**: Verify bitfield packing doesn't overflow.

---

#### Task 3.4: F3 NOTE Button Layer Cycling (Week 7, Day 1)

**RED**: Test layer cycling includes harmony layers

```cpp
TEST(NoteSequenceEditPage, HarmonyLayerCycling) {
    // Test F3 NOTE button cycles through:
    // Note → NoteVariationRange → NoteVariationProbability →
    // HarmonyVoicing → HarmonyInversion → Note (wrap)

    NoteSequenceEditPage page;
    page.setLayer(NoteSequence::Layer::Note);

    page.cycleLayerForward(); // → NoteVariationRange
    ASSERT_EQ(page.layer(), NoteSequence::Layer::NoteVariationRange);

    page.cycleLayerForward(); // → NoteVariationProbability
    ASSERT_EQ(page.layer(), NoteSequence::Layer::NoteVariationProbability);

    page.cycleLayerForward(); // → HarmonyVoicing (NEW)
    ASSERT_EQ(page.layer(), NoteSequence::Layer::HarmonyVoicing);

    page.cycleLayerForward(); // → HarmonyInversion (NEW)
    ASSERT_EQ(page.layer(), NoteSequence::Layer::HarmonyInversion);

    page.cycleLayerForward(); // → Note (wrap)
    ASSERT_EQ(page.layer(), NoteSequence::Layer::Note);

    // Test reverse cycling
    page.cycleLayerBackward(); // → HarmonyInversion
    ASSERT_EQ(page.layer(), NoteSequence::Layer::HarmonyInversion);
}
```

**GREEN**: Add harmony layers to NoteSequence::Layer enum

```cpp
// In NoteSequence.h
enum class Layer {
    Gate,
    GateProbability,
    GateOffset,
    Slide,
    Retrigger,
    RetriggerProbability,
    PulseCount,        // Existing pulse count layer
    GateMode,          // Existing gate mode layer
    Length,
    LengthVariationRange,
    LengthVariationProbability,
    Note,
    NoteVariationRange,
    NoteVariationProbability,

    // NEW Phase 3: Harmony layers
    HarmonyVoicing,
    HarmonyInversion,

    // Existing
    Condition,
    AccumulatorTrigger,
    Last
};
```

**Modify NoteSequenceEditPage.cpp**:

```cpp
void NoteSequenceEditPage::functionShortPress(Function function) {
    if (function == Function::Note) {
        // Cycle through Note-related layers
        switch (_layer) {
            case Layer::Note:
                setLayer(Layer::NoteVariationRange);
                break;
            case Layer::NoteVariationRange:
                setLayer(Layer::NoteVariationProbability);
                break;
            case Layer::NoteVariationProbability:
                setLayer(Layer::HarmonyVoicing); // NEW
                break;
            case Layer::HarmonyVoicing:
                setLayer(Layer::HarmonyInversion); // NEW
                break;
            case Layer::HarmonyInversion:
                setLayer(Layer::Note); // Wrap back
                break;
            default:
                setLayer(Layer::Note);
        }
    }
}
```

**REFACTOR**: Update layer display strings, help text.

---

### Phase 3 Success Criteria

✅ **Must Have**:
- [ ] Drop2 voicing produces audibly different results from Close
- [ ] Slew smooths pitch transitions between steps
- [ ] Per-step voicing/inversion layers accessible via F3 NOTE button
- [ ] UI is intuitive and responsive
- [ ] Performance acceptable with 8 active harmony tracks
- [ ] Production-ready quality (no crashes, clean UI)

✅ **Testing**:
- [ ] Voicing tests for Close and Drop2
- [ ] Slew tests verify smooth transitions
- [ ] Layer cycling tests pass

📊 **Metrics**:
- All Phase 1 + Phase 2 tests still passing
- UI response time <100ms for parameter changes
- Audio glitch-free with 4 master + 12 follower tracks (stress test)

---

## PHASE 4: Advanced Features - 2-3 Weeks (Optional)

**Goal**: Add manual chord quality selection, CV control, performance mode.

**Scope**: Full-featured harmony system (except Harmonic Minor modes).

✅ **Added in Phase 4**:
- Manual chord quality selection (8 qualities)
- Diatonic vs Manual mode toggle
- CV input mapping (inversion, voicing, transpose)
- Performance mode (button keyboard)
- Unison mode

[Details omitted for brevity - expand based on Phase 1-3 patterns]

---

## PHASE 5+: Complete Feature Set (Future)

**Scope**: Harmonic Minor modes, all inversions/voicings, advanced features.

✅ **Added in Phase 5+**:
- 7 Harmonic Minor modes
- 2nd and 3rd inversions
- Drop3 and Spread voicings
- Additional chord qualities
- Voice leading optimization
- Arpeggiator integration

[Reserve for future expansion based on user feedback]

---

## Testing Strategy Summary

### Automated Testing

**Unit Tests** (Phase 1-3):
- `TestHarmonyEngine.cpp`: ~50 tests (scales, chords, inversions, voicings)
- `TestHarmonySequence.cpp`: ~20 tests (track constraints, scale override, serialization)
- `TestHarmonyTrackEngine.cpp`: ~15 tests (DISABLED, for integration)
- `TestHarmonyUI.cpp`: ~10 tests (page navigation, parameter ranges)

**Parametrized Tests**:
```cpp
// Automatically test all mode × quality combinations
INSTANTIATE_TEST_SUITE_P(AllCombinations, HarmonyTest,
    ::testing::Combine(
        ::testing::ValuesIn(allModes),     // 7 in Phase 2
        ::testing::ValuesIn(allQualities), // 4 in Phase 1, 8 in Phase 4
        ::testing::Range(0, 2),            // 2 inversions in Phase 2
        ::testing::ValuesIn(allVoicings)   // 2 in Phase 3
    )
);
// Phase 2: 7 × 4 × 2 × 1 = 56 tests
// Phase 3: 7 × 4 × 2 × 2 = 112 tests
// Phase 4: 7 × 8 × 2 × 2 = 224 tests
```

### Integration Testing

**Simulator Tests** (Manual):
- Basic chord progression (C-F-G-Am)
- Mode switching during playback
- Master/follower synchronization
- Pattern changes
- Project save/load
- Long-duration stability (5 min continuous play)

### Performance Benchmarking

```cpp
TEST(HarmonyPerformance, RealTimeConstraints) {
    HarmonyEngine engine;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; ++i) {
        engine.harmonize(60 + (i % 12), i % 7);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    auto avgMicros = duration.count() / 10000;

    // Must complete in <100μs per operation
    ASSERT_LT(avgMicros, 100);
}
```

---

## Risk Mitigation

### Technical Risks

| Risk | Phase | Mitigation | Status |
|------|-------|------------|--------|
| Track position constraint violations | 1 | Enforce in setHarmonyRole(), UI validation | ✅ Architected |
| Master/follower timing race | 1 | 3-phase tick ordering in Engine::tick() | ✅ Specified |
| Scale override conflicts | 1 | Per-HarmonyRoot scale storage | ✅ Designed |
| Memory overflow from bitfields | 3 | Static assertions, boundary tests | ⏳ Pending |
| Performance degradation | All | Profile early, benchmark continuously | ⏳ Pending |

### Project Risks

| Risk | Mitigation |
|------|------------|
| Scope creep | Strict phase gates, no mid-phase additions |
| Music theory errors | Parametrized testing, reference against Harmonàig manual |
| User confusion | Clear UI labels, example projects, documentation |
| Integration bugs | Daily builds, continuous integration, simulator testing |

---

## Success Metrics

### Phase 1 Gate (MUST PASS to proceed):
- [ ] All 28 unit tests passing
- [ ] C-F-G-Am progression plays correctly in simulator
- [ ] No crashes during 5-minute playback
- [ ] Actual effort within 50% of estimate (2-3 weeks → 1.5-4.5 weeks)

### Phase 2 Gate:
- [ ] All 7 modes produce correct diatonic chords
- [ ] ~70 unit tests passing
- [ ] Modal progressions sound musically correct

### Phase 3 Gate (Production-Ready):
- [ ] ~100 unit tests passing
- [ ] UI polish complete
- [ ] Performance <10% CPU overhead
- [ ] User acceptance testing positive

---

## Documentation Requirements

### Code Documentation:
- [ ] All public API methods have Doxygen comments
- [ ] Music theory formulas documented in lookup tables
- [ ] Architecture decision records (ADRs) for key choices

### User Documentation:
- [ ] CLAUDE.md updated with harmony feature section
- [ ] Example projects demonstrating each mode
- [ ] Quick start guide (5-minute tutorial)

### Testing Documentation:
- [ ] Test coverage report (>90% for HarmonyEngine)
- [ ] Known limitations documented
- [ ] Regression test suite established

---

## Timeline Summary

| Phase | Duration | Cumulative | Deliverable |
|-------|----------|------------|-------------|
| 1 MVP | 2-3 weeks | 2-3 weeks | Basic harmony working |
| 2 Modes | 2-3 weeks | 4-6 weeks | All modes + inversions |
| 3 Polish | 1-2 weeks | 5-8 weeks | Production-ready |
| **GATE** | 1 week buffer | **6-9 weeks** | **Lite version complete** |
| 4 Advanced | 2-3 weeks | 8-12 weeks | Full-featured (optional) |
| 5+ Future | TBD | TBD | Complete feature parity (optional) |

---

## Appendix A: File Structure

```
src/apps/sequencer/
├── model/
│   ├── HarmonyEngine.h/cpp         (Phase 1)
│   ├── HarmonySequence.h/cpp       (Phase 1)
│   └── Project.h                   (add HarmonyEngine member)
├── engine/
│   ├── HarmonyTrackEngine.h/cpp    (Phase 1)
│   └── Engine.cpp                  (modify tick ordering)
└── ui/pages/
    ├── HarmonyGlobalPage.h/cpp     (Phase 2)
    ├── HarmonyTrackConfigPage.h/cpp (Phase 1)
    ├── NoteSequenceEditPage.cpp    (modify for harmony layers, Phase 3)
    └── TopPage.cpp                 (add navigation)

src/tests/unit/sequencer/
├── TestHarmonyEngine.cpp           (Phase 1-3)
├── TestHarmonySequence.cpp         (Phase 1)
├── TestHarmonyTrackEngine.cpp      (Phase 1, DISABLED)
└── TestHarmonyUI.cpp               (Phase 2-3)
```

---

## Appendix B: Bitfield Layout

```cpp
// NoteSequence::Step::Data1 bitfield allocation
union Data1 {
    struct {
        // Existing fields (from prior features):
        uint32_t gate : 1;               // bit 0
        uint32_t gateProbability : 7;    // bits 1-7
        uint32_t gateOffset : 5;         // bits 8-12 (signed, ±15 ticks)
        uint32_t slide : 1;              // bit 13
        uint32_t retrigger : 4;          // bits 14-17 (0-15)
        uint32_t pulseCount : 3;         // bits 18-20 (0-7)
        uint32_t gateMode : 2;           // bits 21-22 (0-3)

        // NEW Phase 3: Harmony fields
        uint32_t hasLocalVoicing : 1;    // bit 23
        uint32_t harmonyVoicing : 2;     // bits 24-25 (0-3)
        uint32_t hasLocalInversion : 1;  // bit 26
        uint32_t harmonyInversion : 2;   // bits 27-28 (0-3)

        // REMAINING: bits 29-31 (3 bits free for future)
    };
    uint32_t raw;
};

static_assert(sizeof(Data1) == 4, "Data1 must be 32 bits");
```

---

**END OF WORKING-TDD-HARMONY-PLAN.md**

This plan is READY FOR IMPLEMENTATION. Proceed with Phase 1, Task 1.1.
