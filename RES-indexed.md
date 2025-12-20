This is a comprehensive architectural plan to port the **ER-101/102 (Indexed/List)** workflow to the **Westlicht Performer** hardware.

This plan assumes you are essentially forking the firmware to create a "Westlicht ER-Edition."

---

### **Phase 1: The Core Engine Rewrite (Time Accumulator)**

*Goal: Move from "Step per Clock Division" to "Step per Duration."*

**1. Data Structure Updates (`Sequence.h` / `Track.h`)**
The `Step` struct must be expanded. In standard x0x, "Gate Length" is a percentage. In ER-101, it is an absolute clock count.

```cpp
struct Step {
    uint8_t note_index;   // Points to Voltage Table (0-99)
    uint32_t duration;    // TICKS to wait before next step (0-max)
    uint32_t gate_length; // TICKS to hold the gate high
    uint8_t group_mask;   // Bitmask for ER-102 Groups (A-D)

    // Legacy support (optional)
    uint8_t velocity;
    uint8_t ratchet;
};

```

**2. The Track Class (`Track.cpp`)**
You need two new counters that persist between clock interrupts.

```cpp
class Track {
    uint32_t step_timer;      // Counts up to Step.duration
    uint32_t gate_timer;      // Counts down from Step.gate_length
    // ... existing members
};

```

**3. The Clock Interrupt (`Engine::tick`)**
This is the heart of the rewrite.

```cpp
void Track::tick() {
    // 1. Handle Gate (Counts down independently of step progress)
    if (gate_timer > 0) {
        gate_timer--;
        if (gate_timer == 0) {
            hardware_gate_write(LOW);
        }
    }

    // 2. Handle Duration (The Accumulator)
    step_timer++;
    Step& current = steps[current_step_index];

    // 3. Check for Step Transition
    if (step_timer >= current.duration) {
        advance_step(); // Logic to move index +1
    }
}

```

---

### **Phase 2: The "Indexed" Voltage System**

*Goal: Reutilize the `UserScale` system to act as a raw Voltage Table.*

**1. The "No-Modulo" Lookup**
Modify the pitch generation logic in `Track::process_cv`.

```cpp
float Track::calculate_pitch(Step& s) {
    if (CONFIG_ER_MODE) {
        // Direct Lookup: Index 13 is just the 13th voltage in the array.
        // No octave math (note / 12). No modulo (note % 12).
        return UserScale::get_voltage(s.note_index);
    } else {
        // Standard Westlicht Logic
        return Scale::get_voltage(s.note_index);
    }
}

```

**2. Table Editor UI**

* **Task:** Clone the existing `Scale Edit` page.
* **Modification:** Change the display formatting.
* *Old:* "C#", "D", "Eb"
* *New:* "+0.083V", "+0.166V" (Displaying the raw float value).


* **Input:** Allow the encoder to increment voltage in `0.001V` (1mV) steps or coarse `0.083V` (semitone) steps.

---

### **Phase 3: The List Editor (Insert/Delete/Copy)**

*Goal: Implement the "ER-101" editing workflow on a grid controller.*

**1. The "Insert" Logic**
The Performer normally has a fixed number of steps. You need a "Max Steps" buffer (e.g., 64) and a dynamic "Active Length."

```cpp
void Track::insert_step(int insertion_index) {
    // 1. Check bounds
    if (track_length >= MAX_STEPS) return;

    // 2. Shift data to the right
    for (int i = track_length; i > insertion_index; i--) {
        steps[i] = steps[i - 1];
    }

    // 3. Clone previous step into new slot (ER-Philosophy)
    steps[insertion_index] = steps[insertion_index - 1];

    // 4. Update length
    track_length++;
}

```

**2. The UI Mappings**

* **Insert:** Mapped to `FUNC + Encoder Click` (or similar). Triggers `insert_step(cursor_pos)`.
* **Delete:** Mapped to `FUNC + Encoder Long Press`. Shifts array left.
* **Copy/Paste:**
* **Action:** Copy Step `X` to Clipboard.
* **Action:** Paste at `Y`. Calls `insert_step(Y)` then overwrites data with Clipboard.



---

### **Phase 4: The Math Engine (Destructive)**

*Goal: Reutilize the "Edit" menu for batch processing.*

**1. The Math Function**
Create a generic function that takes an Operator, a Target, and a Value.

