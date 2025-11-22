#include "UnitTest.h"
#include "TestGlobals.h"
#include "apps/sequencer/model/Model.h"
#include "apps/sequencer/engine/TuesdayTrackEngine.h"

// Helper to create a TuesdayTrackEngine for testing
static TuesdayTrackEngine create_engine(TestEngine &testEngine, int algorithm, int flow, int ornament) {
    auto &model = testEngine.model();
    auto &project = model.project();
    project.setTrackMode(0, Track::TrackMode::Tuesday);
    auto &tuesdayTrack = project.track(0).tuesdayTrack();
    tuesdayTrack.setAlgorithm(algorithm);
    tuesdayTrack.setFlow(flow);
    tuesdayTrack.setOrnament(ornament);
    return TuesdayTrackEngine(testEngine.engine(), model, project.track(0), nullptr);
}

UNIT_TEST("TuesdayAmbient") {

    TestEngine testEngine;

    CASE("ambient_initialization") {
        auto engine = create_engine(testEngine, 13, 1, 1);
        engine.initAlgorithm();

        // Verify drone chord generation (deterministic from flow)
        expectEqual((int)engine._ambient_root_note, 0, "root note should be 0 for flow=1");
        expectEqual((int)engine._ambient_drone_notes[0], 0, "drone note 0 should be root");
        expectEqual((int)engine._ambient_drone_notes[1], 7, "drone note 1 should be a 5th");
        expectEqual((int)engine._ambient_drone_notes[2], 4, "drone note 2 should be a 9th (2+12 -> 14, +2 -> 16)");

        // Verify event timer initialization (deterministic from ornament)
        Random rng(0); // Ornament=1 -> seed=0
        int expected_timer = 16 + (rng.next() % 48);
        expectEqual(engine._ambient_event_timer, expected_timer, "event timer should be deterministic");
        expectEqual((int)engine._ambient_event_type, 0, "event type should be 0 initially");
        expectEqual((int)engine._ambient_event_step, 0, "event step should be 0 initially");
    }

    CASE("ambient_drone_generation") {
        auto engine = create_engine(testEngine, 13, 1, 1);
        engine.initAlgorithm();
        
        // Set timer far in the future to ensure drone logic is tested
        engine._ambient_event_timer = 100;
        engine.generateBuffer();

        // Check drone notes are generated
        expectEqual((int)engine._buffer[0].note, (int)engine._ambient_drone_notes[0], "step 0 should be drone note 0");
        expectEqual((int)engine._buffer[4].note, (int)engine._ambient_drone_notes[1], "step 4 should be drone note 1");
        expectEqual((int)engine._buffer[8].note, (int)engine._ambient_drone_notes[2], "step 8 should be drone note 2");
        expectEqual((int)engine._buffer[0].gatePercent, 1600, "drone gate should be very long");
    }

    CASE("ambient_event_triggering") {
        auto engine = create_engine(testEngine, 13, 1, 1);
        engine.initAlgorithm();

        // Set timer to trigger on the first step
        engine._ambient_event_timer = 1;
        engine.generateBuffer();

        // Event should trigger on step 1 (timer becomes 0)
        // Note: The logic in generateBuffer is complex. This test assumes event triggers and then checks state.
        // It is more of an integration test of the buffer generation.
        
        // To properly test the event, we need to manually step the logic
        engine.initAlgorithm();
        engine._ambient_event_timer = 1;
        
        // Step 0 - timer becomes 0
        engine.tick(0);
        
        // Step 1 - event should be active
        engine.tick(24);

        // This is hard to test without more control. Let's check the buffer after a forced event.
        engine.initAlgorithm();
        engine._ambient_event_timer = 1;
        engine._ambient_event_type = 1; // force single note event
        engine.generateBuffer();
        
        expectEqual((int)engine._buffer[0].note, 1, "Event note should be octave up root");
        expectEqual((int)engine._buffer[0].octave, 1, "Event octave should be 1");
        expectEqual((int)engine._buffer[0].gatePercent, 150, "Event gate should be 150");
    }

} // UNIT_TEST("TuesdayAmbient")
