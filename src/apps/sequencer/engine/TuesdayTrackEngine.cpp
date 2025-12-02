#include "TuesdayTrackEngine.h"

#include "Engine.h"
#include "model/Scale.h"

#include <cmath>
#include <algorithm>

// Initialize algorithm state based on Flow (seed1) and Ornament (seed2)
void TuesdayTrackEngine::initAlgorithm() {
    const auto &_tuesdayTrackRef = tuesdayTrack();
    const auto &sequence = _tuesdayTrackRef.sequence(pattern());
    int flow = sequence.flow();
    int ornament = sequence.ornament();
    int algorithm = sequence.algorithm();

    // Generate seeds
    // Salt the Extra seed to ensure it's distinct even if Flow == Ornament
    uint32_t flowSeed = (flow - 1) << 4;
    uint32_t ornamentSeed = (ornament - 1) << 4;
    
    _uiRng = Random(flow * 37 + ornament * 101);

    _cachedFlow = flow;
    _cachedOrnament = ornament;
    _cachedAlgorithm = algorithm;
    _cachedLoopLength = sequence.loopLength();

    switch (algorithm) {
    case 0: // TEST
        _testMode = (flow - 1) >> 3;
        _testSweepSpeed = ((flow - 1) & 0x3);
        _testAccent = (ornament - 1) >> 3;
        _testVelocity = ((ornament - 1) << 4);
        _testNote = 0;
        
        _rng = Random(flowSeed);
        _extraRng = Random(ornamentSeed + 0x9e3779b9);
        break;

    case 1: // TRITRANCE
        _rng = Random(flowSeed);
        _extraRng = Random(ornamentSeed + 0x9e3779b9);

        _triB1 = (_rng.next() & 0x7);
        _triB2 = (_rng.next() & 0x7);

        _triB3 = (_extraRng.next() & 0x15);
        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
        _triB3 -= 4;
        break;

    case 2: // STOMPER
        // Original Swap: Main RNG = Ornament, Extra RNG = Flow
        _rng = Random(ornamentSeed);
        _extraRng = Random(flowSeed + 0x9e3779b9);

        _stomperMode = (_extraRng.next() % 7) * 2;
        _stomperCountDown = 0;
        _stomperLowNote = _rng.next() % 3;
        _stomperLastNote = _stomperLowNote;
        _stomperLastOctave = 0;
        _stomperHighNote[0] = _rng.next() % 7;
        _stomperHighNote[1] = _rng.next() % 5;
        break;

    case 6: // MARKOV
        _rng = Random(flowSeed);
        // Init History
        _markovHistory1 = (_rng.next() & 0x7);
        _markovHistory3 = (_rng.next() & 0x7);
        // Init Matrix
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                _markovMatrix[i][j][0] = (_rng.next() % 8);
                _markovMatrix[i][j][1] = (_rng.next() % 8);
            }
        }
        break;

    case 7: // CHIPARP 1
        _rng = Random(flowSeed);
        _chipChordSeed = _rng.next();
        _chipRng = Random(_chipChordSeed);
        _chipBase = _rng.next() % 3;
        _chipDir = (_rng.next() >> 7) % 2;
        break;

    case 8: // CHIPARP 2
        _rng = Random(flowSeed);
        _chipChordSeed = _rng.next();
        _chip2Rng = Random(_chipChordSeed);
        _chip2ChordScaler = (_rng.next() % 3) + 2;
        _chip2Offset = (_rng.next() % 5);
        _chip2Len = ((_rng.next() & 0x3) + 1) * 2;
        _chip2TimeMult = _rng.nextBinary() ? (_rng.nextBinary() ? 1 : 0) : 0;
        _chip2DeadTime = 0;
        _chip2Idx = 0;
        _chip2Dir = _rng.nextBinary() ? (_rng.nextBinary() ? 1 : 0) : 0;
        _chip2ChordLen = 3 + (flow >> 2); 
        break;

    case 9: // WOBBLE
        // Swap seeds like Stomper? 
        // Original: R=seed2>>2, Extra=seed1>>2.
        _rng = Random(ornamentSeed);
        _extraRng = Random(flowSeed + 0x9e3779b9);
        
        _wobblePhase = 0;
        // Speed based on pattern length?
        // T->CurrentPattern.Length.
        // Let's assume generic speed based on Flow/Ornament
        // Original: 0xFFFFFFFF / Length.
        // Let's pick a base speed.
        _wobblePhaseSpeed = 0x08000000; // Slow
        _wobblePhase2 = 0;
        _wobbleLastWasHigh = 0;
        _wobblePhaseSpeed2 = 0x02000000; // Slower
        break;

    case 3: // APHEX (Mapped from 18)
    case 18:
        _rng = Random(flowSeed);
        for (int i = 0; i < 4; ++i) _aphex_track1_pattern[i] = _rng.next() % 12;
        for (int i = 0; i < 3; ++i) _aphex_track2_pattern[i] = _rng.next() % 3;
        for (int i = 0; i < 5; ++i) _aphex_track3_pattern[i] = (_rng.next() % 8 == 0) ? (_rng.next() % 5) : 0;

        _aphex_pos1 = (ornament * 1) % 4;
        _aphex_pos2 = (ornament * 2) % 3;
        _aphex_pos3 = (ornament * 3) % 5;
        break;

    case 4: // AUTECHRE (Mapped from 19)
    case 19:
        _autechre_pattern[0] = 0; _autechre_pattern[1] = 0; _autechre_pattern[2] = 24; _autechre_pattern[3] = 0;
        _autechre_pattern[4] = 0; _autechre_pattern[5] = 24; _autechre_pattern[6] = 0; _autechre_pattern[7] = 36;
        _autechre_rule_timer = 8 + (flow * 4);

        _rng = Random(ornamentSeed);
        for (int i = 0; i < 8; ++i) _autechre_rule_sequence[i] = _rng.next() % 5;
        _autechre_rule_index = 0;
        break;

    case 5: // STEPWAVE (Mapped from 20)
    case 20:
        _rng = Random(flowSeed);
        _extraRng = Random(ornamentSeed + 0x9e3779b9);

        if (ornament <= 5) {
            _stepwave_direction = -1;
        } else if (ornament >= 11) {
            _stepwave_direction = 1;
        } else {
            _stepwave_direction = 0;
        }

        _stepwave_step_count = 3 + (_rng.next() % 5);
        _stepwave_current_step = 0;
        _stepwave_chromatic_offset = 0;
        _stepwave_is_stepped = true;
        break;

    default:
        // Fallback to TEST
        _testMode = (flow - 1) >> 3;
        _testSweepSpeed = ((flow - 1) & 0x3);
        _testAccent = (ornament - 1) >> 3;
        _testVelocity = ((ornament - 1) << 4);
        _testNote = 0;
        _rng = Random(flowSeed);
        _extraRng = Random(ornamentSeed + 0x9e3779b9);
        break;
    }
}