```cpp
void Track::apply_math(Range selection, Target target, Op operator, float value) {
    for (int i = selection.start; i <= selection.end; i++) {
        switch (target) {
            case CV:
                // Direct modification of the Index pointer
                if (operator == ADD) steps[i].note_index += (int)value;
                break;
            case DURATION:
                if (operator == MUL) steps[i].duration *= value; // Time stretch
                break;
            // ... cases for Gate, etc.
        }
    }
}

```

**2. Menu Integration**

* Add a **"Math"** entry to the Track Menu.
* **Sub-menu:**
1. `Target` (Note, Dur, Gate)
2. `Op` (+, -, *, /, Set, Rand)
3. `Value` (Encoder variable)
4. `Apply` (Executes the loop).



---

### **Phase 5: Group-Based CV Modulation (Non-Destructive)**

*Goal: Per-step group membership with sequence-level routing configuration.*

This approach is **more flexible than ER-102**: instead of hardwiring CV inputs to groups, we use generic routing targets and let each sequence decide what they affect.

**Architecture Overview:**

1. **Routing Layer**: Generic targets (`IndexedA`, `IndexedB`, optional `C`/`D`)
2. **Sequence Layer**: Each sequence configures what each route affects (groups + parameter + amount)
3. **Step Layer**: Each step declares group membership (bitmask)
4. **Engine Layer**: Applies modulation conditionally based on group membership

---

**1. Data Structure Updates**

Add to `IndexedSequence.h`:

```cpp
class IndexedSequence {
public:
    struct RouteConfig {
        uint8_t targetGroups;      // Bitmask: 0b1010 = groups A+C (bits 0-3)
        ModTarget targetParam;      // What to modulate
        float amount;               // Scale factor (-200% to +200%)
        bool enabled;

        void clear() {
            targetGroups = 0;
            targetParam = ModTarget::Duration;
            amount = 100.0f;
            enabled = false;
        }
    };

    enum class ModTarget : uint8_t {
        Duration,      // Modulate step duration (additive or multiplicative)
        GateLength,    // Modulate gate % (additive)
        NoteIndex      // Transpose note index (additive)
    };

    // Route config accessors
    const RouteConfig& routeA() const { return _routeA; }
    void setRouteA(const RouteConfig& cfg) { _routeA = cfg; }

    const RouteConfig& routeB() const { return _routeB; }
    void setRouteB(const RouteConfig& cfg) { _routeB = cfg; }

private:
    RouteConfig _routeA;
    RouteConfig _routeB;
    // Optional: RouteConfig _routeC, _routeD;
};
```

Update `Step` structure to include group mask:

```cpp
struct Step {
    uint32_t _packed;
    // bits 0-6:   note_index (7 bits)
    // bits 7-22:  duration (16 bits)
    // bits 23-31: gate_length (9 bits)

    uint8_t groupMask() const { return _groupMask; }
    void setGroupMask(uint8_t mask) { _groupMask = mask & 0x0F; }  // 4 bits (A-D)
    void toggleGroup(int groupIndex) {
        _groupMask ^= (1 << groupIndex);
    }

private:
    uint8_t _groupMask = 0;  // Additional byte: groups A-D (bits 0-3)
};
```

**Memory impact:** +1 byte per step = +256 bytes per sequence (still within budget)

---

**2. Routing Engine Integration**

Add generic indexed routing targets to `Routing.h`:

```cpp
enum class Target {
    // ... existing targets ...
    IndexedA,  // Generic indexed modulator A
    IndexedB,  // Generic indexed modulator B
    // Optional: IndexedC, IndexedD
};
```

These are **just voltage sources** - no built-in knowledge of what they modulate.

---

**3. IndexedTrackEngine Modulation Logic**

Apply modulation conditionally based on group membership:

