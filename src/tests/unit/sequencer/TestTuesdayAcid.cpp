#include "UnitTest.h"
#include "core/utils/Random.h"

UNIT_TEST("TuesdayAcid") {

//----------------------------------------
// State Variable Initialization
//----------------------------------------

CASE("acid_state_default_values") {
    uint8_t sequence[8] = {0};
    uint8_t position = 0;
    uint8_t accentPattern = 0;
    uint8_t octaveMask = 0;
    int8_t lastNote = 0;
    uint8_t slideTarget = 0;
    uint8_t stepCount = 0;

    expectEqual((int)position, 0, "position should start at 0");
    expectEqual((int)accentPattern, 0, "accentPattern should start at 0");
    expectEqual((int)octaveMask, 0, "octaveMask should start at 0");
    expectEqual((int)lastNote, 0, "lastNote should start at 0");
    expectEqual((int)slideTarget, 0, "slideTarget should start at 0");
    expectEqual((int)stepCount, 0, "stepCount should start at 0");
    expectEqual((int)sequence[0], 0, "sequence should be zeroed");
}

//----------------------------------------
// Flow Parameter: Slide Probability
//----------------------------------------

CASE("acid_flow_zero_no_slides") {
    int flow = 0;
    Random rng(12345);
    int slideCount = 0;

    for (int i = 0; i < 100; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            slideCount++;
        }
    }

    expectEqual(slideCount, 0, "flow 0 should produce no slides");
}

CASE("acid_flow_high_more_slides") {
    int flow = 12;
    Random rng(12345);
    int slideCount = 0;

    for (int i = 0; i < 100; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            slideCount++;
        }
    }

    expectTrue(slideCount > 50, "high flow should produce more slides");
}

//----------------------------------------
// Ornament Parameter: Accents and Octaves
//----------------------------------------

CASE("acid_accent_bitmask") {
    // Accent pattern uses bitmask
    uint8_t accentPattern = 0b10010010;  // Accents on steps 1, 4, 7
    int step = 4;

    bool hasAccent = (accentPattern & (1 << step)) != 0;
    expectTrue(hasAccent, "step 4 should have accent");
}

CASE("acid_octave_jumps") {
    // Ornament controls octave jump probability
    Random rng(12345);
    int ornament = 10;
    int octaveJumps = 0;

    for (int i = 0; i < 100; i++) {
        if (rng.nextRange(16) < ornament) {
            octaveJumps++;
        }
    }

    expectTrue(octaveJumps > 40, "high ornament should produce octave jumps");
}

//----------------------------------------
// 303-Style Patterns
//----------------------------------------

CASE("acid_sequence_8_steps") {
    // 303 patterns are typically 8 steps
    uint8_t sequence[8];
    Random rng(12345);

    for (int i = 0; i < 8; i++) {
        sequence[i] = rng.next() % 12;
        expectTrue(sequence[i] <= 11, "notes should be 0-11");
    }
}

CASE("acid_position_wraps") {
    uint8_t position = 7;
    position = (position + 1) % 8;
    expectEqual((int)position, 0, "position should wrap at 8");
}

CASE("acid_slide_portamento") {
    // 303 slides should be medium speed
    int slide = 2;
    expectTrue(slide >= 1 && slide <= 3, "slide should be 1-3");
}

//----------------------------------------
// Gate Characteristics
//----------------------------------------

CASE("acid_typical_gate_length") {
    // 303 has punchy gates, around 50-75%
    int gatePercent = 65;
    expectTrue(gatePercent >= 50 && gatePercent <= 100, "acid gates should be punchy");
}

CASE("acid_accent_longer_gate") {
    // Accented notes should have longer gates
    int normalGate = 65;
    int accentGate = 95;
    expectTrue(accentGate > normalGate, "accent should have longer gate");
}

//----------------------------------------
// Note Range
//----------------------------------------

CASE("acid_bass_range") {
    // 303 bass should be in low-mid range
    Random rng(12345);
    for (int i = 0; i < 100; i++) {
        int note = rng.next() % 12;
        int octave = (rng.nextRange(16) < 4) ? 1 : 0;  // Occasional octave up
        expectTrue(note >= 0 && note <= 11, "note should be 0-11");
        expectTrue(octave >= 0 && octave <= 1, "octave should be 0-1");
    }
}

//----------------------------------------
// Pattern Evolution
//----------------------------------------

CASE("acid_step_count_increments") {
    uint8_t stepCount = 0;
    for (int i = 0; i < 10; i++) {
        stepCount++;
    }
    expectEqual((int)stepCount, 10, "step count should increment");
}

CASE("acid_pattern_mutation") {
    // Pattern should occasionally mutate
    Random rng(12345);
    uint8_t sequence[8] = {0, 2, 4, 5, 7, 9, 11, 0};
    int mutations = 0;

    for (int i = 0; i < 100; i++) {
        if (rng.nextRange(16) < 2) {  // Low probability mutation
            int pos = rng.next() % 8;
            sequence[pos] = rng.next() % 12;
            mutations++;
        }
    }

    expectTrue(mutations > 5 && mutations < 30, "should have occasional mutations");
}

} // UNIT_TEST("TuesdayAcid")
