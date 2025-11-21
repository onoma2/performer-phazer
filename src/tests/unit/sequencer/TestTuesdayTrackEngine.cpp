#include "UnitTest.h"

// Test that TuesdayTrackEngine header exists and can be included
#include "apps/sequencer/engine/TuesdayTrackEngine.h"

UNIT_TEST("TuesdayTrackEngine") {

//----------------------------------------
// Phase 0: Verify class exists
//----------------------------------------

CASE("header_compiles") {
    // This test passes if the header can be included
    // Full engine tests require Engine/Model setup (disabled like TestNoteTrackEngine)
    expectTrue(true, "TuesdayTrackEngine.h compiles");
}

CASE("track_mode_constant") {
    // Verify the track mode is correctly defined
    // Can't instantiate engine without full setup, but can check the type
    expectEqual(static_cast<int>(Track::TrackMode::Tuesday), 3, "Tuesday mode is 3");
}

} // UNIT_TEST("TuesdayTrackEngine")