```cpp
void IndexedTrackEngine::tick() {
    Step& currentStep = getCurrentStep();

    // Base values from step data
    uint16_t baseDuration = currentStep.duration();
    float baseGate = currentStep.gateLength() / 100.0f;  // Convert % to 0-1
    int8_t baseNote = currentStep.noteIndex();

    // Apply Route A modulation (if enabled and step is in target groups)
    if (_sequence->routeA().enabled) {
        float cvA = _routing->getValue(Routing::Target::IndexedA, _trackIndex);
        if (currentStep.groupMask() & _sequence->routeA().targetGroups) {
            applyModulation(cvA, _sequence->routeA(), baseDuration, baseGate, baseNote);
        }
    }

    // Apply Route B modulation
    if (_sequence->routeB().enabled) {
        float cvB = _routing->getValue(Routing::Target::IndexedB, _trackIndex);
        if (currentStep.groupMask() & _sequence->routeB().targetGroups) {
            applyModulation(cvB, _sequence->routeB(), baseDuration, baseGate, baseNote);
        }
    }

    // Use modulated values for playback...
    playStep(baseDuration, baseGate, baseNote);
}

void IndexedTrackEngine::applyModulation(
    float cv,                           // CV voltage (typically -5V to +5V, normalized to -1 to +1)
    const RouteConfig& cfg,
    uint16_t& duration,
    float& gate,
    int8_t& note
) {
    switch (cfg.targetParam) {
        case ModTarget::Duration:
            // Additive modulation (could also be multiplicative)
            duration = clamp(duration + int(cv * cfg.amount), 0, 65535);
            break;

        case ModTarget::GateLength:
            // Additive percentage
            gate = clamp(gate + (cv * cfg.amount / 100.0f), 0.0f, 1.0f);
            break;

        case ModTarget::NoteIndex:
            // Transpose (additive semitones)
            note = clamp(note + int(cv * cfg.amount), -63, 64);
            break;
    }
}
```

---

**4. UI Integration**

**Sequence Settings Page** (new page for route config):

```
INDEXED SEQUENCE SETTINGS

ROUTE A:
  Groups: [A][B][ ][ ]    ← Toggle group targets (visual feedback)
  Target: Duration        ← Encoder: Duration / Gate / Note
  Amount: +75%            ← Encoder: -200% to +200%
  Enable: ON              ← Toggle route on/off

ROUTE B:
  Groups: [ ][B][C][ ]
  Target: Note Index
  Amount: +100%
  Enable: ON
```

**Step Edit Page** (add group toggle):

```
Step 05 [Pattern 1]
  Duration: 192T          ← Encoder to edit
  Gate: 50%               ← Encoder to edit
  Note: 12 (+1.00V)       ← Encoder to edit
  Groups: [A][ ][C][ ]    ← Button press to toggle membership
```

**Group Assignment UI:**
- **Grid Mode**: Hold SHIFT + press step buttons to toggle group A membership
- **Function Keys**: F1-F4 select which group to edit (A/B/C/D)
- Visual feedback: LEDs show which steps are in currently selected group

---

**5. Advantages Over ER-102**

| Feature | ER-102 | Performer Indexed (this approach) |
|---------|--------|-----------------------------------|
| **CV-to-Group binding** | Hardwired (CV1→A, CV2→B) | Flexible (any route→any groups) |
| **Per-track variation** | No (global CV mapping) | Yes (Track 1 RouteA ≠ Track 2 RouteA) |
| **Parameter targeting** | Implied/fixed | Explicit (Duration/Gate/Note per route) |
| **Multi-group targeting** | No | Yes (RouteA can affect A+C simultaneously) |
| **Per-pattern config** | No | Yes (Pattern 1 and 2 can use routes differently) |
| **Amount scaling** | Fixed | Variable (-200% to +200% per route) |

**Example Use Case:**

Track 1 (Bass):
- RouteA: Groups=A, Target=Duration, Amount=+50% ← LFO stretches kick timing
- RouteB: Groups=B+C, Target=Note, Amount=+100% ← External CV transposes fills

Track 2 (Lead):
- RouteA: Groups=A+B+C, Target=Gate, Amount=-25% ← LFO shortens all notes
- RouteB: Groups=D, Target=Duration, Amount=+200% ← External CV creates long drones

Same routing sources (IndexedA, IndexedB) achieve completely different musical results per track!

---

### Feasibility / Resource Fit

**Current System Status (from updated_resource_analysis.md):**
- SRAM: 110KB/128KB used (86%) → **~18KB available**
- Largest sequence type (NoteSequence): ~2.5KB in Track union
- Track storage uses union pattern (all tracks sized to largest member)

**Voltage Table Integration:**
- ✅ **Use existing Scale system** - no separate voltage table needed
- IndexedSequence uses `Scale::get_voltage(note_index)` for direct lookup
- Zero additional memory cost - Scale system already exists
- Per-project customization via existing Scale editor UI
- 64+ entries available (enough for ER-101 workflow)

**Step Data Layout (Conservative - 32 steps × 8 patterns):**