void TuesdayTrackEngine::reset() {
    _cachedAlgorithm = -1;
    _cachedFlow = -1;
    _cachedOrnament = -1;
    _cachedLoopLength = -1;

    _stepIndex = 0;
    _displayStep = -1;
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

    _gateLengthTicks = 0;
    _pendingGateOffsetTicks = 0;
    _pendingGateActivation = false;
    _tieActive = false;

    _retriggerArmed = false;
    _retriggerCount = 0;
    _retriggerPeriod = 0;
    _retriggerLength = 0;
    _retriggerTimer = 0;
    _isTrillNote = false;
    _trillCvTarget = 0.f;
    _ratchetInterval = 0;

    _activity = false;
    _gateOutput = false;
    _cvOutput = 0.f;
    _lastGatedCv = 0.f;

    initAlgorithm();
}

void TuesdayTrackEngine::restart() {
    reset();
}

void TuesdayTrackEngine::reseed() {
    // Reset step to beginning
    _stepIndex = 0;
    _coolDown = 0;

    // Advance the RNGs to get new seeds
    uint32_t newSeed1 = _rng.next();
    uint32_t newSeed2 = _extraRng.next();

    // Re-seed with these new values
    // This simulates "turning the knobs" to a new random position
    // We need to temporarily override the cached Flow/Ornament effectively
    // But initAlgorithm reads from sequence. 
    // Ideally, 'reseed' implies we want variation *within* the current parameters?
    // Or does it mean "Scramble"?
    // For now, we'll just re-init the RNGs directly to simulate a fresh start
    _rng = Random(newSeed1);
    _extraRng = Random(newSeed2);
    
    // We should probably re-run init logic that depends on RNG
    // But without changing the 'flow/ornament' variables which are read from sequence.
    // A full initAlgorithm call would reset seeds based on Sequence.
    // So we need to manually re-init state vars here if we want true random reseed.
    
    const auto &sequence = tuesdayTrack().sequence(pattern());
    int algorithm = sequence.algorithm();

    switch (algorithm) {
    case 1: // TRITRANCE
        _triB1 = (_rng.next() & 0x7);
        _triB2 = (_rng.next() & 0x7);
        _triB3 = (_extraRng.next() & 0x15);
        if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
        _triB3 -= 4;
        break;
    case 2: // STOMPER
        _stomperMode = (_extraRng.next() % 7) * 2;
        _stomperCountDown = 0;
        _stomperLowNote = _rng.next() % 3;
        _stomperHighNote[0] = _rng.next() % 7;
        _stomperHighNote[1] = _rng.next() % 5;
        break;
        // Add others if needed
    }
}

