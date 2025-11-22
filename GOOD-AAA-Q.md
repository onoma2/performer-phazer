# Enhanced TuesdayTrack Algorithms: Aphex Twin, Ambient, Autechre

This document describes three new algorithms for the TuesdayTrack engine that follow the good algorithm design principles outlined in GOOD-ALGO.md. Each algorithm is purpose-driven, has a single clear concept, uses constrained pitch palettes, and employs intelligent randomness.

## Algorithm 18: APHEX TWIN (Pin-sort Style Rhythmic Complexity)

### Core Concept
"Pin-sort" style rhythmic complexity with microtonal shifting characteristic of Aphex Twin's work.

### Design Principles Applied
- **Single Clear Concept**: Complex polyrhythmic patterns with microtonal shifts
- **Rhythm First Structure**: Uses 3-against patterns as the foundation
- **Constrained Pitch Palette**: Based on Aphex's characteristic scales
- **Intelligent Randomness**: Used for microtonal shifts and pattern evolution, not note generation

### Algorithm Parameters
- **Flow**: Determines complexity level (2-4 internal voices)
- **Ornament**: Affects microtonal content and pitch selection
- **Gate Length**: 25%, 50%, 75%, or 100% based on rhythmic complexity

### Pitch Generation
- Uses 4 characteristic scales: Phrygian, Atonal, Whole tone, Augmented
- Interleaves internal "voices" through time creating a single melodic line
- Applies microtonal shifts (-2 to +2 semitones) based on Ornament parameter
- Creates monophonic sequence that captures the effect of multiple voices

### Implementation State Variables
```
uint8_t _aphexStep;              // Current step in the pin-sort sequence
uint8_t _aphexVoice;             // Current voice index for internal processing
uint8_t _aphexRhythmPattern[16]; // Complex polyrhythmic pattern
int8_t _aphexPitchSequence[16];  // Microtonal pitch sequence (-12 to +12)
uint8_t _aphexVoiceCount;        // Number of internal voices
int8_t _aphexMicrotonalShift;    // Current microtonal offset
uint8_t _aphexPatternIndex;      // Index in the pattern sequence
uint8_t _aphexActiveVoice;       // Which voice sounds this step
```

---

## Algorithm 13: AMBIENT (Evolutionary Drone with Micro-Tonal Harmonic Progression)

### Core Concept
Evolutionary drone with harmonic progression characteristic of ambient music.

### Design Principles Applied
- **Single Clear Concept**: Slow harmonic evolution with sustained textures
- **Strong Pitch Structure**: Harmonic series with sustained notes as foundation
- **Constrained Pitch Palette**: Based on select harmonic choices (chords, fifths)
- **Intelligent Randomness**: Used for evolution timing and texture selection

### Algorithm Parameters
- **Flow**: Determines harmonic complexity and evolution speed
- **Ornament**: Affects harmonic series choice and evolution rules
- **Gate Length**: 150%-400% for sustained, evolving textures

### Pitch Generation
- Uses 4 harmonic chord types: Major 7th, Minor 7th, Suspended, Second inversion
- Creates sustained, evolving textures through time-based sequencing
- Applies long gate lengths for ambient feel
- Implements slow pitch drift for ambient atmosphere

### Implementation State Variables
```
uint8_t _ambientDroneNote;       // Root drone note
uint8_t _ambientHarmonicIndex;   // Current position in harmonic series
int8_t _ambientHarmonicStack[8]; // Harmonic stack for internal harmony
uint8_t _ambientEvolutionStep;   // Step counter for harmonic evolution
uint8_t _ambientTexture;         // Current texture type
uint8_t _ambientDensity;         // Note density (0-16)
uint8_t _ambientDrift;           // Slow pitch drift amount
int8_t _ambientCurrentHeldNote;  // Currently held note for evolution
uint8_t _ambientPhase;           // Current phase of harmonic evolution
```

---

## Algorithm 19: AUTECHRE (Algorithmic Counterpoint with Mathematical Sequence Evolution)

### Core Concept
Algorithmic counterpoint with mathematical sequence evolution characteristic of Autechre's style.

### Design Principles Applied
- **Single Clear Concept**: Mathematical sequence counterpoint
- **Strong Structural Foundation**: Mathematical progressions as the base
- **Constrained Pitch Palette**: Derived from mathematical progressions
- **Intelligent Randomness**: Used to vary mathematical rules and sequence interaction

### Algorithm Parameters
- **Flow**: Determines mathematical sequence type (Fibonacci, Prime, Powers of 2)
- **Ornament**: Affects sequence interaction rules and evolution
- **Gate Length**: Variable (25%-175%) based on mathematical relationships

### Pitch Generation
- Uses 3 mathematical progressions: Fibonacci, Prime numbers, Powers of 2
- Creates single melodic line that alternates between sequence elements
- Applies 4 mathematical interaction rules: Addition, Subtraction, Multiplication, Interpolation
- Presents algorithmic counterpoint as a single evolving melodic sequence

### Implementation State Variables
```
uint8_t _autechreSequenceA[16];  // Primary mathematical sequence
uint8_t _autechreSequenceB[16];  // Secondary mathematical sequence
uint8_t _autechreSeqIndexA;      // Index in primary sequence
uint8_t _autechreSeqIndexB;      // Index in secondary sequence
uint8_t _autechreMathRule;       // Current mathematical rule being applied
uint8_t _autechreStep;           // Step counter for evolution
int8_t _autechreCurrentNote;     // Current note for this step
uint8_t _autechrePatternDensity; // Pattern complexity based on Flow
uint8_t _autechreCounterpoint;   // Counterpoint phase
```

---

## Design Principles Summary

All three algorithms follow the design principles from GOOD-ALGO.md:

### 1. Single, Clear Concept
Each algorithm focuses on one specific musical idea: Aphex Twin's pin-sort rhythms, Ambient's harmonic evolution, or Autechre's mathematical patterns.

### 2. Rhythm or Pitch Structure First
- Aphex: Rhythmic complexity foundation with 3-against patterns
- Ambient: Harmonic series foundation with sustained textures
- Autechre: Mathematical sequences as the structural base

### 3. Constrained, Purposeful Pitch Palettes
- Aphex: Scales characteristic of Aphex Twin's work
- Ambient: Harmonic chord choices and circle-of-fifths extensions
- Autechre: Notes derived from mathematical progressions

### 4. Intelligent Use of Randomness
- Aphex: Randomness for microtonal shifts and voice selection
- Ambient: Randomness for evolution timing and texture changes
- Autechre: Randomness for mathematical rule changes and sequence interaction

These algorithms avoid the issue of generic random note generation by focusing on specific musical concepts characteristic of each artist's work, using constrained pitch palettes, and employing randomness for variation within structured frameworks.