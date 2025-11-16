## Brainstorm: Implementing Accumulator & Pulse Count in PER|FORMER

### 1. **Accumulator Implementation Strategy**

#### Core Architecture Changes

**New Data Model Components** (`src/apps/sequencer/model/`):
```cpp
// New Accumulator.h
class Accumulator {
    enum Mode { Stage, Track };
    enum Polarity { Unipolar, Bipolar };
    enum Direction { Up, Down, Freeze };
    enum Order { Wrap, Pendulum, Random, Hold };

    int16_t _currentValue = 0;
    int16_t _minValue = -7;
    int16_t _maxValue = 7;
    uint8_t _stepValue = 1;
    bool _pendulumDirection = true;

    // State tracking
    uint32_t _loopCount = 0;
    int _lastTriggeredStage = -1;
};
```

**Integration Points**:

1. **NoteSequence Enhancement** (`NoteSequence.h`):
   - Add accumulator instance per sequence
   - Store accumulation target (pitch, gate length, probability, etc.)
   - Track which steps trigger accumulation

2. **NoteTrackEngine Modifications** (`NoteTrackEngine.cpp`):
   ```cpp
   // In sequence advance logic
   if (sequence.accumulator().enabled()) {
       if (shouldAccumulate(currentStep, loopBoundary)) {
           int16_t accum = sequence.accumulator().tick();
           // Apply to chosen parameter
           transposition += accum;
       }
   }
   ```

3. **PlayState Integration**:
   - Track loop counts for accumulator
   - Handle reset conditions
   - Freeze state for performance control

#### Key Implementation Decisions

**Memory Efficiency**:
- Store accumulator state in bitfield (current PER|FORMER pattern)
- Only allocate when enabled (save RAM for 8 tracks)

**Parameter Targets**:
- **Primary**: Note transposition (adhering current scale)
- **Secondary**(optional):
  - CV output offset
  - Gate probability
  - Step length multiplier
  - Sequence direction

**UI Integration**:
- New layer in NOTE page: "ACCUM" to enable MODE (accessed via pressing NOTE button)
- New layer in NOTE page: "ACCST" to determine amount of steps (accessed via pressing NOTE button)
- Quick edit: Encoder for value, buttons for mode
- Visual feedback: Show current accumulation offset

### 2. **Pulse Count/Ratcheting Implementation**

#### Architecture Approach

**Per-Step Data Extension**:
```cpp
// Extend NoteSequence::Step
struct Step {
    // Existing...
    uint8_t note;
    uint8_t gate;

    // New ratcheting fields (4 bits each = 1 byte total)
    uint8_t pulseCount : 4;  // 1-8 pulses
    uint8_t gateType : 4;    // Rest, Tie, Single, Repeat
};
```

**Engine Processing**:
```cpp
// In NoteTrackEngine::tick()
if (isStepActive(currentStep)) {
    uint8_t pulses = step.pulseCount;
    uint32_t pulseDuration = stepDuration / pulses;

    // Subdivide the step
    uint32_t pulseIndex = (tickInStep * pulses) / stepDuration;

    switch(step.gateType) {
        case GateType::Repeat:
            if (tickInPulse == 0) triggerGate();
            break;
        case GateType::Hold:
            gateActive = true;
            break;
        // etc...
    }
}
```

#### Ratcheting Modes

**Gate Type Options**:
1. **Rest**: No gates (creates space)
2. **Tie**: Extends previous gate (legato)
3. **Single**: One gate at step start
4. **Repeat**: Multiple gates per pulse count
5. **Flam**: Quick double-hit at start
6. **Accelerando**: Speeds up subdivisions
7. **Ritardando**: Slows down subdivisions
8. **Probability**: Random gate per subdivision

### 3. **Synergy Between Features**

**Combined Effects**:
- Accumulator affecting ratchet density
- Pulse count modulating accumulator speed
- Cross-track accumulation (Track 1 affects Track 2)

**Performance Controls**:
```cpp
// Quick performance gestures
if (shift_held && encoder_turned) {
    // Real-time accumulator offset
    track.accumulator().nudge(encoder_delta);
}

if (pattern_button_held) {
    // Freeze all accumulators
    project.freezeAccumulators();
}
```

### 4. **Implementation Phases**

**Phase 1: Core Accumulator** (2-3 weeks)
- Basic accumulator data model
- Track-level accumulation
- Pitch transposition only
- Simple UI page

**Phase 2: Advanced Accumulator** (1-2 weeks)
- Per-stage accumulation
- Multiple parameter targets
- Pendulum/Random/Hold modes
- Performance controls

**Phase 3: Basic Ratcheting** (2 weeks)
- Pulse count per step
- Basic gate types (Single/Repeat)
- UI for editing

**Phase 4: Advanced Ratcheting** (1 week)
- Complex gate types
- Probability per subdivision
- Swing/humanization

**Phase 5: Integration** (1 week)
- Cross-modulation
- Pattern save/load
- MIDI output support

### 5. **Technical Challenges & Solutions**

**Challenge**: Memory constraints (STM32F405 has 192KB RAM)
**Solution**:
- Use bitfields aggressively
- Share accumulator instances between tracks
- Conditional compilation for features

**Challenge**: Timing precision for ratchets
**Solution**:
- Pre-calculate subdivision points
- Use integer math (avoid floating point)
- Leverage existing Clock system's high resolution

**Challenge**: UI complexity
**Solution**:
- Modal editing (normal vs accumulator mode)
- Use LED brightness for accumulator visualization
- Context-sensitive encoder behavior

### 6. **Unique PER|FORMER Enhancements**

**Beyond Metropolix**:
1. **CV Shape Accumulation**: Apply to curve sequences
2. **Euclidean Integration**: Accumulate rhythm density
3. **Scale-Aware**: Accumulate within scale constraints
4. **Multi-Track Coupling**: Master accumulator affecting all tracks
5. **Recording**: Record accumulator movements as automation

**Performance Features**:
- **Accumulator Scenes**: Save/recall accumulator states
- **Gesture Recording**: Capture freeze/unfreeze patterns
- **MIDI Learn**: Map external controllers to accumulator params

### 7. **File Format Considerations**

```cpp
// Extend project save format
void NoteSequence::write(VersionedSerializedWriter &writer) {
    // Existing...
    writer.write(_steps);

    // New v2 fields
    if (writer.version() >= ProjectVersion::Version2) {
        writer.write(_accumulator.enabled);
        writer.write(_accumulator.mode);
        writer.write(_accumulator.target);
        // Ratchet data embedded in steps
    }
}
```


