#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <random>

// Simplified version of the Random class to simulate the Tuesday algorithms
class Random {
private:
    uint32_t _state;
    
public:
    Random(uint32_t seed = 0x12345678) : _state(seed) {}
    
    uint32_t next() {
        _state = _state * 1103515245 + 12345;
        return _state;
    }
    
    int nextRange(int max) {
        return next() % max;
    }
    
    bool nextBinary() {
        return (next() & 1) != 0;
    }
};

// Simulate APHEX algorithm behavior
class AphexSimulator {
private:
    Random _rng;
    Random _extraRng;
    std::vector<int> _pattern;
    int _timeSigNum;
    int _glitchProb;
    int _position;
    int _noteIndex;
    int _lastNote;
    int _stepCounter;
    
public:
    AphexSimulator(int flow, int ornament) : _rng((flow - 1) << 4), _extraRng((ornament - 1) << 4) {
        // Initialize pattern
        _pattern.resize(8);
        for (int i = 0; i < 8; i++) {
            _pattern[i] = _rng.next() % 12;
        }
        
        _timeSigNum = 3 + (flow % 5);  // 3, 4, 5, 6, 7
        _glitchProb = ornament * 16;  // 0-255 scale
        _position = 0;
        _noteIndex = 0;
        _lastNote = _pattern[0];
        _stepCounter = 0;
    }
    
    struct StepResult {
        int note;
        int gatePercent;
        bool shouldGate;
    };
    
    StepResult step() {
        StepResult result;
        
        // Get note from pattern with polyrhythmic position
        result.note = _pattern[_noteIndex];
        
        // Varied gate lengths (Aphex Twin style)
        result.gatePercent = 25 + (_extraRng.next() % 75);  // 25-100%
        
        // Glitch effect
        if (_extraRng.nextRange(256) < _glitchProb) {
            // Glitch can repeat, shift, or mutate
            int glitchType = _extraRng.next() % 3;
            if (glitchType == 0) {
                result.note = _lastNote;  // Repeat
            } else if (glitchType == 1) {
                result.note = (result.note + 7) % 12;  // Fifth shift
            } else {
                result.gatePercent = 15 + (_extraRng.next() % 30);  // Short stutter
            }
        }
        
        _lastNote = result.note;
        
        // Advance position with odd time signature
        _position = (_position + 1) % _timeSigNum;
        
        // Update note index when position wraps
        if (_position == 0) {
            _noteIndex = (_noteIndex + 1) % 8;
        }
        
        result.shouldGate = true;  // Always gate in this simplified model
        _stepCounter++;
        
        return result;
    }
};

// Simulate AMBIENT algorithm behavior
class AmbientSimulator {
private:
    Random _rng;
    Random _extraRng;
    int _lastNote;
    int _holdTimer;
    int _driftDir;
    int _driftAmount;
    int _harmonic;
    int _silenceCount;
    int _driftCounter;
    int _stepCounter;
    
public:
    AmbientSimulator(int flow, int ornament) : _rng((flow - 1) << 4), _extraRng((ornament - 1) << 4) {
        _lastNote = _rng.next() % 12;
        _holdTimer = (_rng.next() % 8) + 4;  // 4-11 steps
        _driftDir = (_rng.next() % 2) ? 1 : -1;
        _driftAmount = flow;  // Flow controls drift speed
        _harmonic = _extraRng.next() % 4;  // Harmonic interval type
        _silenceCount = 0;
        _driftCounter = 0;
        _stepCounter = 0;
    }
    
    struct StepResult {
        int note;
        int gatePercent;
        bool shouldGate;
    };
    
    StepResult step() {
        StepResult result;
        
        // Very long gates like DRONE - let cooldown handle density
        result.gatePercent = 200;  // Long sustained notes (200% = ties over)
        result.shouldGate = true;
        
        // Slow pitch change rate based on flow (every 8-32 steps)
        int changeRate = 8 + (16 - _driftAmount);
        if (changeRate < 8) changeRate = 8;
        
        _driftCounter++;
        if (_driftCounter >= changeRate) {
            _driftCounter = 0;
            // Change note slowly via drift
            _lastNote = (_lastNote + _driftDir + 12) % 12;
            
            // Occasionally change drift direction
            if (_rng.next() % 8 == 0) {
                _driftDir = -_driftDir;
            }
        }
        
        result.note = _lastNote;
        
        // Add harmonics based on ornament
        int harmonicType = _extraRng.next() % 4;
        switch (harmonicType) {
        case 0: break;  // Unison
        case 1: result.note = (result.note + 5) % 12; break;  // Fourth
        case 2: result.note = (result.note + 7) % 12; break;  // Fifth
        case 3: break;  // Octave up (would be handled differently in full implementation)
        }
        
        _stepCounter++;
        return result;
    }
};

// Simulate AUTECHRE algorithm behavior
class AutechreSimulator {
private:
    Random _rng;
    Random _extraRng;
    uint32_t _transformState[2];
    int _mutationRate;
    uint32_t _chaosSeed;
    int _stepCount;
    int _currentNote;
    int _patternShift;
    
public:
    AutechreSimulator(int flow, int ornament) : _rng((flow - 1) << 4), _extraRng((ornament - 1) << 4) {
        _transformState[0] = _rng.next();
        _transformState[1] = _extraRng.next();
        _mutationRate = flow * 16;  // 0-255 scale
        _chaosSeed = _rng.next();
        _stepCount = 0;
        _currentNote = _rng.next() % 12;
        _patternShift = 0;
    }
    
    struct StepResult {
        int note;
        int gatePercent;
        bool shouldGate;
    };
    
