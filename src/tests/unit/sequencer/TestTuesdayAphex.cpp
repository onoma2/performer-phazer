#include "UnitTest.h"
#include "TestGlobals.h"
#include "apps/sequencer/model/Model.h"
#include "apps/sequencer/engine/TuesdayTrackEngine.h"
#include "core/utils/Random.h"

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

UNIT_TEST("TuesdayAphex") {

    TestEngine testEngine;

    CASE("aphex_initialization") {
        auto engine = create_engine(testEngine, 18, 1, 1);
        engine.initAlgorithm();

        // Verify pattern generation (deterministic from seed)
        Random rng(0); // Flow=1 -> seed=0
        expectEqual((int)engine._aphex_track1_pattern[0], (int)(rng.next() % 12), "track1[0] should match");
        expectEqual((int)engine._aphex_track1_pattern[1], (int)(rng.next() % 12), "track1[1] should match");
        
        // Verify phase initialization from ornament
        expectEqual((int)engine._aphex_pos1, 1, "pos1 should be 1");
        expectEqual((int)engine._aphex_pos2, 2, "pos2 should be 2");
        expectEqual((int)engine._aphex_pos3, 3, "pos3 should be 3");
    }

    CASE("aphex_pattern_generation_deterministic") {
        auto engine = create_engine(testEngine, 18, 5, 5);
        engine.initAlgorithm();

        // Check if patterns are generated deterministically
        Random rng((5 - 1) << 4);
        uint8_t t1[4], t2[3], t3[5];
        for (int i = 0; i < 4; ++i) t1[i] = rng.next() % 12;
        for (int i = 0; i < 3; ++i) t2[i] = rng.next() % 3;
        for (int i = 0; i < 5; ++i) t3[i] = (rng.next() % 8 == 0) ? (rng.next() % 5) : 0;

        for (int i = 0; i < 4; ++i) expectEqual((int)engine._aphex_track1_pattern[i], (int)t1[i], "track1 should be deterministic");
        for (int i = 0; i < 3; ++i) expectEqual((int)engine._aphex_track2_pattern[i], (int)t2[i], "track2 should be deterministic");
        for (int i = 0; i < 5; ++i) expectEqual((int)engine._aphex_track3_pattern[i], (int)t3[i], "track3 should be deterministic");
    }

    CASE("aphex_polyrhythm_logic") {
        auto engine = create_engine(testEngine, 18, 1, 1);
        engine.initAlgorithm();
        engine.generateBuffer();

        // Manually set patterns for predictable test
        engine._aphex_track1_pattern[1] = 7; // G
        engine._aphex_track2_pattern[2] = 1; // Stutter
        engine._aphex_track3_pattern[3] = 2; // Bass override D
        engine._aphex_pos1 = 1;
        engine._aphex_pos2 = 2;
        engine._aphex_pos3 = 3;

        engine.generateBuffer(); // Regenerate with new patterns

        // Step 0: pos1=1, pos2=2, pos3=3 -> G, Stutter, Bass Override D
        expectEqual((int)engine._buffer[0].note, 2, "Note should be D from bass override");
        expectEqual((int)engine._buffer[0].octave, -1, "Octave should be -1 from bass");
        expectEqual((int)engine._buffer[0].gatePercent, 90, "Gate should be 90 from bass");
        
        // Step 1: pos1=2, pos2=0, pos3=4 -> pattern[2], no mod, no bass
        engine._aphex_pos1 = 2;
        engine._aphex_pos2 = 0;
        engine._aphex_pos3 = 4;
        engine.generateBuffer();
        expectEqual((int)engine._buffer[1].gatePercent, 75, "Gate should be default 75");

    }

} // UNIT_TEST("TuesdayAphex")
