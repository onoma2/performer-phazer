#include "TuesdayTrackEngine.h"

#include "Engine.h"
#include "model/Scale.h"

#include <cmath>
#include <algorithm>

// Initialize algorithm state based on Flow (seed1) and Ornament (seed2)
// This mirrors the original Tuesday Init functions
void TuesdayTrackEngine::initAlgorithm() {
    const auto &_tuesdayTrackRef = tuesdayTrack();
    const auto &sequence = _tuesdayTrackRef.sequence(pattern());
    int flow = sequence.flow();
    int ornament = sequence.ornament();
    int algorithm = sequence.algorithm();

    _uiRng = Random(flow * 37 + ornament * 101);

    _cachedFlow = flow;
    _cachedOrnament = ornament;
    _cachedAlgorithm = algorithm;

    switch (algorithm) {
    case 0: // TEST
        // seed1 (flow) determines mode and sweep speed
        // seed2 (ornament) determines accent and velocity
        _testMode = (flow - 1) >> 3;  // 0 or 1 (1-8 = mode 0, 9-16 = mode 1)
        _testSweepSpeed = ((flow - 1) & 0x3);  // 0-3
        _testAccent = (ornament - 1) >> 3;  // 0 or 1
        _testVelocity = ((ornament - 1) << 4);  // 0-240
        _testNote = 0;
        break;

    case 1: // TRITRANCE
        // seed1 (flow) seeds main RNG for b1, b2
        // seed2 (ornament) seeds extra RNG for b3
        _rng = Random((flow - 1) << 4);
        _extraRng = Random((ornament - 1) << 4);

        _triB1 = (_rng.next() & 0x7);   // High note for case 2
        _triB2 = (_rng.next() & 0x7);   // Phase offset for mod 3

        // b3: note offset for octave 0/1
        _triB3 = (_extraRng.next() & 0x15);
        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
        _triB3 -= 4;  // Range: -4 to +3
        break;

    case 2: // STOMPER
        // seed2 (ornament) seeds main RNG for note choices
        // seed1 (flow) seeds extra RNG for mode/pattern
        _rng = Random((ornament - 1) << 4);
        _extraRng = Random((flow - 1) << 4);

        _stomperMode = (_extraRng.next() % 7) * 2;  // Initial pattern mode
        _stomperCountDown = 0;
        _stomperLowNote = _rng.next() % 3;
        _stomperLastNote = _stomperLowNote;
        _stomperLastOctave = 0;
        _stomperHighNote[0] = _rng.next() % 7;
        _stomperHighNote[1] = _rng.next() % 5;
        break;

    case 3: // APHEX - Polyrhythmic Event Sequencer (previously algorithm 18)
{
    // 1. Seed the patterns based on Flow, using the main RNG
    _rng = Random((sequence.flow() - 1) << 4);
    for (int i = 0; i < 4; ++i) _aphex_track1_pattern[i] = _rng.next() % 12;
    for (int i = 0; i < 3; ++i) _aphex_track2_pattern[i] = _rng.next() % 3; // 0, 1, or 2
    for (int i = 0; i < 5; ++i) _aphex_track3_pattern[i] = (_rng.next() % 8 == 0) ? (_rng.next() % 5) : 0; // Sparse bass notes

    // 2. Set initial phase based on Ornament
    int ornament_val = sequence.ornament();
    _aphex_pos1 = (ornament_val * 1) % 4;
    _aphex_pos2 = (ornament_val * 2) % 3;
    _aphex_pos3 = (ornament_val * 3) % 5;
    break;
}

    case 4: // AUTECHRE - Algorithmic Transformation Engine (previously algorithm 19)
{
    // 1. Start with octave-jumping pattern: 5 roots, 2 at +2oct, 1 at +3oct
    _autechre_pattern[0] = 0;   // root
    _autechre_pattern[1] = 0;   // root
    _autechre_pattern[2] = 24;  // +2 octaves
    _autechre_pattern[3] = 0;   // root
    _autechre_pattern[4] = 0;   // root
    _autechre_pattern[5] = 24;  // +2 octaves
    _autechre_pattern[6] = 0;   // root
    _autechre_pattern[7] = 36;  // +3 octaves
    // 2. Set rule timing based on Flow
    _autechre_rule_timer = 8 + (sequence.flow() * 4);

    // 3. Create a sequence of rules based on Ornament
    _rng = Random((sequence.ornament() - 1) << 4);
    for (int i = 0; i < 8; ++i) _autechre_rule_sequence[i] = _rng.next() % 5; // 5 different rules
    _autechre_rule_index = 0;
    break;
}

    case 5: // STEPWAVE - Scale stepping with chromatic trill (previously algorithm 20)
{
    _rng = Random((flow - 1) << 4);
    _extraRng = Random((ornament - 1) << 4);

    // Flow controls scale step direction: 0-7=down, 8=stationary, 9-16=up
    // Ornament controls trill direction AND step size:
    //   0-5: trill down, 2 steps
    //   6-10: trill random, 2-3 steps mixed
    //   11-16: trill up, 3 steps
    if (ornament <= 5) {
        _stepwave_direction = -1;  // Trill down
    } else if (ornament >= 11) {
        _stepwave_direction = 1;   // Trill up
    } else {
        _stepwave_direction = 0;   // Random trill direction each time
    }

    _stepwave_step_count = 3 + (_rng.next() % 5);  // 3-7 substeps for trill
    _stepwave_current_step = 0;
    _stepwave_chromatic_offset = 0;
    _stepwave_is_stepped = true;  // Default to stepped, glide probability determines slide
    break;
}

    default:
        // Fallback to TEST algorithm to handle any invalid cases
        _testMode = (flow - 1) >> 3;  // 0 or 1 (1-8 = mode 0, 9-16 = mode 1)
        _testSweepSpeed = ((flow - 1) & 0x3);  // 0-3
        _testAccent = (ornament - 1) >> 3;  // 0 or 1
        _testVelocity = ((ornament - 1) << 4);  // 0-240
        _testNote = 0;
        break;
    }
}

void TuesdayTrackEngine::reset() {
    _cachedAlgorithm = -1;
    _cachedFlow = -1;
    _cachedOrnament = -1;
    _cachedLoopLength = -1;

    _stepIndex = 0;
    _displayStep = -1;  // No step displayed until first tick
    _gateLength = 0;
    _gateTicks = 0;
    _coolDown = 0;
    _coolDownMax = 0;
    _gatePercent = 75;
    _gateOffset = 0;
    _slide = 0;
    _cvTarget = 0.f;
    _cvCurrent = 0.f;
    _cvDelta = 0.f;
    _slideCountDown = 0;

    // Gate timing state
    _gateLengthTicks = 0;
    _pendingGateOffsetTicks = 0;
    _pendingGateActivation = false;

    // Retrigger/Trill State
    _retriggerArmed = false;
    _retriggerCount = 0;
    _retriggerPeriod = 0;
    _retriggerLength = 0;
    _retriggerTimer = 0;
    _isTrillNote = false;
    _trillCvTarget = 0.f;

    _activity = false;
    _gateOutput = false;
    _cvOutput = 0.f;
    _lastGatedCv = 0.f;
    _bufferValid = false;

    // Re-initialize algorithm with current seeds
    initAlgorithm();
}

void TuesdayTrackEngine::restart() {
    reset();
}