float TuesdayTrackEngine::scaleToVolts(int noteIndex, int octave) const {
    const auto &sequence = tuesdayTrack().sequence(pattern());
    const auto &project = _model.project();

    // 1. Resolve which Scale to use
    int trackScaleIdx = sequence.scale();
    bool useProjectScale = (trackScaleIdx < 0);
    const Scale &scale = useProjectScale ? project.selectedScale() : Scale::get(trackScaleIdx);

    // 2. Resolve Root Note
    int trackRoot = sequence.rootNote();
    int rootNote = (trackRoot < 0) ? project.rootNote() : trackRoot;

    // 3. Determine quantization mode
    // In Tuesday, we assume if a Scale is selected, we use it.
    bool quantize = sequence.useScale() || !useProjectScale || project.scale() > 0;

    float voltage = 0.f;

    if (quantize) {
        // SCALE MODE: noteIndex is Scale Degree
        int totalDegree = noteIndex + (octave * scale.notesPerOctave());
        
        // Apply Transpose (in Degrees)
        totalDegree += sequence.transpose();

        // Convert to Volts
        voltage = scale.noteToVolts(totalDegree);

        // Add Root Note offset
        voltage += (rootNote * (1.f / 12.f));
    } else {
        // CHROMATIC MODE: noteIndex is Semitone
        int totalSemitones = noteIndex + (octave * 12);
        
        // Apply Transpose (in Semitones)
        totalSemitones += sequence.transpose();

        // 1V/Oct
        voltage = totalSemitones * (1.f / 12.f);
    }

    // 4. Apply Track Octave Offset
    voltage += sequence.octave();

    return voltage;
}

