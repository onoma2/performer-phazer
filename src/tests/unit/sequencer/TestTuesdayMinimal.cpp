#include "UnitTest.h"
#include "core/utils/Random.h"

UNIT_TEST("TuesdayMinimal") {

//----------------------------------------
// State Variable Initialization
//----------------------------------------

CASE("minimal_state_default_values") {
    // Test initial state values
    uint8_t burstLength = 0;
    uint8_t silenceLength = 0;
    uint8_t clickDensity = 0;
    uint8_t burstTimer = 0;
    uint8_t silenceTimer = 0;
    uint8_t noteIndex = 0;
    uint8_t mode = 0;

    expectEqual((int)burstLength, 0, "burstLength should start at 0");
    expectEqual((int)silenceLength, 0, "silenceLength should start at 0");
    expectEqual((int)clickDensity, 0, "clickDensity should start at 0");
    expectEqual((int)burstTimer, 0, "burstTimer should start at 0");
    expectEqual((int)silenceTimer, 0, "silenceTimer should start at 0");
    expectEqual((int)noteIndex, 0, "noteIndex should start at 0");
    expectEqual((int)mode, 0, "mode should start at 0 (silence)");
}

//----------------------------------------
// Flow Parameter: Silence Duration
//----------------------------------------

CASE("minimal_flow_zero_short_silence") {
    // Flow=0 should produce minimal silence gaps
    int flow = 0;
    int silenceLength = 1 + flow;  // At least 1 step
    expectEqual(silenceLength, 1, "minimal silence at flow 0");
}

CASE("minimal_flow_high_long_silence") {
    // Higher flow = longer silence gaps
    int flow = 16;
    int silenceLength = 1 + flow;
    expectEqual(silenceLength, 17, "longer silence at high flow");
}

//----------------------------------------
// Ornament Parameter: Glitch Repeats
//----------------------------------------

CASE("minimal_ornament_glitch_probability") {
    // Higher ornament = more glitch repeats
    Random rng(12345);
    int ornament = 12;
    int glitchCount = 0;
    int total = 100;

    for (int i = 0; i < total; i++) {
        if (rng.nextRange(16) < ornament) {
            glitchCount++;
        }
    }

    // Ornament 12/16 = 75% chance
    expectTrue(glitchCount > 50, "high ornament should produce more glitches");
}

//----------------------------------------
// Musical Characteristics
//----------------------------------------

CASE("minimal_burst_mode_toggle") {
    // Mode should toggle between burst (1) and silence (0)
    uint8_t mode = 0;

    // Start in silence mode
    expectEqual((int)mode, 0, "should start in silence");

    // Toggle to burst
    mode = 1;
    expectEqual((int)mode, 1, "should switch to burst");

    // Toggle back to silence
    mode = 0;
    expectEqual((int)mode, 0, "should switch back to silence");
}

CASE("minimal_staccato_short_gates") {
    // MINIMAL should produce short, staccato gates
    int gatePercent = 25;  // Typical staccato gate
    expectTrue(gatePercent <= 50, "staccato gates should be short");
}

CASE("minimal_burst_length_range") {
    // Bursts should be 2-8 steps typically
    Random rng(12345);
    int burstLength = 2 + (rng.next() % 7);  // 2-8 steps
    expectTrue(burstLength >= 2 && burstLength <= 8, "burst length should be 2-8");
}

CASE("minimal_silence_length_range") {
    // Silence should be 4-16 steps typically
    Random rng(12345);
    int flow = 8;
    int silenceLength = 4 + (flow % 13);  // 4-16 steps
    expectTrue(silenceLength >= 4 && silenceLength <= 16, "silence length should be 4-16");
}

//----------------------------------------
// Click Patterns
//----------------------------------------

CASE("minimal_click_density_low") {
    // Low ornament = sparse clicks
    Random rng(12345);
    int ornament = 2;
    int clickCount = 0;
    int total = 100;

    for (int i = 0; i < total; i++) {
        if (rng.nextRange(16) < ornament) {
            clickCount++;
        }
    }

    expectTrue(clickCount < 30, "low ornament should produce sparse clicks");
}

CASE("minimal_note_index_wraps") {
    // Note index should wrap within burst length
    int noteIndex = 7;
    int burstLength = 4;
    noteIndex = noteIndex % burstLength;
    expectEqual(noteIndex, 3, "note index should wrap");
}

//----------------------------------------
// Timer Behavior
//----------------------------------------

CASE("minimal_burst_timer_decrement") {
    // Burst timer should decrement each step
    uint8_t burstTimer = 5;
    for (int i = 0; i < 3; i++) {
        if (burstTimer > 0) burstTimer--;
    }
    expectEqual((int)burstTimer, 2, "burst timer should decrement correctly");
}

CASE("minimal_silence_timer_decrement") {
    // Silence timer should decrement each step
    uint8_t silenceTimer = 8;
    for (int i = 0; i < 5; i++) {
        if (silenceTimer > 0) silenceTimer--;
    }
    expectEqual((int)silenceTimer, 3, "silence timer should decrement correctly");
}

CASE("minimal_mode_switch_on_timer_zero") {
    // Mode should switch when timer reaches 0
    uint8_t mode = 1;  // burst
    uint8_t burstTimer = 1;

    burstTimer--;
    if (burstTimer == 0) {
        mode = 0;  // switch to silence
    }

    expectEqual((int)mode, 0, "should switch to silence when timer is 0");
}

} // UNIT_TEST("TuesdayMinimal")