void TuesdayTrackEngine::reseed() {
    // Reset step to beginning
    _stepIndex = 0;
    _coolDown = 0;

    // Generate new random seeds for a fresh pattern
    // Use current RNG state to generate new seeds, giving variety
    uint32_t newSeed1 = _rng.next();
    uint32_t newSeed2 = _extraRng.next();

    // Reinitialize RNGs with new random seeds
    _rng = Random(newSeed1);
    _extraRng = Random(newSeed2);

    // Reinitialize algorithm state with new RNG values
    const auto &sequence = tuesdayTrack().sequence(pattern());
    int algorithm = sequence.algorithm();

    switch (algorithm) {
    case 0: // TEST
        _testMode = (_rng.next() & 0x1);
        _testSweepSpeed = (_rng.next() & 0x3);
        _testAccent = (_rng.next() & 0x1);
        _testVelocity = (_rng.next() & 0xF0);
        _testNote = 0;
        break;

    case 1: // TRITRANCE
        _triB1 = (_rng.next() & 0x7);
        _triB2 = (_rng.next() & 0x7);
        _triB3 = (_extraRng.next() & 0x15);
        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
        _triB3 -= 4;
        break;

    case 2: // STOMPER
        _stomperMode = (_rng.next() % 7) * 2;
        _stomperCountDown = 0;
        _stomperLowNote = _rng.next() % 3;
        _stomperLastNote = _stomperLowNote;
        _stomperLastOctave = 0;
        _stomperHighNote[0] = _rng.next() % 7;
        _stomperHighNote[1] = _rng.next() % 5;
        break;

    case 18: // APHEX
    case 19: // AUTECH
    case 20: // STEPWAVE
        // These algorithms are fully deterministic from flow/ornament,
        // so reseed just re-initializes with the same parameters.
        initAlgorithm();
        break;



    default:
        break;
    }
}

