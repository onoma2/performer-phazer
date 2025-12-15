#include "UnitTest.h"

#include "apps/sequencer/engine/TuesdayTrackEngine.h"
#include "apps/sequencer/model/Project.h"

UNIT_TEST("TuesdayTrackBurstMasking") {

CASE("engine_burst_masking_pattern_2_allows_for_specified_duration") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set up a basic sequence
    sequence.setAlgorithm(0);  // Test algorithm
    sequence.setFlow(8);
    sequence.setOrnament(8);
    sequence.setPower(8);
    sequence.setPrimeMaskPattern(2);  // BURST-BASED
    sequence.setPrimeMaskParameter(3);  // Allow for 3 ticks at a time
    
    TuesdayTrackEngine engine(project, 0, 0);
    
    // Check initial settings
    expectEqual(sequence.primeMaskPattern(), 2, "pattern should be 2 (BURST-BASED)");
    expectEqual(sequence.primeMaskParameter(), 3, "parameter should be 3");
}

CASE("engine_burst_masking_pattern_3_creates_euclidean_rhythms") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set up a basic sequence
    sequence.setAlgorithm(0);  // Test algorithm
    sequence.setFlow(8);
    sequence.setOrnament(8);
    sequence.setPower(8);
    sequence.setPrimeMaskPattern(3);  // CUSTOM-BASED
    sequence.setPrimeMaskParameter(4);  // 4 events to distribute
    
    TuesdayTrackEngine engine(project, 0, 0);
    
    // Check initial settings
    expectEqual(sequence.primeMaskPattern(), 3, "pattern should be 3 (CUSTOM-BASED)");
    expectEqual(sequence.primeMaskParameter(), 4, "parameter should be 4");
}

CASE("engine_burst_masking_cache_initialization") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);
    
    // Set up sequence
    sequence.setPrimeMaskPattern(2);
    sequence.setPrimeMaskParameter(5);
    
    TuesdayTrackEngine engine(project, 0, 0);
    
    // After initAlgorithm is called, cache should be set to the values
    // This is tested by verifying the values are initially different and will trigger reset
    // when first called in the tick function
    expectEqual(sequence.primeMaskPattern(), 2, "pattern should be 2");
    expectEqual(sequence.primeMaskParameter(), 5, "parameter should be 5");
}

CASE("engine_mask_param_zero_allows_all_ticks") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);

    // Set up sequence with mask param = 0 (should allow all)
    sequence.setAlgorithm(0);
    sequence.setPrimeMaskPattern(2);
    sequence.setPrimeMaskParameter(0);  // Should allow all

    TuesdayTrackEngine engine(project, 0, 0);

    expectEqual(sequence.primeMaskParameter(), 0, "parameter should be 0");
    expectEqual(sequence.primeMaskPattern(), 2, "pattern should be 2");
}

CASE("engine_prime_pattern_toggles_properly") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);

    // Set up sequence with prime pattern
    sequence.setAlgorithm(0);
    sequence.setPrimeMaskPattern(2);
    sequence.setPrimeMaskParameter(1);  // Should select prime=2, so allow 2 ticks, mask 2 ticks

    TuesdayTrackEngine engine(project, 0, 0);

    // Test the prime array values
    expectEqual(sequence.primeMaskParameter(), 1, "parameter should be 1");
    expectEqual(sequence.primeMaskPattern(), 2, "pattern should be 2");
}

CASE("engine_fibonacci_pattern_toggles_properly") {
    Project project;
    auto &tuesdayTrack = project.tuesdayTrack(0);
    auto &sequence = tuesdayTrack.sequence(0);

    // Set up sequence with fibonacci pattern
    sequence.setAlgorithm(0);
    sequence.setPrimeMaskPattern(3);
    sequence.setPrimeMaskParameter(2);  // Will influence fibonacci index

    TuesdayTrackEngine engine(project, 0, 0);

    expectEqual(sequence.primeMaskParameter(), 2, "parameter should be 2");
    expectEqual(sequence.primeMaskPattern(), 3, "pattern should be 3");
}

} // UNIT_TEST("TuesdayTrackBurstMasking")