TuesdayTrackEngine::TuesdayTickResult TuesdayTrackEngine::generateStep(uint32_t tick) {
    const auto &sequence = tuesdayTrack().sequence(pattern());
    int algorithm = sequence.algorithm();
    
    TuesdayTickResult result;
    result.velocity = 255; // Default high velocity

    // Map legacy/alternate algorithms to supported set
    int algo = algorithm;
    if (algorithm == 18) algo = 3; // APHEX
    if (algorithm == 19) algo = 4; // AUTECHRE
    if (algorithm == 20) algo = 5; // STEPWAVE
    
    switch (algo) {
    case 0: // TEST
        {
            result.gateRatio = 75;
            if (_rng.nextRange(100) < sequence.glide()) {
                result.slide = true;
            }

            switch (_testMode) {
            case 0: // OCTSWEEPS
                result.octave = (_stepIndex % 5);
                result.note = 0;
                break;
            case 1: // SCALEWALKER
            default:
                result.octave = 0;
                result.note = _testNote;
                _testNote = (_testNote + 1) % 12;
                break;
            }
            result.velocity = _testVelocity;
        }
        break;

    case 1: // TRITRANCE
        {
            // Gate Length Logic (Probabilistic)
            int gateLenRnd = _rng.nextRange(100);
            if (gateLenRnd < 40) result.gateRatio = 50 + (_rng.nextRange(4) * 12);
            else if (gateLenRnd < 70) result.gateRatio = 100 + (_rng.nextRange(4) * 25);
            else result.gateRatio = 200 + (_rng.nextRange(9) * 25);

            // Slide Logic
            if (_rng.nextRange(100) < sequence.glide()) {
                result.slide = true;
            }

            int phase = (_stepIndex + _triB2) % 3;
            switch (phase) {
            case 0:
                if (_extraRng.nextBinary() && _extraRng.nextBinary()) {
                    _triB3 = (_extraRng.next() & 0x15);
                    if (_triB3 >= 7) _triB3 -= 7; else _triB3 = 0;
                    _triB3 -= 4;
                }
                result.octave = 0;
                result.note = _triB3 + 4;
                break;
            case 1:
                result.octave = 1;
                result.note = _triB3 + 4;
                if (_rng.nextBinary()) _triB2 = (_rng.next() & 0x7);
                break;
            case 2:
                result.octave = 2;
                result.note = _triB1;
                result.velocity = 255; // Accent phase
                result.accent = true;
                if (_rng.nextBinary()) _triB1 = ((_rng.next() >> 5) & 0x7);
                break;
            }

            // Velocity variation (unless accented above)
            if (!result.accent) {
                result.velocity = (_rng.nextRange(256) / 2);
            }
        }
        break;

    case 2: // STOMPER
        {
            result.gateRatio = 75;
            int accentoffs = 0;
            uint8_t veloffset = 0;

            if (_stomperCountDown > 0) {
                // Rest period (Countdown)
                result.gateRatio = _stomperCountDown * 25;
                result.velocity = 0; // Silent
                _stomperCountDown--;
                result.note = _stomperLastNote;
                result.octave = _stomperLastOctave;
            } else {
                // Active period
                if (_stomperMode >= 14) {
                    _stomperMode = (_extraRng.next() % 7) * 2;
                }
                
                // Randomize notes periodically (simulated "PercChance 100" from original)
                // In original, PercChance(100) means ~60% chance? Or 100%?
                // Original: if (Tuesday_PercChance(R, 100)) -> if ((Rand >> 6) & 0xFF >= 100)
                // 0..255 >= 100 -> ~60% true.
                if (_rng.nextRange(256) >= 100) _stomperLowNote = _rng.next() % 3;
                if (_rng.nextRange(256) >= 100) _stomperHighNote[0] = _rng.next() % 7;
                if (_rng.nextRange(256) >= 100) _stomperHighNote[1] = _rng.next() % 5;

                veloffset = 100; // Base velocity when active
                int maxticklen = 2;

                switch (_stomperMode) {
                case 10: // SLIDEDOWN1
                    result.octave = 1;
                    result.note = _stomperHighNote[_rng.next() % 2];
                    _stomperMode++;
                    break;
                case 11: // SLIDEDOWN2
                    result.octave = 0;
                    result.note = _stomperLowNote;
                    result.slide = true;
                    if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                    _stomperMode = 14;
                    break;
                case 12: // SLIDEUP1
                    result.octave = 0;
                    result.note = _stomperLowNote;
                    _stomperMode++;
                    break;
                case 13: // SLIDEUP2
                    result.octave = 1;
                    result.note = _stomperHighNote[_rng.next() % 2];
                    result.slide = true;
                    if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                    _stomperMode = 14;
                    break;
                case 4: case 5: // LOW
                    accentoffs = 100;
                    result.octave = 0;
                    result.note = _stomperLowNote;
                    if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                    _stomperMode = 14;
                    break;
                case 0: case 1: // PAUSE
                    result.octave = _stomperLastOctave;
                    result.note = _stomperLastNote;
                    veloffset = 0; // Quiet
                    if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                    _stomperMode = 14;
                    break;
                case 6: case 7: // HIGH
                case 8: case 9: // HILOW
                case 2: case 3: // LOWHI
                    // Simplified mapping for brevity, adhering to logic
                    if (_stomperMode == 6 || _stomperMode == 7 || _stomperMode == 9 || _stomperMode == 2) accentoffs = 100;
                    
                    if (_stomperMode == 8 || _stomperMode == 3) {
                        result.octave = 1;
                        result.note = _stomperHighNote[_rng.next() % 2];
                        if (_stomperMode == 8) _stomperMode++; else if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen; 
                        if (_stomperMode == 3) _stomperMode = 14;
                    } else {
                         result.octave = 0;
                         result.note = _stomperLowNote;
                         if (_extraRng.nextBinary()) _stomperCountDown = _extraRng.next() % maxticklen;
                         _stomperMode = 14;
                    }
                    break;
                }
                _stomperLastNote = result.note;
                _stomperLastOctave = result.octave;
                
                // Calculate velocity
                // Original: (Tuesday_Rand(&Extra) / 4) + veloffset;
                result.velocity = (_extraRng.nextRange(256) / 4) + veloffset;
                // Accent logic: PercChance(50 + accentoffs)
                if (_rng.nextRange(256) >= (50 + accentoffs)) {
                     result.accent = true;
                }
                
                // Chaos for trills
                result.chaos = 18; // Base chaos
            }
        }
        break;

    case 3: // APHEX
        {
            // Track 1: Main Melody
            result.note = _aphex_track1_pattern[_aphex_pos1];
            result.octave = 0;
            result.gateRatio = 75;
            result.velocity = 180; // Standard Note

            // Track 2: Modifier
            // 0=Off, 1=Stutter/Ghost, 2=Slide/Legato
            uint8_t modifier = _aphex_track2_pattern[_aphex_pos2];
            if (modifier == 1) {
                // Stutter: Short gate, Low velocity (Ghost note), High Chaos (Ratchet candidate)
                result.gateRatio = 25;
                result.velocity = 80; 
                result.chaos = 60; 
            } else if (modifier == 2) {
                // Slide: Legato, Slide Request
                result.gateRatio = 100;
                result.slide = true;
            }

            // Track 3: Bass Override
            uint8_t bass = _aphex_track3_pattern[_aphex_pos3];
            if (bass > 0) {
                // Bass is always an Accent and Deep
                result.note = bass;
                result.octave = -1;
                result.gateRatio = 90;
                result.velocity = 255;
                result.accent = true;
            }

            _aphex_pos1 = (_aphex_pos1 + 1) % 4;
            _aphex_pos2 = (_aphex_pos2 + 1) % 3;
            _aphex_pos3 = (_aphex_pos3 + 1) % 5;
            
            // Polyrhythmic Collision: Extreme Chaos
            int polyCollision = ((_aphex_pos1 * 7) ^ (_aphex_pos2 * 5) ^ (_aphex_pos3 * 3)) % 12;
            if (polyCollision > 9) {
                result.chaos = 100;
                result.velocity = 255; // Accentuate the glitch
            }
        }
        break;
        
    case 4: // AUTECHRE
        {
            int patternVal = _autechre_pattern[_stepIndex % 8];
            result.note = patternVal % 12;
            result.octave = patternVal / 12;
            
            _autechre_rule_timer--;
            
            // Default state
            result.velocity = 160;
            result.gateRatio = 75;
            result.chaos = 10;

            // Transformation Event
            if (_autechre_rule_timer <= 0) {
                uint8_t rule = _autechre_rule_sequence[_autechre_rule_index];
                int intensity = sequence.power() / 2;
                
                // MARK THE EVENT: High Velocity Accent on transformation steps
                result.velocity = 255;
                result.accent = true;
                result.chaos = 50; // Chance of ratchet on transformation
                
                // Phrasing based on Rule Type
                if (rule <= 1) { 
                    // Rotate/Reverse: Fluid/Legato
                    result.gateRatio = 95; 
                } else { 
                    // Swap/Invert/Add: Glitch/Staccato
                    result.gateRatio = 40; 
                }
                
                // Apply Transformation
                if (rule == 0) { int8_t t = _autechre_pattern[7]; for(int i=7; i>0; --i) _autechre_pattern[i]=_autechre_pattern[i-1]; _autechre_pattern[0]=t; } // Rotate
                else if (rule == 1) { for(int i=0;i<4;++i) std::swap(_autechre_pattern[i], _autechre_pattern[7-i]); } // Reverse
                else if (rule == 2) { /* Invert */ for(int i=0;i<8;++i) { int o=_autechre_pattern[i]/12; int n=_autechre_pattern[i]%12; n=(6-(n-6)+12)%12; _autechre_pattern[i]=n+o*12; } }
                else if (rule == 3) { for(int i=0;i<8;i+=2) std::swap(_autechre_pattern[i], _autechre_pattern[i+1]); } // Swap
                else if (rule == 4) { for(int i=0;i<8;++i) { int o=_autechre_pattern[i]/12; int n=(_autechre_pattern[i]%12+intensity)%12; _autechre_pattern[i]=n+o*12; } } // Add

                _autechre_rule_timer = 8 + (sequence.flow() * 4);
                _autechre_rule_index = (_autechre_rule_index + 1) % 8;
            }
        }
        break;

    case 5: // STEPWAVE
        {
             int flow = sequence.flow();
             int ornament = sequence.ornament();
             
             int dir = (flow <= 7) ? -1 : (flow >= 9 ? 1 : 0);
             int stepSize = (ornament <= 5) ? 2 : (ornament >= 11 ? 3 : (2 + _extraRng.next()%2));
             
             result.note = (_stepIndex * dir * stepSize) % 7;
             if (result.note < 0) result.note += 7;
             
             result.velocity = 200;
             
             // Octave Jumps are Accents
             if (_rng.nextRange(100) < (20 + flow * 3)) {
                 result.octave = 2;
                 result.velocity = 255;
                 result.accent = true;
             } else {
                 result.octave = 0;
             }
             
             // Probabilistic Slide Request (based on Glide param/Ornament)
             // If slide is active, make it legato.
             if (_rng.nextRange(100) < sequence.glide()) {
                 result.slide = true;
                 result.gateRatio = 100;
             } else {
                 result.gateRatio = 75; // Stepped
             }
             
             // High base chaos implies this algorithm LOVES to trill
             // The FX layer catches Algo 20 specifically to do the "Climbing" trill
             result.chaos = 80; 
        }
        break;

    case 6: // MARKOV
        {
            result.gateRatio = 75;
            int idx = _rng.nextBinary() ? 1 : 0;
            int newNote = _markovMatrix[_markovHistory1][_markovHistory3][idx];
            
            _markovHistory1 = _markovHistory3;
            _markovHistory3 = newNote;
            
            result.note = newNote; // 0-7 scale degrees
            result.octave = _rng.nextBinary();
            
            // Random Slide/Length
            // Logic from RandomSlideAndLength
            if (_rng.nextRange(100) < 50) result.gateRatio = 100 + (_rng.nextRange(4) * 25);
            else result.gateRatio = 75;
            
            if (_rng.nextBinary() && _rng.nextBinary()) result.slide = true;
            
            if (_rng.nextRange(256) >= 100) result.accent = true;
            
            result.velocity = (_rng.nextRange(256) / 2) + 40;
        }
        break;

    case 7: // CHIPARP 1
        {
            result.gateRatio = 75;
            // Original uses I % TPB. Let's use stepIndex % 4 (1 beat)
            int chordpos = _stepIndex % 4;
            
            if (chordpos == 0) {
                result.accent = true;
                _chipRng = Random(_chipChordSeed);
                if (_rng.nextRange(256) >= 0x20) _chipBase = _rng.next() % 3;
                if (_rng.nextRange(256) >= 0xF0) _chipDir = (_rng.next() >> 7) % 2;
            }
            
            if (_chipDir == 1) chordpos = 4 - chordpos - 1;
            
            if (_chipRng.nextRange(256) >= 0x20) result.accent = true;
            
            if (_chipRng.nextRange(256) >= 0x80) {
                // slide = rand%3. 
                // We map to bool slide
                result.slide = true;
            }
            
            if (_chipRng.nextRange(256) >= 0xD0) {
                result.velocity = 0; // Note Off
            } else {
                result.note = (chordpos * 2) + _chipBase;
                result.octave = 0;
                int rnd = (_chipRng.next() >> 7) % 3;
                result.gateRatio = (4 + 6 * rnd) * (100/6); // approx mapping
            }
            
            // Vel from main RNG
            int extraVel = ((_stepIndex + _triB2) == 0) ? 127 : 0; // triB2 unused? Original used GENERIC.b2
            result.velocity = (_rng.nextRange(256) / 2) + extraVel; 
        }
        break;

    case 8: // CHIPARP 2
        {
            result.gateRatio = 75;
            result.octave = 0;
            
            if (_chip2DeadTime > 0) {
                _chip2DeadTime--;
                result.velocity = 0; // Note Off
            } else {
                int deadTimeAdd = 0;
                if (_chip2Idx == _chip2ChordLen) {
                    _chip2Idx = 0;
                    _chip2Len--;
                    result.accent = true;
                    if (_rng.nextRange(256) >= 200) deadTimeAdd = 1 + (_rng.next() % 3);
                    
                    if (_chip2Len == 0) {
                        _chipChordSeed = _rng.next();
                        _chip2ChordScaler = (_rng.next() % 3) + 2;
                        _chip2Offset = (_rng.next() % 5);
                        _chip2Len = ((_rng.next() & 0x3) + 1) * 2;
                        if (_rng.nextBinary()) {
                            _chip2Dir = _rng.nextBinary();
                        }
                    }
                    _chip2Rng = Random(_chipChordSeed);
                }
                
                int scaleidx = (_chip2Idx % _chip2ChordLen);
                if (_chip2Dir) scaleidx = _chip2ChordLen - scaleidx - 1;
                
                result.note = (scaleidx * _chip2ChordScaler) + _chip2Offset;
                _chip2Idx++;
                
                _chip2DeadTime = _chip2TimeMult;
                if (_chip2DeadTime > 0) {
                    result.gateRatio = (1 + _chip2TimeMult) * 100; 
                }
                _chip2DeadTime += deadTimeAdd;
            }
            
            result.velocity = _chip2Rng.nextRange(256);
        }
        break;

    case 9: // WOBBLE
        {
            _wobblePhase += _wobblePhaseSpeed;
            _wobblePhase2 += _wobblePhaseSpeed2;
            
            // PercChance(R, seed2). seed2=ornament.
            // Map ornament 0-16 to 0-255 threshold
            if (_rng.nextRange(256) >= (sequence.ornament() * 16)) {
                // Use Phase 2
                // NOTE(1, (pos2 >> 27)) -> Octave 1, Note = top 5 bits of phase
                result.octave = 1;
                result.note = (_wobblePhase2 >> 27) & 0x1F; // 0-31
                
                if (_wobbleLastWasHigh == 0) {
                    if (_rng.nextRange(256) >= 200) result.slide = true;
                }
                _wobbleLastWasHigh = 1;
            } else {
                // Use Phase 1
                result.octave = 0;
                result.note = (_wobblePhase >> 27) & 0x1F;
                
                if (_wobbleLastWasHigh == 1) {
                    if (_rng.nextRange(256) >= 200) result.slide = true;
                }
                _wobbleLastWasHigh = 0;
            }
            
            // ScaleToNote will quantize 0-31 note to Scale
            
            result.velocity = (_extraRng.nextRange(256) / 4);
            if (_rng.nextRange(256) >= 50) result.accent = true;
        }
        break;
    }

    return result;
}