void TuesdayTrackEngine::generateBuffer() {
    // Forced recompilation marker
    // Initialize algorithm fresh to get deterministic pattern
    // initAlgorithm(); // ALgo is not resseeded on capture

    const auto &_tuesdayTrackRef = tuesdayTrack();
    const auto &sequence = _tuesdayTrackRef.sequence(pattern());

    int algorithm = sequence.algorithm();
    int glide = sequence.glide();

    // Generate buffer directly from initial algorithm state
    // When default is infinite mode, algorithm evolves naturally during playback
    // Buffer captures current state when switching to finite loop

    // Generate 64 steps into buffer
    for (int step = 0; step < BUFFER_SIZE; step++) {
        int note = 0;
        int octave = 0;
        uint8_t gatePercent = 75;
        uint8_t slide = 0;
        uint8_t gateOffset = 0;  // Initialize default value for gateOffset
        bool isTrill = false;

        // Map algorithms to our reduced set (0, 1, 2, 3, 4, 5)
        // Original mappings: 0->0(TEST), 1->1(TRITRANCE), 2->2(STOMPER),
        //                    18->3(APHEX), 19->4(AUTECHRE), 20->5(STEPWAVE)
        // All others (3-17) are mapped to 0 (fallback to TEST)
        int mapped_algorithm = algorithm;
        if (algorithm >= 3 && algorithm <= 17) {
            // Map all removed algorithms to algorithm 0 (TEST) for fallback behavior
            mapped_algorithm = 0;
        } else if (algorithm == 18) {
            mapped_algorithm = 3;  // APHEX
        } else if (algorithm == 19) {
            mapped_algorithm = 4;  // AUTECHRE
        } else if (algorithm == 20) {
            mapped_algorithm = 5;  // STEPWAVE
        }

        switch (mapped_algorithm) {
        case 0: // TEST (fallback for all removed algorithms)
            {
                gatePercent = 75;
                if (_rng.nextRange(100) < glide) {
                    slide = _testSweepSpeed + 1;
                }

                switch (_testMode) {
                case 0:  // OCTSWEEPS
                    octave = (step % 5);
                    note = 0;
                    // TEST: Apply timing variations based on mode
                    gateOffset = (step % 4) * 10;  // Simple periodic timing variations
                    break;
                case 1:  // SCALEWALKER
                default:
                    octave = 0;
                    _testNote = (_testNote + 1) % 12;
                    note = _testNote;
                    break;
                }
            }
            break;

        case 1: // TRITRANCE
            {
                int gateLengthChoice = _rng.nextRange(100);
                if (gateLengthChoice < 40) {
                    gatePercent = 50 + (_rng.nextRange(4) * 12);  // 50%, 62%, 74%, 86%
                } else if (gateLengthChoice < 70) {
                    gatePercent = 100 + (_rng.nextRange(4) * 25); // 100%, 125%, 150%, 175%
                } else {
                    gatePercent = 200 + (_rng.nextRange(9) * 25); // 200% to 400%
                }

                {
                    int glideChance = _rng.nextRange(100);
                    int slideAmount = (_rng.nextRange(3)) + 1;
                    if (glideChance < glide) {
                        slide = slideAmount;
                    }
                }

                // TRITRANCE: Generate phase-based timing variations for swing feel
                int phase = (step + _triB2) % 3;
                switch (phase) {
                case 0:
                    // First phase: slightly early timing
                    gateOffset = 10 + (_rng.nextRange(15));  // 10-25% early
                    break;
                case 1:
                    // Second phase: on-time or slightly late
                    gateOffset = 45 + (_rng.nextRange(10));  // 45-55% timing
                    break;
                case 2:
                    // Third phase: swing delay
                    gateOffset = 60 + (_rng.nextRange(20));  // 60-80% delay
                    break;
                }
                switch (phase) {
                case 0:
                    if (_extraRng.nextBinary() && _extraRng.nextBinary()) {
                        _triB3 = (_extraRng.next() & 0x15);
                        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
                        _triB3 -= 4;
                    }
                    octave = 0;
                    note = _triB3 + 4;
                    break;
                case 1:
                    octave = 1;
                    note = _triB3 + 4;
                    if (_rng.nextBinary()) {
                        _triB2 = (_rng.next() & 0x7);
                    }
                    break;
                case 2:
                    octave = 2;
                    note = _triB1;
                    if (_rng.nextBinary()) {
                        _triB1 = ((_rng.next() >> 5) & 0x7);
                    }
                    break;
                }

                if (note < 0) note = 0;
                if (note > 11) note = 11;
            }
            break;

        case 2: // STOMPER - buffer generation
            {
                gatePercent = 75;
                slide = 0;

                if (_stomperCountDown > 0) {
                    gatePercent = _stomperCountDown * 25;
                    _stomperCountDown--;
                    // Still generate a note but mark as rest
                    note = _stomperLastNote;
                    octave = _stomperLastOctave;
                } else {
                    if (_stomperMode >= 14) {
                        _stomperMode = (_extraRng.next() % 7) * 2;
                    }

                    _rng.nextRange(100);
                    _stomperLowNote = _rng.next() % 3;
                    _rng.nextRange(100);
                    _stomperHighNote[0] = _rng.next() % 7;
                    _rng.nextRange(100);
                    _stomperHighNote[1] = _rng.next() % 5;

                    int maxticklen = 2;

                    switch (_stomperMode) {
                    case 10:
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        _stomperMode++;
                        break;
                    case 11:
                        octave = 0;
                        note = _stomperLowNote;
                        if (_rng.nextRange(100) < glide) {
                            slide = (_rng.nextRange(3)) + 1;
                        }
                        // Check for Trill on stomp down
                        {
                            int trillChanceAlgorithmic = 18;
                            int userTrillSetting = sequence.trill();
                            int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;
                            if (_uiRng.nextRange(100) < finalTrillChance) {
                                isTrill = true;
                            }
                        }
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 12:
                        octave = 0;
                        note = _stomperLowNote;
                        _stomperMode++;
                        break;
                    case 13:
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        if (_rng.nextRange(100) < glide) {
                            slide = (_rng.nextRange(3)) + 1;
                        }
                        // Check for Trill on stomp up
                        {
                            int trillChanceAlgorithmic = 18;
                            int userTrillSetting = sequence.trill();
                            int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;
                            if (_uiRng.nextRange(100) < finalTrillChance) {
                                isTrill = true;
                            }
                        }
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 4:
                    case 5:
                        octave = 0;
                        note = _stomperLowNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 0:
                    case 1:
                        octave = _stomperLastOctave;
                        note = _stomperLastNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 6:
                    case 7:
                        octave = 0;
                        note = _stomperLowNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 8:
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        _stomperMode++;
                        break;
                    case 9:
                        octave = 0;
                        note = _stomperLowNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    case 2:
                        octave = 0;
                        note = _stomperLowNote;
                        _stomperMode++;
                        break;
                    case 3:
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;
                        break;
                    default:
                        octave = _stomperLastOctave;
                        note = _stomperLastNote;
                        break;
                    }

                    _stomperLastNote = note;
                    _stomperLastOctave = octave;
                }

                if (note < 0) note = 0;
                if (note > 11) note = 11;

                // STOMPER: Apply timing variations that complement the stomping rhythm
                // Different timing based on the current mode
                switch (_stomperMode) {
                case 0: case 1: case 2: case 3:
                    gateOffset = 5;   // Early timing for foundational notes
                    break;
                case 4: case 5: case 6: case 7:
                    gateOffset = 25;  // Mid-range timing for build-up
                    break;
                case 8: case 9: case 10: case 11:
                    gateOffset = 75;  // Later timing for climax
                    break;
                case 12: case 13:
                    gateOffset = 15;  // Moderate timing for transitions
                    break;
                case 14:
                default:
                    gateOffset = 0;   // On-beat timing for steady sections
                    break;
                }
            }
            break;

            case 18: // APHEX buffer generation
            {
                // --- Main melody from Track 1 ---
                note = _aphex_track1_pattern[_aphex_pos1];
                octave = 0;
                gatePercent = 75;
                slide = 0;

                // --- Modification from Track 2 ---
                uint8_t modifier = _aphex_track2_pattern[_aphex_pos2];
                if (modifier == 1) { // Stutter
                    gatePercent = 20;
                } else if (modifier == 2) { // Slide
                    slide = 1;
                }

                // --- Override from Track 3 ---
                uint8_t bass_note = _aphex_track3_pattern[_aphex_pos3];
                if (bass_note > 0) {
                    note = bass_note;
                    octave = -1; // Deep bass
                    gatePercent = 90;
                    slide = 0;
                }

                // --- Advance all tracks ---
                _aphex_pos1 = (_aphex_pos1 + 1) % 4;
                _aphex_pos2 = (_aphex_pos2 + 1) % 3;
                _aphex_pos3 = (_aphex_pos3 + 1) % 5;

                // APHEX: Apply polyrhythmic timing variations
                // Use multi-track positioning to create complex timing variations
                int polyRhythmFactor = (_aphex_pos1 + _aphex_pos2 + _aphex_pos3) % 12;
                gateOffset = polyRhythmFactor * 8;  // Scale to 0-96% range

                // Check for Trill on polyrhythmic collision points (glitchy)
                int polyCollision = ((_aphex_pos1 * 7) ^ (_aphex_pos2 * 5) ^ (_aphex_pos3 * 3)) % 12;
                if (polyCollision > 8) {
                    int trillChanceAlgorithmic = 45; // High for glitch
                    int userTrillSetting = sequence.trill();
                    int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;
                    if (_uiRng.nextRange(100) < finalTrillChance) {
                        isTrill = true;
                    }
                }
            }
            break;

        case 19: // AUTECHRE buffer generation
            {
                // --- Always play the current state of the pattern ---
                // Pattern encodes note + octave*12, so extract both
                int patternVal = _autechre_pattern[step % 8];
                note = patternVal % 12;
                octave = patternVal / 12;
                gatePercent = 75;
                slide = 0;

                // --- Countdown to the next transformation ---
                _autechre_rule_timer--;
                if (_autechre_rule_timer <= 0) {
                    uint8_t current_rule = _autechre_rule_sequence[_autechre_rule_index];
                    int intensity = sequence.power() / 2; // Power: 0-8

                    // --- Apply a deterministic transformation rule ---
                    switch (current_rule) {
                        case 0: // ROTATE
                            {
                                int8_t temp = _autechre_pattern[7];
                                for (int i = 7; i > 0; --i) _autechre_pattern[i] = _autechre_pattern[i-1];
                                _autechre_pattern[0] = temp;
                            }
                            break;
                        case 1: // REVERSE
                            for (int i = 0; i < 4; ++i) {
                                int8_t temp = _autechre_pattern[i];
                                _autechre_pattern[i] = _autechre_pattern[7-i];
                                _autechre_pattern[7-i] = temp;
                            }
                            break;
                        case 2: // INVERT around a pivot - preserve octave
                            for (int i = 0; i < 8; ++i) {
                                int octave = _autechre_pattern[i] / 12;
                                int note = _autechre_pattern[i] % 12;
                                int interval = note - 6;
                                note = (6 - interval + 12) % 12;
                                _autechre_pattern[i] = note + (octave * 12);
                            }
                            break;
                        case 3: // SWAP adjacent notes
                            for (int i = 0; i < 8; i += 2) {
                                int8_t temp = _autechre_pattern[i];
                                _autechre_pattern[i] = _autechre_pattern[i+1];
                                _autechre_pattern[i+1] = temp;
                            }
                            break;
                        case 4: // ADD intensity - preserve octave
                            for (int i = 0; i < 8; ++i) {
                                int octave = _autechre_pattern[i] / 12;
                                int note = (_autechre_pattern[i] % 12 + intensity) % 12;
                                _autechre_pattern[i] = note + (octave * 12);
                            }
                            break;
                    }

                    // --- Reset for next rule ---
                    _autechre_rule_timer = 8 + (sequence.flow() * 4);
                    _autechre_rule_index = (_autechre_rule_index + 1) % 8;
                }

                // AUTECHRE: Apply algorithmic timing variations that complement the pattern transformations
                // Use the current rule index and timer to create evolving timing variations
                gateOffset = ((_autechre_rule_index * 10) + (_autechre_rule_timer % 7)) % 100;

                // Check for Trill on SWAP rule or fresh transformation (glitchy)
                uint8_t current_rule = _autechre_rule_sequence[_autechre_rule_index];
                if (current_rule == 3 || _autechre_rule_timer < 2) {
                    int trillChanceAlgorithmic = 50; // Very high for abstract chaos
                    int userTrillSetting = sequence.trill();
                    int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;
                    if (_uiRng.nextRange(100) < finalTrillChance) {
                        isTrill = true;
                    }
                }
            }
            break;

        case 20: // STEPWAVE buffer generation
            {
                // Flow controls scale step direction with octave jumps
                // 0-7: step down, 8: stationary, 9-16: step up
                // Also probabilistically jump octaves
                int flowVal = sequence.flow();
                int ornamentVal = sequence.ornament();

                // Determine scale step movement based on flow
                int scaleStepDir = 0;
                if (flowVal <= 7) {
                    scaleStepDir = -1;  // Step down
                } else if (flowVal >= 9) {
                    scaleStepDir = 1;   // Step up
                }
                // flowVal == 8 means stationary (scaleStepDir = 0)

                // Determine step size from ornament: 0-5=2 steps, 6-10=2-3 mixed, 11-16=3 steps
                int stepSize;
                if (ornamentVal <= 5) {
                    stepSize = 2;
                } else if (ornamentVal >= 11) {
                    stepSize = 3;
                } else {
                    stepSize = 2 + (_extraRng.next() % 2);  // 2 or 3
                }

                // Calculate note as scale degree offset from previous
                // Use step counter to accumulate movement
                note = (step * scaleStepDir * stepSize) % 7;  // Keep within single octave range for scale
                if (note < 0) note += 7;

                // Octave jumps: probabilistically jump up 2 octaves
                // Higher flow = more upward movement = more high octave jumps
                int octaveJumpChance = 20 + (flowVal * 3);  // 20-68%
                if (_rng.nextRange(100) < octaveJumpChance) {
                    octave = 2;
                } else {
                    octave = 0;
                }

                gatePercent = 85;  // Slightly longer gates for the stepping effect

                // Check for trill (base probability 50%)
                int trillChanceAlgorithmic = 50;
                int userTrillSetting = sequence.trill();
                int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                if (_uiRng.nextRange(100) < finalTrillChance) {
                    isTrill = true;

                    // Determine if this is stepped or slide (glide parameter)
                    if (_rng.nextRange(100) < glide) {
                        slide = 2;  // Long slide for smooth glissando
                    }

                    // Trill step count from ornament: lower=2, middle=2-3, higher=3
                    if (ornamentVal <= 5) {
                        _stepwave_step_count = 2;
                    } else if (ornamentVal >= 11) {
                        _stepwave_step_count = 3;
                    } else {
                        _stepwave_step_count = 2 + (_extraRng.next() % 2);
                    }
                }

                // Gate offset: create rhythmic interest
                gateOffset = (step % 4) * 15;  // 0, 15, 30, 45
            }
            break;

        default:
            note = 0;
            octave = 0;
            break;
        }

        _buffer[step].note = note;
        _buffer[step].octave = octave;
        _buffer[step].gatePercent = gatePercent;
        _buffer[step].slide = slide;
        _buffer[step].gateOffset = gateOffset;
        _buffer[step].isTrill = isTrill;
    }

    _bufferValid = true;

    // Reinitialize algorithm for live playback
    // initAlgorithm(); // don't initialize
}

