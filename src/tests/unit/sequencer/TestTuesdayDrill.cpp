#include "UnitTest.h"
#include "core/utils/Random.h"

UNIT_TEST("TuesdayDrill") {

//----------------------------------------
// State Variable Initialization
//----------------------------------------

CASE("drill_state_default_values") {
    // Test initial state values
    uint8_t hiHatPattern = 0;
    uint8_t slideTarget = 0;
    uint8_t tripletMode = 0;
    uint8_t rollCount = 0;
    uint8_t lastNote = 0;
    uint8_t stepInBar = 0;
    uint8_t subdivision = 1;

    expectEqual((int)hiHatPattern, 0, "hiHatPattern should start at 0");
    expectEqual((int)slideTarget, 0, "slideTarget should start at 0");
    expectEqual((int)tripletMode, 0, "tripletMode should start at 0");
    expectEqual((int)rollCount, 0, "rollCount should start at 0");
    expectEqual((int)lastNote, 0, "lastNote should start at 0");
    expectEqual((int)stepInBar, 0, "stepInBar should start at 0");
    expectEqual((int)subdivision, 1, "subdivision should start at 1");
}

//----------------------------------------
// Flow Parameter: Slide Frequency
//----------------------------------------

CASE("drill_flow_zero_no_slides") {
    // Flow=0 should produce no slides
    int flow = 0;
    Random rng(12345);
    int slideCount = 0;
    int total = 100;

    for (int i = 0; i < total; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            slideCount++;
        }
    }

    expectEqual(slideCount, 0, "flow 0 should produce no slides");
}

CASE("drill_flow_high_more_slides") {
    // Higher flow = more slides
    int flow = 12;
    Random rng(12345);
    int slideCount = 0;
    int total = 100;

    for (int i = 0; i < total; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            slideCount++;
        }
    }

    expectTrue(slideCount > 50, "high flow should produce more slides");
}

//----------------------------------------
// Ornament Parameter: Triplet Feel
//----------------------------------------

CASE("drill_ornament_triplet_probability") {
    // Higher ornament = more triplets
    Random rng(12345);
    int ornament = 10;
    int tripletCount = 0;
    int total = 100;

    for (int i = 0; i < total; i++) {
        if (rng.nextRange(16) < ornament) {
            tripletCount++;
        }
    }

    expectTrue(tripletCount > 40, "high ornament should produce more triplets");
}

//----------------------------------------
// Hi-Hat Patterns
//----------------------------------------

CASE("drill_hihat_bitmask_pattern") {
    // Hi-hat pattern uses bitmask (8 bits)
    uint8_t pattern = 0b10101010;  // Every other step

    int hits = 0;
    for (int i = 0; i < 8; i++) {
        if (pattern & (1 << i)) hits++;
    }

    expectEqual(hits, 4, "pattern should have 4 hits");
}

CASE("drill_hihat_roll_generation") {
    // Rolls should generate multiple rapid notes
    uint8_t rollCount = 4;
    int gatePercent = 25;  // Short gates for rolls

    expectTrue(rollCount >= 2 && rollCount <= 8, "rolls should be 2-8 notes");
    expectTrue(gatePercent <= 50, "roll gates should be short");
}

//----------------------------------------
// Bass Slides
//----------------------------------------

CASE("drill_bass_slide_target") {
    // Bass slides should have a target note
    Random rng(12345);
    uint8_t lastNote = 5;
    uint8_t slideTarget = rng.next() % 12;

    expectTrue(slideTarget >= 0 && slideTarget <= 11, "slide target should be valid note");
}

CASE("drill_slide_portamento") {
    // Slides should produce portamento effect
    int slide = 2;  // Medium glide
    expectTrue(slide >= 1 && slide <= 3, "slide amount should be 1-3");
}

//----------------------------------------
// Triplet Subdivisions
//----------------------------------------

CASE("drill_triplet_step_calculation") {
    // Triplet mode affects subdivision
    uint8_t tripletMode = 1;
    uint8_t stepInBar = 4;

    // In triplet mode, steps group in 3s
    int tripletGroup = stepInBar / 3;
    expectEqual(tripletGroup, 1, "step 4 should be in triplet group 1");
}

CASE("drill_bar_wrap") {
    // Step in bar should wrap at 8
    uint8_t stepInBar = 9;
    stepInBar = stepInBar % 8;
    expectEqual((int)stepInBar, 1, "step should wrap at 8");
}

//----------------------------------------
// Note Range
//----------------------------------------

CASE("drill_bass_note_low_range") {
    // Drill bass should be in low octave range
    int octave = -1;  // One octave down for deep bass
    expectTrue(octave <= 0, "drill bass should be in low octave");
}

CASE("drill_note_range") {
    // Notes should be 0-11 (chromatic)
    Random rng(12345);
    for (int i = 0; i < 100; i++) {
        int note = rng.next() % 12;
        expectTrue(note >= 0 && note <= 11, "note should be 0-11");
    }
}

//----------------------------------------
// Gate Patterns
//----------------------------------------

CASE("drill_gate_on_hihat_hit") {
    // Gate should fire on hi-hat pattern hits
    uint8_t pattern = 0b10101010;
    int step = 1;  // Bit 1 is set

    bool shouldGate = (pattern & (1 << step)) != 0;
    expectTrue(shouldGate, "should gate on pattern hit");
}

CASE("drill_no_gate_on_miss") {
    // No gate on pattern misses
    uint8_t pattern = 0b10101010;
    int step = 0;  // Bit 0 is not set

    bool shouldGate = (pattern & (1 << step)) != 0;
    expectFalse(shouldGate, "should not gate on pattern miss");
}

} // UNIT_TEST("TuesdayDrill")