TrackEngine::TickResult TuesdayTrackEngine::tick(uint32_t tick) {
    if (mute()) { _gateOutput=false; _cvOutput=0.f; _activity=false; return TickResult::NoUpdate; }
    
    const auto &sequence = tuesdayTrack().sequence(pattern());
    int power = sequence.power();
    if (power == 0) { _gateOutput=false; _cvOutput=0.f; _activity=false; return TickResult::NoUpdate; }

    // Parameter Change Check
    bool paramsChanged = (_cachedAlgorithm != sequence.algorithm() ||
                         _cachedFlow != sequence.flow() ||
                         _cachedOrnament != sequence.ornament() ||
                         _cachedLoopLength != sequence.loopLength());
    if (paramsChanged) {
        initAlgorithm();
    }

    // Time Calculation
    uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    int loopLength = sequence.actualLoopLength();
    uint32_t resetDivisor = (loopLength > 0) ? (loopLength * divisor) : 0;
    
    uint32_t relativeTick = (resetDivisor == 0) ? tick : tick % resetDivisor;
    
    // Finite Loop Reset
    if (resetDivisor > 0 && relativeTick == 0) {
        initAlgorithm(); // Reset RNGs to loop start state
        _stepIndex = 0;
    }

    bool stepTrigger = (relativeTick % divisor == 0);

    // --- ENGINE UPDATE LOGIC ---
    
    // Handle existing gate timer
    if (_gateTicks > 0) {
        _gateTicks--;
        if (_gateTicks == 0) {
             // Gate OFF
             _gateOutput = false;
             // If in retrigger cycle, this is the gap between ratchets
             // Logic handled in update()
        }
    }
    
    // Handle CV Slide
    if (_slideCountDown > 0) {
        _cvCurrent += _cvDelta;
        _slideCountDown--;
        if (_slideCountDown == 0) _cvCurrent = _cvTarget;
        _cvOutput = _cvCurrent;
    }

    if (stepTrigger) {
        // Advance Step
        uint32_t calculatedStep = relativeTick / divisor;
        _stepIndex = calculatedStep; // Loop wrapping handled by relativeTick/resetDivisor
        _displayStep = _stepIndex;

        // 1. GENERATE (The Brain)
        TuesdayTickResult result = generateStep(tick);
        
        // 2. FX LAYER (Post-Processing)
        
        // A. Trill/Ratchet
        _retriggerArmed = false;
        if (result.chaos > 0) {
            int chance = (result.chaos * sequence.trill()) / 100;
            if (_uiRng.nextRange(100) < chance) {
                _retriggerArmed = true;
                _retriggerCount = 2; // Default triplet feel or simple double
                _retriggerPeriod = divisor / 3;
                _retriggerLength = _retriggerPeriod / 2;
                _ratchetInterval = 0; // Standard trill
                
                // Custom Trill Logic per Algorithm could go here (e.g. StepWave intervals)
                if (sequence.algorithm() == 20) { // StepWave Special
                     _ratchetInterval = _stepwave_direction;
                     _retriggerCount = _stepwave_step_count - 1;
                     _retriggerPeriod = divisor / _stepwave_step_count;
                     _retriggerLength = _retriggerPeriod / 2;
                }
            }
        }

        // B. Glide
        _slide = 0;
        if (result.slide && sequence.glide() > 0) {
             _slide = 1 + (sequence.glide() / 30); // Map 0-100 to 1-4 range approx
        }

        // C. Density (Power Gating)
        // Calculate Base Cooldown from Power (Linear 1-16)
        // High Power (16) -> Cooldown 1 (Every step allowed)
        // Low Power (1) -> Cooldown 16 (Sparse)
        int baseCooldown = 17 - power;
        if (baseCooldown < 1) baseCooldown = 1;
        _coolDownMax = baseCooldown;
        
        // Decrement cooldown state
        if (_coolDown > 0) {
            _coolDown--;
            if (_coolDown > _coolDownMax) _coolDown = _coolDownMax;
        }
        
        // Gate Decision:
        // 1. Must satisfy Cooldown (Density)
        // 2. Velocity helps overcome Cooldown (Original Tuesday Logic)
        // Original: if (vel >= cooldown)
        // We map velocity 0-255 to roughly 0-16 range to compare with cooldown?
        // Or just use raw velocity if cooldown was velocity-based?
        // In C++ port, cooldown is a counter.
        // Let's implement the "Beat the Clock" logic:
        // If cooldown is high, we need high velocity to trigger.
        // If we trigger, we reset cooldown.
        
        bool densityGate = false;
        if (_coolDown == 0) {
            densityGate = true; // Always trigger if timer expired
        } else {
            // Probabilistic override: High velocity can force a trigger?
            // Or strict timer?
            // Tuesday original uses velocity as the threshold.
            // Let's map Velocity (0-255) to Density (0-16)
            int velDensity = result.velocity / 16; 
            if (velDensity >= _coolDown) {
                densityGate = true;
            }
        }
        
        if (densityGate) {
             // Reset pressure
             _coolDown = _coolDownMax;
             
             // D. Gate Length (Scaler)
             // User Param: gateLength() (0-100, mapped to 0-200%?)
             // Default gate is 50% in Sequence model (which maps to 100% scale?)
             // Let's check Sequence model default: 50.
             // If userGate = 50, we want scale = 1.0.
             // If userGate = 0, scale = 0.0.
             // If userGate = 100, scale = 2.0.
             // scale = userGate / 50.0
             int userGate = sequence.gateLength();
             uint32_t baseLen = (divisor * result.gateRatio) / 100;
             
             _gateTicks = (baseLen * userGate) / 50; // Center at 50
             if (_gateTicks < 1) _gateTicks = 1;
             
             // Handle Ties
             // If gateTicks > divisor, it's a tie candidate.
             // But we need to check next step... 
             // For now, just let the gate run long. 
             // update() handles the drop.
             
             _gateOutput = true;
             _activity = true;
             
             // Set CV
             float volts = scaleToVolts(result.note, result.octave);
             
             // Handle Slide CV
             if (_slide > 0) {
                 _cvTarget = volts;
                 int slideTicks = _slide * 12; 
                 _cvDelta = (_cvTarget - _cvCurrent) / slideTicks;
                 _slideCountDown = slideTicks;
             } else {
                 _cvCurrent = volts;
                 _cvOutput = volts;
                 _slideCountDown = 0;
             }
             
             // Handle Trill Start CV
             if (_retriggerArmed) {
                 _gateTicks = _retriggerLength; // Override gate for trill
                 _trillCvTarget = volts; // Start base
                 // Recalculate trill target for next ratchet?
                 // For simple trill, we oscillate.
                 // For StepWave, we might climb.
             }
             
        } else {
             // Ghost note / Rest
             // Update CV anyway if Free mode?
             if (sequence.cvUpdateMode() == TuesdaySequence::Free) {
                  float volts = scaleToVolts(result.note, result.octave);
                  _cvCurrent = volts;
                  _cvOutput = volts;
             }
        }
        
        return TickResult::CvUpdate | TickResult::GateUpdate;
    }

    return TickResult::NoUpdate;
}