TrackEngine::TickResult TuesdayTrackEngine::tick(uint32_t tick) {
    // Check mute
    if (mute()) {
        _gateOutput = false;
        _cvOutput = 0.f;
        _activity = false;
        return TickResult::NoUpdate;
    }

    const auto &_tuesdayTrackRef = tuesdayTrack();
    const auto &sequence = _tuesdayTrackRef.sequence(pattern());

    // Get parameters
    int power = sequence.power();
    int loopLength = sequence.actualLoopLength();
    int algorithm = sequence.algorithm();

    // Power = 0 means silent
    if (power == 0) {
        _gateOutput = false;
        _cvOutput = 0.f;
        _activity = false;
        return TickResult::NoUpdate;
    }

    // Check if parameters changed - if so, reinitialize and invalidate buffer
    // Note: Scan is NOT included here - it's a real-time playback parameter
    bool paramsChanged = (_cachedAlgorithm != sequence.algorithm() ||
                         _cachedFlow != sequence.flow() ||
                         _cachedOrnament != sequence.ornament() ||
                         _cachedLoopLength != sequence.loopLength());
    if (paramsChanged) {
        _cachedAlgorithm = sequence.algorithm();
        _cachedLoopLength = sequence.loopLength();
        initAlgorithm();
        _bufferValid = false;
    }

    // Generate buffer for finite loops if needed
    if (loopLength > 0 && !_bufferValid) {
        generateBuffer();
    }

    // Calculate base cooldown from power
    // Linear mapping: Power = number of notes per 16 steps
    // Use fixed reference length for consistent density regardless of loop length
    // int effectiveLength = 16;  // Fixed reference for density calculations
    // int baseCooldown = (power > 0) ? effectiveLength / power : effectiveLength;
    int baseCooldown = 17 - power;
    if (baseCooldown < 1) baseCooldown = 1;

    // Apply skew to cooldown based on loop position
    // Skew value determines what fraction of loop is at power 16:
    // Skew 8 = last 50% at power 16, Skew 4 = last 25% at power 16
    int skew = sequence.skew();
    if (skew != 0 && loopLength > 0) {
        // Calculate position in loop (0.0 to 1.0), clamped for safety
        float position = (float)_stepIndex / (float)loopLength;
        if (position > 1.0f) position = 1.0f;
        if (position < 0.0f) position = 0.0f;

        if (skew > 0) {
            // Build-up: last (skew/16) of loop at power 16
            // Skew 8 → switch at 0.5, Skew 4 → switch at 0.75
            float switchPoint = 1.0f - (float)skew / 16.0f;

            if (position < switchPoint) {
                // Before switch: use base power setting
                _coolDownMax = baseCooldown;
            } else {
                // After switch: full density (power 16)
                _coolDownMax = 1;
            }
        } else {
            // Fade-out: first (|skew|/16) of loop at power 16
            // Skew -8 → switch at 0.5, Skew -4 → switch at 0.25
            float switchPoint = (float)(-skew) / 16.0f;

            if (position < switchPoint) {
                // Before switch: full density (power 16)
                _coolDownMax = 1;
            } else {
                // After switch: base power setting
                _coolDownMax = baseCooldown;
            }
        }

        // Clamp to valid range
        if (_coolDownMax < 1) _coolDownMax = 1;
        if (_coolDownMax > 16) _coolDownMax = 16;
    } else {
        _coolDownMax = baseCooldown;
    }

    // Calculate step timing with clock sync
    // Use track divisor (converts from PPQN to actual ticks)
    uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);

    // Calculate reset divisor from resetMeasure parameter
    int resetMeasure = sequence.resetMeasure();
    uint32_t resetDivisor;

    if (loopLength > 0) {
        // Finite loop - calculate reset from loop duration
        // This ensures steps always align with loop boundaries
        uint32_t loopDuration = loopLength * divisor;

        if (resetMeasure > 0) {
            // If resetMeasure is set, round up to next complete loop cycle
            // to allow patterns to evolve over multiple loop cycles
            uint32_t measureReset = resetMeasure * _engine.measureDivisor();
            resetDivisor = ((measureReset + loopDuration - 1) / loopDuration) * loopDuration;
        } else {
            // No reset measure - just use loop duration
            resetDivisor = loopDuration;
        }
    } else {
        // Infinite loop - no reset, tick grows forever
        resetDivisor = 0;
    }

    uint32_t relativeTick = resetDivisor == 0 ? tick : tick % resetDivisor;

    // Reset on measure boundary (only if resetMeasure is enabled and not infinite loop)
    if (resetDivisor > 0 && relativeTick == 0) {
        reset();
    }

    // Check if we're at a step boundary
    bool stepTrigger = (relativeTick % divisor == 0);

    // Handle gate timing
    if (_gateTicks > 0) {
        _gateTicks--;
        if (_gateTicks == 0) {
            _gateOutput = false;
            _activity = false;
        }
    }

    // Handle slide/portamento
    if (_slideCountDown > 0) {
        _cvCurrent += _cvDelta;
        _slideCountDown--;
        if (_slideCountDown == 0) {
            _cvCurrent = _cvTarget;  // Ensure we hit target exactly
        }
        _cvOutput = _cvCurrent;
    }

    if (stepTrigger) {
        // Disarm any re-triggers from the previous step to ensure a clean state
        _retriggerArmed = false;
        _retriggerCount = 0;

        // Calculate step from tick count (ensures sync with divisor)
        uint32_t calculatedStep = relativeTick / divisor;

        // For finite loops, wrap within loop length
        if (loopLength > 0) {
            // Check if we wrapped to beginning of loop - reinitialize algorithm
            uint32_t newStep = calculatedStep % loopLength;
            if (newStep < _stepIndex && _stepIndex > 0) {
                // Loop wrapped - reinitialize algorithm for deterministic repeats
                initAlgorithm();
            }
            _stepIndex = newStep;
        } else {
            _stepIndex = calculatedStep;
        }

        // Set display step for UI sync
        _displayStep = _stepIndex;

        // Generate output based on algorithm
        bool shouldGate = false;
        float noteVoltage = 0.f;
        int note = 0;
        int octave = 0;

        // Calculate effective step index
        // Scan: offsets where on the infinite tape we capture from (0-127)
        // Rotate: for finite loops, shifts start point within the captured loop
        int scan = sequence.scan();
        uint32_t effectiveStep = _stepIndex;
        if (loopLength > 0) {
            // Finite loop: rotate shifts within loop, then scan offsets into buffer
            int rot = sequence.rotate();
            // Handle negative rotation with proper modulo
            effectiveStep = ((_stepIndex + rot) % loopLength + loopLength) % loopLength;
            // Add scan offset
            effectiveStep += scan;
            // Wrap to buffer size (64 steps) - longer loops cycle through the pattern
            int bufferIndex = effectiveStep % BUFFER_SIZE;

            // Read from pre-generated buffer
            {
                const auto &bufferedStep = _buffer[bufferIndex];
                note = bufferedStep.note;
                octave = bufferedStep.octave;
                _gatePercent = bufferedStep.gatePercent;
                _slide = bufferedStep.slide;

                // Check for trill/re-trigger
                if (bufferedStep.isTrill) {
                    _retriggerArmed = true;
                    _retriggerCount = 2; // 3 notes total
                    _retriggerPeriod = divisor / 3; // 8th note triplets
                    _retriggerLength = _retriggerPeriod / 2;
                    _isTrillNote = false;

                    float baseVoltage = (note + (octave * 12)) / 12.f;
                    _trillCvTarget = baseVoltage + (2.f / 12.f);
                }

                // Retrieve gate offset from buffered data
                uint8_t bufferedGateOffset = bufferedStep.gateOffset;
                uint8_t globalGateOffset = sequence.gateOffset();
                uint8_t finalOffset = 0;

                if (globalGateOffset == 0) {
                    // Mode 1: Force On-Beat
                    finalOffset = 0;
                } else if (globalGateOffset == 100) {
                    // Mode 3: Pure Random Offset
                    finalOffset = _uiRng.nextRange(101); // 0-100
                } else {
                    // Mode 2: Probabilistic blend of Algorithmic and Random
                    finalOffset = bufferedGateOffset; // Base is the algorithmic groove
                    // with a chance of being overridden by a random value
                    if (_uiRng.nextRange(100) < globalGateOffset) {
                        finalOffset = _uiRng.nextRange(101); // 0-100
                    }
                }
                _gateOffset = finalOffset;

                shouldGate = true;
                // Calculate noteVoltage from buffered data for CV/glide
                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
        } else {
            // Infinite loop: live generation with scan offset
            effectiveStep = _stepIndex + scan;

            // Map algorithms to our reduced set (0, 1, 2, 3, 4, 5)
            // Original mappings: 0->0(TEST), 1->1(TRITRANCE), 2->2(STOMPER),
            //                    18->3(APHEX), 19->4(AUTECHRE), 20->5(STEPWAVE)
            // All others (3-17) are mapped to 0 (fallback to TEST)
            int mapped_algorithm_tick = algorithm;
            if (algorithm >= 3 && algorithm <= 17) {
                // Map all removed algorithms to algorithm 0 (TEST) for fallback behavior
                mapped_algorithm_tick = 0;
            } else if (algorithm == 18) {
                mapped_algorithm_tick = 3;  // APHEX
            } else if (algorithm == 19) {
                mapped_algorithm_tick = 4;  // AUTECHRE
            } else if (algorithm == 20) {
                mapped_algorithm_tick = 5;  // STEPWAVE
            }

            switch (mapped_algorithm_tick) {
        case 0: // TEST - Test patterns (fallback for all removed algorithms)
            // Flow: Mode (OCTSWEEPS or SCALEWALKER) + SweepSpeed
            // Ornament: Accent + Velocity
            {
                shouldGate = true;  // Always gate in test mode
                _gatePercent = 75;  // Default gate length

                // Slide controlled by glide parameter
                int glide = sequence.glide();
                if (_rng.nextRange(100) < glide) {
                    _slide = _testSweepSpeed + 1;  // Use sweep speed as slide amount
                } else {
                    _slide = 0;
                }

                switch (_testMode) {
                case 0:  // OCTSWEEPS - sweep through octaves
                    octave = (effectiveStep % 5);  // 5 octaves
                    note = 0;
                    break;
                case 1:  // SCALEWALKER - walk through notes
                default:
                    octave = 0;
                    note = _testNote;
                    _testNote = (_testNote + 1) % 12;
                    break;
                }

                // CV: 1V/octave standard
                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        case 1: // TRITRANCE - German minimal style arpeggios
            // Flow: Seeds RNG for b1 (high note), b2 (phase offset)
            // Ornament: Seeds RNG for b3 (note offset)
            {
                shouldGate = true;

                // Random gate length - min 50%, up to 400%
                int gateLengthChoice = _rng.nextRange(100);
                if (gateLengthChoice < 40) {
                    _gatePercent = 50 + (_rng.nextRange(4) * 12);  // 50%, 62%, 74%, 86%
                } else if (gateLengthChoice < 70) {
                    _gatePercent = 100 + (_rng.nextRange(4) * 25); // 100%, 125%, 150%, 175%
                } else {
                    _gatePercent = 200 + (_rng.nextRange(9) * 25); // 200% to 400%
                }

                // Random slide controlled by glide parameter
                int glide = sequence.glide();
                if (_rng.nextRange(100) < glide) {
                    _slide = (_rng.nextRange(3)) + 1;  // 1-3
                } else {
                    _slide = 0;
                }

                // Tritrance pattern based on step position mod 3
                int phase = (effectiveStep + _triB2) % 3;
                switch (phase) {
                case 0:
                    // Maybe change b3
                    if (_extraRng.nextBinary() && _extraRng.nextBinary()) {
                        _triB3 = (_extraRng.next() & 0x15);
                        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
                        _triB3 -= 4;
                    }
                    octave = 0;
                    note = _triB3 + 4;  // Center around note 4
                    break;
                case 1:
                    octave = 1;
                    note = _triB3 + 4;
                    // Maybe change b2
                    if (_rng.nextBinary()) {
                        _triB2 = (_rng.next() & 0x7);
                    }
                    break;
                case 2:
                    octave = 2;
                    note = _triB1;
                    // Maybe change b1
                    if (_rng.nextBinary()) {
                        _triB1 = ((_rng.next() >> 5) & 0x7);
                    }
                    break;
                }

                // Clamp note to valid range
                if (note < 0) note = 0;
                if (note > 11) note = 11;

                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        case 2: // STOMPER - Acid bass patterns with slides
            // Flow: Seeds RNG for pattern modes
            // Ornament: Seeds RNG for note choices
            {
                int accentoffs = 0;
                _gatePercent = 75;  // Default
                _slide = 0;  // Default no slide

                if (_stomperCountDown > 0) {
                    // Rest/note-off period - use countdown for gate length
                    shouldGate = false;
                    _gatePercent = _stomperCountDown * 25;  // Shorter gates during countdown
                    _stomperCountDown--;
                } else {
                    shouldGate = true;

                    // Generate new mode if needed
                    if (_stomperMode >= 14) {  // STOMPER_MAKENEW
                        _stomperMode = (_extraRng.next() % 7) * 2;
                    }

                    // Maybe change notes
                    if (_rng.nextRange(100) < 100) {
                        _stomperLowNote = _rng.next() % 3;
                    }
                    if (_rng.nextRange(100) < 100) {
                        _stomperHighNote[0] = _rng.next() % 7;
                    }
                    if (_rng.nextRange(100) < 100) {
                        _stomperHighNote[1] = _rng.next() % 5;
                    }

                    int maxticklen = 2;

                    // Pattern state machine
                    switch (_stomperMode) {
                    case 10:  // SLIDEDOWN1
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        _stomperMode++;
                        break;
                    case 11:  // SLIDEDOWN2
                        octave = 0;
                        note = _stomperLowNote;
                        // Slide controlled by glide parameter
                        if (sequence.glide() > 0) {
                            _slide = (_rng.nextRange(3)) + 1;  // Slide 1-3
                        } else {
                            _slide = 0;
                        }
                        // Check for Trill on stomp down
                        {
                            int trillChanceAlgorithmic = 18; // 18% base chance
                            int userTrillSetting = sequence.trill();
                            int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                            if (_uiRng.nextRange(100) < finalTrillChance) {
                                _retriggerArmed = true;
                                _retriggerCount = 1; // 2 notes total
                                _retriggerPeriod = divisor / 2; // 8th notes
                                _retriggerLength = _retriggerPeriod / 2;
                                _isTrillNote = false;

                                float baseVoltage = (note + (octave * 12)) / 12.f;
                                _trillCvTarget = baseVoltage + (12.f / 12.f); // Octave up (match high)
                            }
                        }
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 12:  // SLIDEUP1
                        octave = 0;
                        note = _stomperLowNote;
                        _stomperMode++;
                        break;
                    case 13:  // SLIDEUP2
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        // Slide controlled by glide parameter
                        if (sequence.glide() > 0) {
                            _slide = (_rng.nextRange(3)) + 1;  // Slide 1-3
                        } else {
                            _slide = 0;
                        }
                        // Check for Trill on stomp up
                        {
                            int trillChanceAlgorithmic = 18; // 18% base chance
                            int userTrillSetting = sequence.trill();
                            int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                            if (_uiRng.nextRange(100) < finalTrillChance) {
                                _retriggerArmed = true;
                                _retriggerCount = 1; // 2 notes total
                                _retriggerPeriod = divisor / 2; // 8th notes
                                _retriggerLength = _retriggerPeriod / 2;
                                _isTrillNote = false;

                                float baseVoltage = (note + (octave * 12)) / 12.f;
                                _trillCvTarget = baseVoltage - (12.f / 12.f); // Octave down (match low)
                            }
                        }
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 4:  // LOW1
                        accentoffs = 100;
                        // fall through
                    case 5:  // LOW2
                        octave = 0;
                        note = _stomperLowNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 0:  // PAUSE1
                    case 1:  // PAUSE2
                        octave = _stomperLastOctave;
                        note = _stomperLastNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 6:  // HIGH1
                        accentoffs = 100;
                        // fall through
                    case 7:  // HIGH2
                        octave = 0;
                        note = _stomperLowNote;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 8:  // HILOW1
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        _stomperMode++;
                        break;
                    case 9:  // HILOW2
                        octave = 0;
                        note = _stomperLowNote;
                        accentoffs = 100;
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    case 2:  // LOWHI1
                        octave = 0;
                        note = _stomperLowNote;
                        accentoffs = 100;
                        _stomperMode++;
                        break;
                    case 3:  // LOWHI2
                        octave = 1;
                        note = _stomperHighNote[_rng.next() % 2];
                        if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                        _stomperMode = 14;  // MAKENEW
                        break;
                    default:
                        octave = _stomperLastOctave;
                        note = _stomperLastNote;
                        break;
                    }

                    _stomperLastNote = note;
                    _stomperLastOctave = octave;
                }

                // Clamp
                if (note < 0) note = 0;
                if (note > 11) note = 11;

                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        case 18: // APHEX - complex polyrhythmic patterns (infinite loop)
            {
                shouldGate = true;
                // --- Main melody from Track 1 ---
                note = _aphex_track1_pattern[_aphex_pos1];
                octave = 0;
                _gatePercent = 75;
                _slide = 0;

                // --- Modification from Track 2 ---
                uint8_t modifier = _aphex_track2_pattern[_aphex_pos2];
                if (modifier == 1) { // Stutter
                    _gatePercent = 20;
                } else if (modifier == 2) { // Slide
                    _slide = 1;
                }

                // --- Override from Track 3 ---
                uint8_t bass_note = _aphex_track3_pattern[_aphex_pos3];
                if (bass_note > 0) {
                    note = bass_note;
                    octave = -1; // Deep bass
                    _gatePercent = 90;
                    _slide = 0;
                }

                // --- Advance all tracks ---
                _aphex_pos1 = (_aphex_pos1 + 1) % 4;
                _aphex_pos2 = (_aphex_pos2 + 1) % 3;
                _aphex_pos3 = (_aphex_pos3 + 1) % 5;

                // Check for Trill on polyrhythmic collision points (glitchy)
                int polyCollision = ((_aphex_pos1 * 7) ^ (_aphex_pos2 * 5) ^ (_aphex_pos3 * 3)) % 12;
                if (polyCollision > 8) {
                    int trillChanceAlgorithmic = 45; // High for glitch
                    int userTrillSetting = sequence.trill();
                    int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                    if (_uiRng.nextRange(100) < finalTrillChance) {
                        _retriggerArmed = true;
                        _retriggerCount = 3 + (_uiRng.nextRange(3)); // 4-6 notes (extreme glitch)
                        _retriggerPeriod = divisor / 5; // 32nd notes - rapid fire
                        _retriggerLength = _retriggerPeriod / 3; // Short staccato
                        _isTrillNote = false;

                        float baseVoltage = (note + (octave * 12)) / 12.f;
                        // Dissonant intervals: tritone (+6) or minor 2nd (+1)
                        float interval = ((_aphex_pos1 + _aphex_pos3) % 2) ? 6.f : 1.f;
                        _trillCvTarget = baseVoltage + (interval / 12.f);
                    }
                }

                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        case 19: // AUTECHRE - constantly evolving abstract patterns (infinite loop)
            {
                shouldGate = true;
                // --- Always play the current state of the pattern ---
                // Pattern encodes note + octave*12, so extract both
                int patternVal = _autechre_pattern[effectiveStep % 8];
                note = patternVal % 12;
                octave = patternVal / 12;
                _gatePercent = 75;
                _slide = 0;

                // --- Countdown to the next transformation ---
                _autechre_rule_timer--;
                if (_autechre_rule_timer <= 0) {
                    uint8_t current_rule = _autechre_rule_sequence[_autechre_rule_index];
                    int intensity = sequence.power() / 2; // Power: 0-8

                    // --- Apply a deterministic transformation rule ---
                    switch (current_rule) {
                        case 0: // ROTATE
                            {
                                int8_t temp = _autechre_pattern[7];
                                for (int i = 7; i > 0; --i) _autechre_pattern[i] = _autechre_pattern[i-1];
                                _autechre_pattern[0] = temp;
                            }
                            break;
                        case 1: // REVERSE
                            for (int i = 0; i < 4; ++i) {
                                int8_t temp = _autechre_pattern[i];
                                _autechre_pattern[i] = _autechre_pattern[7-i];
                                _autechre_pattern[7-i] = temp;
                            }
                            break;
                        case 2: // INVERT around a pivot - preserve octave
                            for (int i = 0; i < 8; ++i) {
                                int octave = _autechre_pattern[i] / 12;
                                int note = _autechre_pattern[i] % 12;
                                int interval = note - 6;
                                note = (6 - interval + 12) % 12;
                                _autechre_pattern[i] = note + (octave * 12);
                            }
                            break;
                        case 3: // SWAP adjacent notes
                            for (int i = 0; i < 8; i += 2) {
                                int8_t temp = _autechre_pattern[i];
                                _autechre_pattern[i] = _autechre_pattern[i+1];
                                _autechre_pattern[i+1] = temp;
                            }
                            break;
                        case 4: // ADD intensity - preserve octave
                            for (int i = 0; i < 8; ++i) {
                                int octave = _autechre_pattern[i] / 12;
                                int note = (_autechre_pattern[i] % 12 + intensity) % 12;
                                _autechre_pattern[i] = note + (octave * 12);
                            }
                            break;
                    }

                    // --- Reset for next rule ---
                    _autechre_rule_timer = 8 + (sequence.flow() * 4);
                    _autechre_rule_index = (_autechre_rule_index + 1) % 8;
                }

                // Check for Trill on SWAP rule or fresh transformation (glitchy)
                uint8_t current_rule = _autechre_rule_sequence[_autechre_rule_index];
                if (current_rule == 3 || _autechre_rule_timer < 2) {
                    int trillChanceAlgorithmic = 50; // Very high for abstract chaos
                    int userTrillSetting = sequence.trill();
                    int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                    if (_uiRng.nextRange(100) < finalTrillChance) {
                        _retriggerArmed = true;
                        // More trills on SWAP rule (most chaotic)
                        int minCount = (current_rule == 3) ? 4 : 2;
                        _retriggerCount = minCount + (_uiRng.nextRange(3)); // 4-7 for SWAP, 2-5 others
                        _retriggerPeriod = divisor / 4; // 16th notes
                        _retriggerLength = _retriggerPeriod / 2;
                        _isTrillNote = false;

                        float baseVoltage = (note + (octave * 12)) / 12.f;
                        // Rule-based intervals for deterministic glitch
                        int abstractInterval = (_autechre_rule_index * 3) % 8; // 0,3,6,1,4,7,2,5 semitones
                        _trillCvTarget = baseVoltage + (abstractInterval / 12.f);
                    }
                }

                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        case 20: // STEPWAVE - Scale stepping with trill (infinite loop)
            {
                shouldGate = true;

                // Flow controls scale step direction with octave jumps
                int flowVal = sequence.flow();
                int ornamentVal = sequence.ornament();

                // Determine scale step movement based on flow
                int scaleStepDir = 0;
                if (flowVal <= 7) {
                    scaleStepDir = -1;  // Step down
                } else if (flowVal >= 9) {
                    scaleStepDir = 1;   // Step up
                }

                // Determine step size from ornament: 0-5=2 steps, 6-10=2-3 mixed, 11-16=3 steps
                int stepSize;
                if (ornamentVal <= 5) {
                    stepSize = 2;
                } else if (ornamentVal >= 11) {
                    stepSize = 3;
                } else {
                    stepSize = 2 + (_extraRng.next() % 2);  // 2 or 3
                }

                // Calculate note as scale degree offset
                note = (effectiveStep * scaleStepDir * stepSize) % 7;
                if (note < 0) note += 7;

                // Octave jumps: probabilistically jump up 2 octaves
                int octaveJumpChance = 20 + (flowVal * 3);  // 20-68%
                if (_rng.nextRange(100) < octaveJumpChance) {
                    octave = 2;
                } else {
                    octave = 0;
                }

                _gatePercent = 85;
                _slide = 0;
                _gateOffset = (effectiveStep % 4) * 15;  // Rhythmic timing

                // Check for stepped trill (base probability 50%)
                int trillChanceAlgorithmic = 50;
                int userTrillSetting = sequence.trill();
                int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

                if (_uiRng.nextRange(100) < finalTrillChance) {
                    // Trill step count from ornament: lower=2, middle=2-3, higher=3
                    if (ornamentVal <= 5) {
                        _stepwave_step_count = 2;
                    } else if (ornamentVal >= 11) {
                        _stepwave_step_count = 3;
                    } else {
                        _stepwave_step_count = 2 + (_extraRng.next() % 2);
                    }
                    _stepwave_current_step = 0;
                    _stepwave_chromatic_offset = 0;

                    // Determine trill direction from ornament
                    int direction;
                    if (ornamentVal <= 5) {
                        direction = -1;  // Down
                    } else if (ornamentVal >= 11) {
                        direction = 1;   // Up
                    } else {
                        // Random direction
                        direction = _uiRng.nextBinary() ? 1 : -1;
                    }
                    _stepwave_direction = direction;

                    // Determine if this is stepped or slide
                    int glideParam = sequence.glide();
                    if (glideParam > 0 && _rng.nextRange(100) < glideParam) {
                        _stepwave_is_stepped = false;  // Slide mode
                        _slide = 2;  // Long slide for glissando
                    } else {
                        _stepwave_is_stepped = true;  // Stepped mode
                    }

                    // Arm the retrigger system
                    _retriggerArmed = true;
                    _retriggerCount = _stepwave_step_count - 1;  // -1 because first note plays immediately
                    _retriggerPeriod = divisor / _stepwave_step_count;  // Spread evenly
                    _retriggerLength = _retriggerPeriod / 2;
                    _isTrillNote = false;

                    // Calculate the final target for the trill (used for slide mode)
                    float baseVoltage = (note + (octave * 12)) / 12.f;
                    int totalSteps = (_stepwave_step_count - 1) * direction;
                    _trillCvTarget = baseVoltage + (totalSteps / 12.f);
                }

                noteVoltage = (note + (octave * 12)) / 12.0f;
            }
            break;

        default:
            shouldGate = false;
            noteVoltage = 0.f;
            break;
            }
        } // End else (infinite loop)

        // Apply global GateOffset override
        uint8_t globalGateOffset = sequence.gateOffset();
        if (globalGateOffset == 0) {
            // Mode 1: Force On-Beat
            _gateOffset = 0;
        } else if (globalGateOffset == 100) {
            // Mode 3: Pure Random Offset
            _gateOffset = _uiRng.nextRange(101); // 0-100
        } else {
            // Mode 2: Probabilistic blend of Algorithmic and Random
            // Note: _gateOffset already holds the algorithmic value here.
            // We just check for a random override.
            if (_uiRng.nextRange(100) < globalGateOffset) {
                _gateOffset = _uiRng.nextRange(101); // 0-100
            }
        }

        // Apply octave and transpose from sequence parameters
        int trackOctave = sequence.octave();
        int trackTranspose = sequence.transpose();

        // Get scale and root note (use track settings if not Default, otherwise project)
        int trackScaleIdx = sequence.scale();
        int trackRootNote = sequence.rootNote();

        const Scale &scale = (trackScaleIdx >= 0) ? Scale::get(trackScaleIdx) : _model.project().selectedScale();
        int rootNote = (trackRootNote >= 0) ? trackRootNote : _model.project().rootNote();

        // Apply scale quantization if useScale is enabled, track has specific scale, or project has non-chromatic scale
        if (sequence.useScale() || trackScaleIdx >= 0 || _model.project().scale() > 0) {
            // Treat note as scale degree, convert to voltage
            int scaleNote = note + octave * scale.notesPerOctave();
            // Add transpose (in semitones for chromatic, scale degrees otherwise)
            scaleNote += trackTranspose;
            noteVoltage = scale.noteToVolts(scaleNote) + (scale.isChromatic() ? rootNote : 0) * (1.f / 12.f);
        } else {
            // Free (chromatic) mode - apply transpose directly
            noteVoltage = (note + trackTranspose + (octave * 12)) / 12.0f;
        }

        // Apply octave offset (1V per octave)
        noteVoltage += trackOctave;

        // Decrement cooldown
        if (_coolDown > 0) {
            _coolDown--;
            if (_coolDown > _coolDownMax) _coolDown = _coolDownMax;
        }

        // Apply gate using cooldown system
        // Note triggers only when cooldown has expired
        // Velocity (from algorithm) must beat current cooldown value
        bool gateTriggered = shouldGate && _coolDown == 0;
        if (gateTriggered) {
            // Instead of immediately firing the gate, calculate the offset delay
            uint32_t actualGateOffset = (divisor * _gateOffset) / 100;

            if (actualGateOffset > 0) {
                // Create a delayed gate activation
                _pendingGateOffsetTicks = static_cast<uint8_t>(std::min(actualGateOffset, static_cast<uint32_t>(255))); // Cap at 255
                _pendingGateActivation = true;

                // Don't set the gate yet - it will be set after the offset delay
                // But we still need to reset cooldown and other states
                _activity = true;
                _coolDown = _coolDownMax;
            } else {
                // No offset - set gate immediately as before
                _gateOutput = true;
                // Gate length: use algorithm-determined percentage
                _gateTicks = (divisor * _gatePercent) / 100;
                if (_gateTicks < 1) _gateTicks = 1;  // Minimum 1 tick
                _activity = true;
                // Reset cooldown after triggering
                _coolDown = _coolDownMax;
            }
        }

        // Process pending gate offset if any
        if (_pendingGateActivation && _pendingGateOffsetTicks > 0) {
            _pendingGateOffsetTicks--;
            if (_pendingGateOffsetTicks == 0) {
                // Time to fire the gate after offset delay
                _gateOutput = true;
                // Gate length: use algorithm-determined percentage
                _gateTicks = (divisor * _gatePercent) / 100;
                if (_gateTicks < 1) _gateTicks = 1;  // Minimum 1 tick
                _pendingGateActivation = false;
            }
        }

        // Apply CV with slide/portamento based on cvUpdateMode
        // Free mode: CV updates every step (continuous evolution)
        // Gated mode: CV only updates when gate fires (original Tuesday behavior)
        bool shouldUpdateCv = (sequence.cvUpdateMode() == TuesdaySequence::Free) || gateTriggered;

        if (shouldUpdateCv) {
            _cvTarget = noteVoltage;
            if (_slide > 0) {
                // Calculate slide time: slide * 12 ticks (scaled for our timing)
                int slideTicks = _slide * 12;
                _cvDelta = (_cvTarget - _cvCurrent) / slideTicks;
                _slideCountDown = slideTicks;
            } else {
                // Instant change
                _cvCurrent = _cvTarget;
                _cvOutput = _cvTarget;
                _slideCountDown = 0;
            }
            // Store this as last gated CV for maintaining output in Gated mode
            _lastGatedCv = noteVoltage;
        } else {
            // Gated mode and no gate - maintain last CV value
            // Ensure slide continues if in progress, otherwise keep static
            if (_slideCountDown == 0) {
                _cvOutput = _lastGatedCv;
            }
        }

        // Step advancement and loop handling now done at start of stepTrigger block
        // via tick-based calculation

        // Setup gate timing counters
        if (gateTriggered) {
            _pendingGateActivation = true;
            _pendingGateOffsetTicks = (CONFIG_SEQUENCE_PPQN * _gateOffset) / 100;

            // If a re-trigger was armed by the algorithm, use the short re-trigger
            // length for the gate. Otherwise, use the main gate length.
            if (_retriggerArmed) {
                _gateLengthTicks = _retriggerLength;
            } else {
                _gateLengthTicks = (CONFIG_SEQUENCE_PPQN * _gatePercent) / 100;
            }
        }

        return TickResult::CvUpdate | TickResult::GateUpdate;
    }

    return TickResult::NoUpdate;
}

void TuesdayTrackEngine::update(float dt) {
    const auto &_tuesdayTrackRef = tuesdayTrack();
    const auto &sequence = _tuesdayTrackRef.sequence(pattern());
    // --- CV Sliding ---
    // This happens independently of gate logic
    if (_slideCountDown > 0) {
        _slideCountDown--;
        _cvCurrent += _cvDelta;
    } else {
        _cvCurrent = _cvTarget;
    }
    // Only update final CV output if not in a trill, which has its own CV logic
    if (!_retriggerArmed) {
        _cvOutput = _cvCurrent;
    }


    // --- Gate Timing State Machine ---

    // 1. Handle initial gate offset delay
    if (_pendingGateOffsetTicks > 0) {
        _pendingGateOffsetTicks--;
        return; // Still waiting for the first event to start
    }

    // 2. Activate the very first gate event if it was pending
    if (_pendingGateActivation) {
        _gateOutput = true;
        _pendingGateActivation = false;
        // _gateLengthTicks has already been set correctly in tick()
        // for either a normal note or the first note of a trill.

        // Set the initial CV for the first note
        _cvOutput = _cvTarget;
        return; // Gate is now on, wait for next update
    }

    // 3. Main state machine for gates that are already active or re-triggering
    if (_gateOutput) {
        // --- Gate is currently ON ---
        if (_gateLengthTicks > 0) {
            _gateLengthTicks--;
        } else {
            // ON-time has expired, so turn the gate OFF
            _gateOutput = false;

            // If we are in a re-trigger chain, set the timer for the silent OFF-time
            if (_retriggerArmed && _retriggerCount > 0) {
                _gateLengthTicks = _retriggerPeriod - _retriggerLength; // This now becomes the off-time
            }
        }
    } else {
        // --- Gate is currently OFF ---
        if (_retriggerArmed) {
            if (_retriggerCount > 0) {
                if (_gateLengthTicks > 0) {
                    _gateLengthTicks--;
                } else {
                    // OFF-time has expired, so fire the next re-triggered note
                    _retriggerCount--;
                    _gateOutput = true;
                    _gateLengthTicks = _retriggerLength; // Set ON-time for this new note

                    // Check if this is a STEPWAVE stepped trill
                    int algorithm = sequence.algorithm();
                    if (algorithm == 20 && _stepwave_is_stepped) {
                        // STEPWAVE: Increment through scale degrees
                        _stepwave_current_step++;
                        _stepwave_chromatic_offset += _stepwave_direction;

                        // Get scale info to convert scale degrees to voltage
                        int trackScaleIdx = sequence.scale();
                        int trackRootNote = sequence.rootNote();
                        const Scale &scale = (trackScaleIdx >= 0) ? Scale::get(trackScaleIdx) : _model.project().selectedScale();
                        int rootNote = (trackRootNote >= 0) ? trackRootNote : _model.project().rootNote();

                        // Calculate new CV using scale degrees (respects project/track scale)
                        if (sequence.useScale() || trackScaleIdx >= 0 || _model.project().scale() > 0) {
                            // Scale mode: treat offset as scale degree offset
                            // _cvTarget is the base note in voltage, we need to find its scale degree
                            // and add the offset, then convert back to voltage
                            int baseScaleDegree = 0;  // Root note starts at degree 0
                            int targetDegree = baseScaleDegree + _stepwave_chromatic_offset;
                            _cvOutput = scale.noteToVolts(targetDegree) + (scale.isChromatic() ? rootNote : 0) * (1.f / 12.f);
                            _cvOutput += sequence.octave();  // Apply track octave
                        } else {
                            // Chromatic/free mode: offset in semitones
                            _cvOutput = _cvTarget + (_stepwave_chromatic_offset / 12.f);
                        }
                    } else {
                        // Standard alternating trill for other algorithms
                        _isTrillNote = !_isTrillNote;
                        if (_isTrillNote) {
                            _cvOutput = _trillCvTarget;
                        } else {
                            _cvOutput = _cvTarget;
                        }
                    }
                }
            } else {
                // Last re-trigger has finished, disarm the system for the next step
                _retriggerArmed = false;
            }
        }
    }

    _activity = _gateOutput || _retriggerArmed;
}