Bit-packed step structure:
```cpp
struct Step {
    uint32_t _packed;  // 32 bits total:
    // bits 0-6:   note_index (7 bits = 0-127, using 0-63 for Scale lookup)
    // bits 7-22:  duration (16 bits = direct tick count, 0-65535)
    // bits 23-31: gate_length (9 bits = 0-511, as % of duration)
};
```

Duration encoding (Option A: Direct Tick Count):
- **16-bit direct tick count**: 0 to 65,535 ticks
- ✅ **Arbitrary tick values** - no quantization to musical grid
- At 192 PPQN: 0 to 341 quarter notes = **~85 bars** (4/4 time)
- Sufficient range for eurorack sequences (typical: 8-16 bars, longest: ~32 bars)
- **UI Encoder Behavior**: By default, encoder turns increment/decrement duration by current divisor value (in ticks)
  - Example: If divisor = 48T (8th note), encoder click = ±48T steps
  - SHIFT+encoder: Fine adjustment (±1 tick) for precise control
  - This makes musical editing intuitive while preserving arbitrary tick capability

**Memory Calculation:**

Per-sequence data:
- Step data (packed): 32 steps × 8 patterns × 4 bytes = 1,024 bytes
- Step group masks: 32 steps × 8 patterns × 1 byte = 256 bytes
- Sequence config: ~200 bytes (divisor, loop, scale, etc.)
- Pattern metadata: ~100 bytes
- RouteConfig A+B: 2 × 8 bytes = 16 bytes
- **Total: ~1.6KB**

Union impact:
- IndexedSequence in union: ~3KB (with padding/alignment)
- Current largest (NoteSequence): ~2.5KB
- **Overhead per track: 0.5KB**
- **Total for 8 tracks: 8 × 0.5KB = 4KB**

Engine state (per track):
- step_timer, gate_timer, playback state: ~300 bytes
- **Total for 8 tracks: 2.4KB**

**TOTAL OVERHEAD: ~6.5KB** ✅ Well within 18KB available

**Track Count Consideration:**
- Limiting to 4 tracks (tracks 1-4) does NOT save RAM in union-based architecture
- Union size = largest member, affects all 8 tracks equally
- Can optionally enforce 4-track limit in UI for workflow reasons
- Alternative storage (external arrays) costs MORE (12KB vs 4KB)
- **Recommendation:** Add to union, allow on any track, 4KB overhead acceptable