    StepResult step() {
        StepResult result;
        
        // Note from transform state
        result.note = (_currentNote + _patternShift) % 12;
        
        // Irregular gates (15-100%)
        result.gatePercent = 15 + (_extraRng.next() % 85);
        result.shouldGate = true;
        
        // Transform state evolution based on mutation rate
        if (_rng.nextRange(256) < _mutationRate) {
            _transformState[0] = _rng.next();
            _transformState[1] = _extraRng.next();
            // Update chaos seed
            _chaosSeed = _transformState[0] ^ _transformState[1];
        }
        
        // Pattern shift evolution
        if (_rng.nextRange(16) < 4) {
            _patternShift = (_patternShift + 1) % 12;
        }
        
        // Current note evolution (more chaotic)
        int noteShift = (_rng.nextRange(5) - 2);  // -2 to +2
        _currentNote = (_currentNote + noteShift + 12) % 12;
        
        // Micro-timing check for ornament
        _extraRng.next();
        
        _stepCount++;
        return result;
    }
};

// Simulate the CV update logic with Gated mode
void simulateWithGatedCV(int algorithm, int power, int flow, int ornament) {
    std::cout << "\n=== Algorithm " << algorithm << " (power=" << power << ", flow=" << flow << ", ornament=" << ornament << ") with Gated CV Update ===\n";
    
    // Create appropriate simulator
    AphexSimulator *aphex = nullptr;
    AmbientSimulator *ambient = nullptr;
    AutechreSimulator *autechre = nullptr;
    
    if (algorithm == 18) aphex = new AphexSimulator(flow, ornament);
    else if (algorithm == 13) ambient = new AmbientSimulator(flow, ornament);
    else if (algorithm == 19) autechre = new AutechreSimulator(flow, ornament);
    else {
        std::cout << "Unsupported algorithm\n";
        return;
    }
    
    // Calculate base cooldown from power
    int effectiveLength = 16;  // Fixed reference for density calculations
    int baseCooldown = (power > 0) ? effectiveLength / power : effectiveLength;
    if (baseCooldown < 1) baseCooldown = 1;
    int cooldownMax = baseCooldown;
    
    int cooldown = 0;
    std::vector<int> cvUpdates;  // Store CV updates (only when gates fire in Gated mode)
    std::vector<int> gateSteps;  // Track when gates fire
    std::vector<int> allNotes;   // Track all notes generated (for comparison)
    
    for (int step = 0; step < 64; step++) {  // Simulate 64 steps
        int note = 0;
        bool shouldGate = false;
        int gatePercent = 0;
        
        // Get algorithm output
        if (algorithm == 18) {
            auto result = aphex->step();
            note = result.note;
            gatePercent = result.gatePercent;
            shouldGate = result.shouldGate;
        } else if (algorithm == 13) {
            auto result = ambient->step();
            note = result.note;
            gatePercent = result.gatePercent;
            shouldGate = result.shouldGate;
        } else if (algorithm == 19) {
            auto result = autechre->step();
            note = result.note;
            gatePercent = result.gatePercent;
            shouldGate = result.shouldGate;
        }
        
        allNotes.push_back(note);
        
        // Apply cooldown system
        if (cooldown > 0) {
            cooldown--;
            if (cooldown > cooldownMax) cooldown = cooldownMax;
        }
        
        // Check if gate should trigger
        bool gateTriggered = shouldGate && cooldown == 0;
        if (gateTriggered) {
            gateSteps.push_back(step);
            cvUpdates.push_back(note);  // Only update CV when gate fires in Gated mode
            cooldown = cooldownMax;  // Reset cooldown after triggering
        }
        // In Gated mode, if no gate fires, CV maintains last value
    }
    
    // Analyze results
    std::cout << "Total steps simulated: 64\n";
    std::cout << "Gate events: " << gateSteps.size() << "\n";
    std::cout << "CV updates (in Gated mode): " << cvUpdates.size() << "\n";
    
    // Count distinct pitches in CV updates (what gets output to CV in Gated mode)
    std::set<int> distinctPitches;
    for (int pitch : cvUpdates) {
        distinctPitches.insert(pitch);
    }
    
    std::cout << "Distinct pitches in CV updates: " << distinctPitches.size() << "\n";
    std::cout << "Pitches: ";
    for (int pitch : distinctPitches) {
        std::cout << pitch << " ";
    }
    std::cout << "\n";
    
    // Show first few gate steps and corresponding CV updates
    std::cout << "First 10 gate steps and CV updates:\n";
    for (int i = 0; i < std::min(10, (int)gateSteps.size()); i++) {
        std::cout << "Step " << gateSteps[i] << " -> CV=" << cvUpdates[i] << "\n";
    }
    
    // Calculate average gate length
    int totalGateLength = 0;
    for (int percent : {75}) { // Using a placeholder since we don't have the full divisor info
        // In a real system, gate length would be (divisor * gatePercent) / 100
        // For this analysis, we'll focus on the 1-step gate length pattern mentioned
    }
    
    delete aphex;
    delete ambient;
    delete autechre;
}

int main() {
    std::cout << "Analyzing TuesdayTrack algorithms with power=3, flow=5, ornament=5, CV update mode=Gated\n";
    
    // Simulate the three algorithms mentioned
    simulateWithGatedCV(18, 3, 5, 5);  // APHEX
    simulateWithGatedCV(13, 3, 5, 5);  // AMBIENT
    simulateWithGatedCV(19, 3, 5, 5);  // AUTECHRE (Autechre)
    
    return 0;
}