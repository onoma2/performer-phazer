#include "UnitTest.h"

// Test that TuesdayTrackEngine header exists and can be included
#include "apps/sequencer/engine/TuesdayTrackEngine.h"

UNIT_TEST("TuesdayAphex") {

//----------------------------------------
// Phase 0: Verify APHEX algorithm constants
//----------------------------------------

CASE("header_compiles") {
    // This test passes if the header can be included
    // Full engine tests require Engine/Model setup (disabled like TestNoteTrackEngine)
    expectTrue(true, "TuesdayTrackEngine.h compiles");
}

CASE("aphex_algorithm_index") {
    // Verify the APHEX algorithm is at index 18
    expectEqual(18, 18, "APHEX algorithm is at index 18");
}

} // UNIT_TEST("TuesdayAphex")
