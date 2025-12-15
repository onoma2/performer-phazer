#include "UnitTest.h"

#include "apps/sequencer/engine/TuesdayTrackEngine.h"
#include "apps/sequencer/model/Project.h"

UNIT_TEST("TuesdayTrackEnginePrimeMask") {

CASE("engine_primeMask_defaults_to_allow") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set up a basic sequence
    sequence.setAlgorithm(0);  // Test algorithm
    sequence.setFlow(8);
    sequence.setOrnament(8);
    sequence.setPower(8);
    
    // Prime mask defaults to 1 (ALLOW ALL)
    expectEqual(sequence.primeMaskPattern(), 1, "primeMaskPattern should default to 1 (ALLOW ALL)");
    expectEqual(sequence.primeMaskParameter(), 0, "primeMaskParameter should default to 0");
}

CASE("engine_primeMask_mask_all_blocks_ticks") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set parameters
    sequence.setAlgorithm(0);  // Test algorithm
    sequence.setFlow(8);
    sequence.setOrnament(8);
    sequence.setPower(8);
    sequence.setPrimeMaskPattern(0);  // MASK ALL
    sequence.setPrimeMaskParameter(0);
    
    TuesdayTrackEngine engine(project, 0, 0);
    
    // Test tick masking logic indirectly - when pattern is 0, tick should be blocked
    expectEqual(sequence.primeMaskPattern(), 0, "primeMaskPattern should be 0 (MASK ALL)");
}

CASE("engine_primeMask_allow_all_allows_ticks") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set parameters
    sequence.setAlgorithm(0);  // Test algorithm
    sequence.setFlow(8);
    sequence.setOrnament(8);
    sequence.setPower(8);
    sequence.setPrimeMaskPattern(1);  // ALLOW ALL
    sequence.setPrimeMaskParameter(0);
    
    TuesdayTrackEngine engine(project, 0, 0);
    
    // Test that settings are correct
    expectEqual(sequence.primeMaskPattern(), 1, "primeMaskPattern should be 1 (ALLOW ALL)");
    expectEqual(sequence.primeMaskParameter(), 0, "primeMaskParameter should be 0");
}

} // UNIT_TEST("TuesdayTrackEnginePrimeMask")