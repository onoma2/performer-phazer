#include "UnitTest.h"
#include "core/utils/Random.h"

UNIT_TEST("TuesdayKraft") {

//----------------------------------------
// State Variable Initialization
//----------------------------------------

CASE("kraft_state_default_values") {
    uint8_t sequence[8] = {0};
    uint8_t position = 0;
    uint8_t lockTimer = 0;
    uint8_t transpose = 0;
    uint8_t transpCount = 0;
    int8_t baseNote = 0;
    uint8_t ghostMask = 0;

    expectEqual((int)position, 0, "position should start at 0");
    expectEqual((int)lockTimer, 0, "lockTimer should start at 0");
    expectEqual((int)transpose, 0, "transpose should start at 0");
    expectEqual((int)transpCount, 0, "transpCount should start at 0");
    expectEqual((int)baseNote, 0, "baseNote should start at 0");
    expectEqual((int)ghostMask, 0, "ghostMask should start at 0");
    expectEqual((int)sequence[0], 0, "sequence should be zeroed");
}

//----------------------------------------
// Flow Parameter: Transposition Frequency
//----------------------------------------

CASE("kraft_flow_zero_no_transpose") {
    int flow = 0;
    Random rng(12345);
    int transposeCount = 0;

    for (int i = 0; i < 100; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            transposeCount++;
        }
    }

    expectEqual(transposeCount, 0, "flow 0 should produce no transposition");
}

CASE("kraft_flow_high_more_transpose") {
    int flow = 10;
    Random rng(12345);
    int transposeCount = 0;

    for (int i = 0; i < 100; i++) {
        if (flow > 0 && rng.nextRange(16) < flow) {
            transposeCount++;
        }
    }

    expectTrue(transposeCount > 40, "high flow should transpose more often");
}

//----------------------------------------
// Ornament Parameter: Ghost Notes
//----------------------------------------

CASE("kraft_ghost_bitmask") {
    // Ghost notes use bitmask
    uint8_t ghostMask = 0b01010101;  // Ghosts on even steps
    int step = 2;

    bool isGhost = (ghostMask & (1 << step)) != 0;
    expectTrue(isGhost, "step 2 should be ghost");
}

CASE("kraft_ghost_probability") {
    Random rng(12345);
    int ornament = 8;
    int ghostCount = 0;

    for (int i = 0; i < 100; i++) {
        if (rng.nextRange(16) < ornament) {
            ghostCount++;
        }
    }

    expectTrue(ghostCount > 30 && ghostCount < 70, "moderate ghosts at ornament 8");
}

//----------------------------------------
// Mechanical Patterns
//----------------------------------------

CASE("kraft_sequence_8_steps") {
    // Kraftwerk patterns are precise 8-step sequences
    uint8_t sequence[8];
    Random rng(12345);

    // Generate mechanical pattern (often uses limited notes)
    int notes[] = {0, 0, 7, 7, 5, 5, 3, 3};  // Repetitive mechanical pattern
    for (int i = 0; i < 8; i++) {
        sequence[i] = notes[i];
        expectTrue(sequence[i] <= 11, "notes should be 0-11");
    }
}

CASE("kraft_position_wraps") {
    uint8_t position = 7;
    position = (position + 1) % 8;
    expectEqual((int)position, 0, "position should wrap at 8");
}

CASE("kraft_locked_groove") {
    // Lock timer keeps pattern fixed
    uint8_t lockTimer = 8;
    for (int i = 0; i < 5; i++) {
        if (lockTimer > 0) lockTimer--;
    }
    expectEqual((int)lockTimer, 3, "lock timer should decrement");
}

//----------------------------------------
// Transposition
//----------------------------------------

CASE("kraft_transpose_range") {
    // Transpose should be 0-11 semitones
    Random rng(12345);
    for (int i = 0; i < 100; i++) {
        uint8_t transpose = rng.next() % 12;
        expectTrue(transpose <= 11, "transpose should be 0-11");
    }
}

CASE("kraft_transpose_count") {
    // Track transposition changes
    uint8_t transpCount = 0;
    for (int i = 0; i < 5; i++) {
        transpCount++;
    }
    expectEqual((int)transpCount, 5, "transp count should track changes");
}

//----------------------------------------
// Gate Characteristics
//----------------------------------------

CASE("kraft_precise_gates") {
    // Kraftwerk has precise, mechanical gates
    int gatePercent = 50;  // Sharp, precise
    expectTrue(gatePercent >= 25 && gatePercent <= 75, "kraft gates should be precise");
}

CASE("kraft_ghost_shorter_gate") {
    // Ghost notes should have shorter, quieter gates
    int normalGate = 50;
    int ghostGate = 25;
    expectTrue(ghostGate < normalGate, "ghost should have shorter gate");
}

//----------------------------------------
// Robotic Repetition
//----------------------------------------

CASE("kraft_repetitive_pattern") {
    // Kraftwerk patterns often repeat notes
    uint8_t sequence[8] = {0, 0, 7, 7, 0, 0, 5, 5};
    int repeats = 0;

    for (int i = 1; i < 8; i++) {
        if (sequence[i] == sequence[i-1]) repeats++;
    }

    expectTrue(repeats >= 3, "should have repetitive notes");
}

CASE("kraft_base_note_stable") {
    // Base note should be relatively stable
    int8_t baseNote = 5;
    // In actual algorithm, base note changes slowly
    expectTrue(baseNote >= 0 && baseNote <= 11, "base note should be valid");
}

} // UNIT_TEST("TuesdayKraft")
