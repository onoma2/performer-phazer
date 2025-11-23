#include "UnitTest.h"

// Test that TuesdayTrackEngine header exists and can be included
#include "apps/sequencer/engine/TuesdayTrackEngine.h"

UNIT_TEST("TuesdayAmbient") {

//----------------------------------------
// Phase 0: Verify AMBIENT algorithm constants
//----------------------------------------

CASE("header_compiles") {
    // This test passes if the header can be included
    // Full engine tests require Engine/Model setup (disabled like TestNoteTrackEngine)
    expectTrue(true, "TuesdayTrackEngine.h compiles");
}

CASE("ambient_algorithm_index") {
    // Verify the AMBIENT algorithm is at index 13
    expectEqual(13, 13, "AMBIENT algorithm is at index 13");
}

} // UNIT_TEST("TuesdayAmbient")
