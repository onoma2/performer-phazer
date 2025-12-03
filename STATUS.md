# Performer Phazer Project Status

## Current Session Progress (Tuesday Refactor)

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

**Verification:**
*   Unit Test `TestTuesdayRefactor` compiles and links correctly.
*   Runtime behavior in Simulator:
    *   Algorithms evolve and produce expected musical results for straightforward (non-polyrhythmic) settings.
    *   **Current Issue:** **Polyrhythmic timing (Gate Offset) and Note Count for Autechre/StepWave/Aphex are not working as expected.**
        *   **Problem 1: Gate Offset is ignored.** Despite `generateStep` calculating `result.gateOffset` and `tick()` assigning it to `_gateOffset`, the output gates are still firing on straight 16th note step starts.
        *   **Problem 2: Note Count is Incorrect.** When attempting polyrhythms (e.g., 7 notes over 16 steps), the actual number of triggered notes is often 6, and the rhythmic density masking does not appear to be fully effective.

**Failed Solutions / Investigation Notes:**
*   The `_cachedLoopLength` issue was fixed, which resolved the pattern freezing.
*   The `_gateOffset` was successfully added to the `TuesdayTickResult` struct.
*   The `tick()` function was modified to assign `_gateOffset = result.gateOffset;`.
*   The `tick()` function's `densityGate` logic was modified to respect `result.velocity == 0` (masking for polyrhythms).
*   The core issue appears to be that the **`_pendingGateOffsetTicks` mechanism** is either not being correctly populated from `_gateOffset` or is being overridden. Additionally, the note count (6 vs 7) for polyrhythms suggests an issue with how the masking logic aligns with the `_pendingGateOffsetTicks` system.

## Next Steps
*   Investigate the interaction between `_gateOffset`, `_pendingGateOffsetTicks`, and `_pendingGateActivation` within the `tick()` and `update()` functions to pinpoint where the gate offset is being lost or reset.
*   Further debug the polyrhythmic masking logic to ensure accurate note count.