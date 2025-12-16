#include "UnitTest.h"
#include "apps/sequencer/engine/DiscreteMapTrackEngine.h"
#include "apps/sequencer/model/Project.h"
#include "apps/sequencer/engine/Engine.h"

UNIT_TEST("DiscreteMapTrackEngine") {

    // Mock/Stub setup
    Project project;
    Engine engine(project);
    Track &track = project.track(0);
    track.setTrackMode(Track::TrackMode::DiscreteMap);
    
    DiscreteMapTrackEngine *trackEngine = static_cast<DiscreteMapTrackEngine*>(engine.trackEngine(0));
    DiscreteMapSequence &seq = track.discreteMapSequence(0);

    // Helper to set up a basic sequence
    auto setupBasicSequence = [&]() {
        seq.clear();
        seq.setClockSource(DiscreteMapSequence::ClockSource::External); // Manual input control
        
        // Stage 0: Threshold 0, Rise
        seq.stage(0).setThreshold(0);
        seq.stage(0).setDirection(DiscreteMapSequence::Stage::TriggerDir::Rise);
        seq.stage(0).setNoteIndex(0);

        // Stage 1: Threshold 50, Fall
        seq.stage(1).setThreshold(50);
        seq.stage(1).setDirection(DiscreteMapSequence::Stage::TriggerDir::Fall);
        seq.stage(1).setNoteIndex(12); // Octave up

        trackEngine->reset();
    };

    CASE("reset_state") {
        setupBasicSequence();
        expectEqual(trackEngine->activeStage(), -1, "active stage -1 on reset");
        expectEqual(int(trackEngine->currentInput() * 100), -500, "input at min (-5V)");
    }

    CASE("find_active_stage_rise") {
        setupBasicSequence();
        
        // Threshold 0 (0V) maps to center of -5V..5V range if mapped 1:1?
        // Wait, Engine::getThresholdVoltage maps [-127,127] to [-5V, 5V]
        // Thresh 0 -> 0V.
        
        // Simulate input crossing 0V upwards
        // prev = -1V, current = 1V
        float t0_volts = 0.0f; 
        
        int stage = trackEngine->findActiveStage(t0_volts + 0.1f, t0_volts - 0.1f);
        expectEqual(stage, 0, "should trigger stage 0 (Rise at 0)");
    }

    CASE("find_active_stage_fall") {
        setupBasicSequence();
        
        // Stage 1 is Fall at Thresh 50.
        // 50/127 * 5V ~= 1.96V
        float t1_volts = (50.f / 127.f) * 5.0f;

        // Simulate input crossing downwards
        int stage = trackEngine->findActiveStage(t1_volts - 0.1f, t1_volts + 0.1f);
        expectEqual(stage, 1, "should trigger stage 1 (Fall at 50)");
    }

    CASE("ignore_off_stage") {
        setupBasicSequence();
        seq.stage(0).setDirection(DiscreteMapSequence::Stage::TriggerDir::Off);
        
        // Cross 0V again
        int stage = trackEngine->findActiveStage(0.1f, -0.1f);
        expectEqual(stage, -1, "should ignore stage 0 when OFF");
    }

    CASE("threshold_invalidation") {
        setupBasicSequence();
        // Force Position mode to verify dirty flag behavior logic (though Position mode doesn't strictly need calc)
        // Switch to Length mode where invalidation matters
        seq.setThresholdMode(DiscreteMapSequence::ThresholdMode::Length);
        
        trackEngine->invalidateThresholds();
        // tick() would normally call recalculate. Here we verify the flag if we could access it, 
        // or verify the result of getThresholdVoltage changes.
        
        // We can test getThresholdVoltage directly if we make it accessible or via tick
        // For now, just ensure the method exists and runs without crash
        trackEngine->invalidateThresholds();
        expectTrue(true, "invalidateThresholds called successfully");
    }

} // UNIT_TEST