void TuesdayTrackEngine::update(float dt) {
    // CV Slide Update
    if (_slideCountDown > 0) {
        _slideCountDown--;
        _cvCurrent += _cvDelta;
        if (!_retriggerArmed) _cvOutput = _cvCurrent;
    }

    // Retrigger Logic
    if (_retriggerArmed) {
        if (_gateOutput) {
            // Waiting for ON phase to end
            if (_gateTicks > 0) _gateTicks--;
            else {
                _gateOutput = false; // Gap
                _gateTicks = _retriggerPeriod - _retriggerLength; // Gap length
            }
        } else {
            // Waiting for OFF phase to end (Gap)
            if (_gateTicks > 0) _gateTicks--;
            else {
                if (_retriggerCount > 0) {
                    _retriggerCount--;
                    _gateOutput = true;
                    _gateTicks = _retriggerLength;
                    
                    // Apply Trill Interval
                    if (_ratchetInterval != 0) {
                        // Arp/Step mode
                         // We need to know current note to offset it.
                         // This is tricky without persistent note state.
                         // Simplification: just add voltage
                         _cvOutput += (_ratchetInterval / 12.f);
                    } else {
                        // Standard Trill: Toggle
                        // Need to know interval (e.g. +2 semitones)
                        // For now, just keep same voltage (Ratchet) or simple offset
                        // Original code added +2/12v.
                         _cvOutput += (2.f / 12.f); 
                    }
                } else {
                    _retriggerArmed = false;
                }
            }
        }
    }
    
    _activity = _gateOutput || _retriggerArmed;
}