**Feature Constraints:**
- **Voltage/index model:** Reuses Scale system for raw voltage lookup; direct index-to-voltage, no modulo/octave math
- **Step capacity:** 32 steps max (vs ER-101's 64) to keep memory footprint conservative
- **Pattern capacity:** 8 patterns (vs 16) to fit in union constraints
- **Engine:** Duration-driven tick (step timer + gate timer) self-contained in `IndexedTrackEngine`; ~300B state
- **Groups:** Per-step group mask (4 bits) for ER-102 style conditional routing
- **Math/edit:** Destructive math (±,×,÷ on duration/gate/index) operates on step arrays
- **RAM impact:** ~3KB per sequence in union, ~6.5KB total system overhead

```

### **Feasibility Checklist**

| Feature | Difficulty | Hardware Limitation? | Notes |
| --- | --- | --- | --- |
| **Duration Engine** | 🟡 Medium | No | Requires rewriting `tick()` logic completely. |
| **Voltage Tables** | 🟢 Easy | No | Just a display hack of `UserScale`. |
| **Insert/Delete** | 🟡 Medium | RAM | Max steps per track might need to be limited (e.g., 64) to prevent RAM overflow. |
| **Math Ops** | 🟢 Easy | No | Standard C++ math logic. |
| **OLED UI** | 🔴 Hard | Screen Size | Visualizing variable step lengths on a 128x64 screen is tricky. Stick to a numerical list view or a "current step" focused view. |

### **Implementation Roadmap**

**Phase 0: Foundation (Conservative Approach - 32 steps, 8 patterns)**

1. **Add TrackMode::Indexed** to Track.h enum
2. **Create IndexedSequence model** (`model/IndexedSequence.h/cpp`)
   - 32-bit bit-packed Step structure
   - 32 steps × 8 patterns capacity
   - Scale integration for voltage lookup
   - Serialization (write/read methods)
3. **Add to Track union** (`model/Track.h`)
   - Accept 4KB overhead for all 8 tracks
   - Any track can be Indexed

**Phase 1: Duration Engine**

4. **Create IndexedTrackEngine** (`engine/IndexedTrackEngine.h/cpp`)
   - Tick-based accumulator (step_timer, gate_timer)
   - Duration-driven step advancement
   - Gate countdown independent of step progress
   - Pattern switching logic
5. **Register in TrackEngineContainer**
6. **Test basic playback** with hardcoded sequence

**Phase 2: Scale System Integration**

7. **Direct voltage lookup** in `IndexedTrackEngine::calculateCV()`
   - Use `Scale::get_voltage(step.noteIndex())`
   - No octave math, no modulo
8. **Scale selection UI** (reuse existing Scale parameter)
9. **Test pitch output** across Scale entries

**Phase 3: Basic UI (List View)**

10. **Create IndexedSequencePage** (`ui/pages/IndexedSequencePage.h/cpp`)
    - Numerical list view (avoid timeline visualization)
    - Display: Step# | Duration | Gate | Note | CV
    - Cursor navigation (encoder scroll)
11. **Step editing**
    - Edit mode: Duration / Gate / Note selection
    - Duration encoder behavior:
      - Normal: increment/decrement by divisor (e.g., ±48T if divisor=48T)
      - SHIFT: fine adjustment (±1 tick)
    - Gate encoder: adjust percentage (0-100%)
    - Note encoder: increment/decrement note index (±1)
12. **Pattern selection** (reuse existing pattern UI)

**Phase 4: Insert/Delete Operations**

13. **Dynamic length tracking** (`_activeLength` field)
14. **Insert step** (FUNC + Button)
    - Shift array right
    - Clone previous step
    - Increment _activeLength
15. **Delete step** (FUNC + Long Press)
    - Shift array left
    - Decrement _activeLength
16. **Bounds checking** (max 32 steps)

**Phase 5: Group-Based CV Modulation**

17. **Add routing targets** to Routing.h
    - Routing::Target::IndexedA
    - Routing::Target::IndexedB
    - Register in routing engine getValue()
18. **Extend IndexedSequence model**
    - Add RouteConfig struct (groups, target param, amount, enabled)
    - Add ModTarget enum (Duration, GateLength, NoteIndex)
    - Add _routeA, _routeB members
    - Serialization support
19. **Update Step structure**
    - Add _groupMask member (1 byte, 4 bits used)
    - Add groupMask(), setGroupMask(), toggleGroup() methods
    - Serialization support
20. **Implement engine modulation logic**
    - Read RouteConfig from sequence
    - Get CV values from routing engine
    - Check group membership before applying
    - applyModulation() function for Duration/Gate/Note
21. **Create Route Config UI page**
    - Display Route A/B settings
    - Edit target groups (visual toggle)
    - Edit target parameter (encoder: Duration/Gate/Note)
    - Edit amount (-200% to +200%)
    - Toggle route enable/disable
22. **Add group editing to Step Edit page**
    - Display current step's group membership
    - Button press to toggle groups A/B/C/D
23. **Group assignment grid mode**
    - SHIFT + step buttons to toggle group membership
    - Function keys F1-F4 select active group
    - LED feedback showing group membership

**Phase 6: Advanced Features (Optional)**

24. **Math operations** submenu
    - Target: Duration / Gate / Note
    - Op: Add / Subtract / Multiply / Divide / Set / Random
    - Apply to selection/all steps
25. **Copy/Paste** clipboard operations
26. **Step probability** (per-step trigger chance)

**Testing Strategy:**

- ✅ Model: Unit tests for bit-packing, duration encoding, insert/delete, group masks
- ✅ Engine: Integration tests for tick accumulator, gate timing
- ✅ Routing: Test group-conditional modulation (CV only affects target groups)
- ✅ RouteConfig: Test Duration/Gate/Note modulation with various amounts
- ✅ Memory: Build with all 8 tracks as Indexed, verify RAM usage (~1.6KB/seq)
- ✅ Scale: Test all Scale entries for correct voltage output
- ⚠️ Hardware: Timing validation on STM32 (simulator timing differs)

**Recommended Starting Point:**

Start with **Phase 0-2** to get basic playback working with Scale integration. This proves the core concept (duration engine + voltage lookup) with minimal UI complexity. Then add UI in Phase 3-4 once engine is stable.

**Phase 5 (Group Modulation)** can be added incrementally after basic functionality works - it's non-destructive and doesn't affect core playback if routes are disabled.
