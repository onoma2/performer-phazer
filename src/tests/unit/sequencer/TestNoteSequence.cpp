#include "UnitTest.h"

#include "apps/sequencer/model/NoteSequence.h"

UNIT_TEST("NoteSequence") {

CASE("step_is_accumulator_trigger") {
    NoteSequence::Step step;
    step.setAccumulatorTrigger(true);
    expectTrue(step.isAccumulatorTrigger(), "isAccumulatorTrigger should be true");
    step.setAccumulatorTrigger(false);
    expectFalse(step.isAccumulatorTrigger(), "isAccumulatorTrigger should be false");
    step.toggleAccumulatorTrigger();
    expectTrue(step.isAccumulatorTrigger(), "isAccumulatorTrigger should be true after toggle");
}

CASE("note_sequence_has_accumulator") {
    NoteSequence noteSequence;
    Accumulator &accumulator = noteSequence.accumulator();
    accumulator.setEnabled(true);
    expectTrue(noteSequence.accumulator().enabled(), "accumulator should be enabled");
}

CASE("harmony_track_position_constraints") {
    // Track 1 (index 0) should be able to be HarmonyMaster
    NoteSequence seq1(0);
    expectTrue(seq1.canBeHarmonyMaster(), "Track 1 should be able to be HarmonyMaster");

    seq1.setHarmonyRole(NoteSequence::HarmonyMaster);
    expectEqual(static_cast<int>(seq1.harmonyRole()), static_cast<int>(NoteSequence::HarmonyMaster), "Track 1 should be HarmonyMaster");

    // Track 2 (index 1) should NOT be able to be HarmonyMaster
    NoteSequence seq2(1);
    expectFalse(seq2.canBeHarmonyMaster(), "Track 2 should NOT be able to be HarmonyMaster");

    seq2.setHarmonyRole(NoteSequence::HarmonyMaster);
    expectTrue(static_cast<int>(seq2.harmonyRole()) != static_cast<int>(NoteSequence::HarmonyMaster), "Track 2 should auto-revert from HarmonyMaster");
    expectEqual(static_cast<int>(seq2.harmonyRole()), static_cast<int>(NoteSequence::HarmonyFollower3rd), "Track 2 should auto-assign to HarmonyFollower3rd");

    // Track 5 (index 4) should be able to be HarmonyMaster
    NoteSequence seq5(4);
    expectTrue(seq5.canBeHarmonyMaster(), "Track 5 should be able to be HarmonyMaster");

    seq5.setHarmonyRole(NoteSequence::HarmonyMaster);
    expectEqual(static_cast<int>(seq5.harmonyRole()), static_cast<int>(NoteSequence::HarmonyMaster), "Track 5 should be HarmonyMaster");
}

CASE("harmony_properties") {
    NoteSequence seq(0);

    // Test default values
    expectEqual(static_cast<int>(seq.harmonyRole()), static_cast<int>(NoteSequence::HarmonyOff), "default harmonyRole should be HarmonyOff");
    expectEqual(seq.masterTrackIndex(), 0, "default masterTrackIndex should be 0");
    expectEqual(seq.harmonyScale(), 0, "default harmonyScale should be 0");

    // Test harmonyScale setter and getter
    seq.setHarmonyScale(3); // Lydian mode
    expectEqual(seq.harmonyScale(), 3, "harmonyScale should be 3 after setting");

    // Test clamping (0-6 for 7 modes)
    seq.setHarmonyScale(10); // Should clamp to 6
    expectEqual(seq.harmonyScale(), 6, "harmonyScale should clamp to 6");

    seq.setHarmonyScale(-5); // Should clamp to 0
    expectEqual(seq.harmonyScale(), 0, "harmonyScale should clamp to 0");

    // Test masterTrackIndex setter and getter
    seq.setMasterTrackIndex(4); // Track 5
    expectEqual(seq.masterTrackIndex(), 4, "masterTrackIndex should be 4 after setting");

    // Test clamping (0-7 for 8 tracks)
    seq.setMasterTrackIndex(12); // Should clamp to 7
    expectEqual(seq.masterTrackIndex(), 7, "masterTrackIndex should clamp to 7");

    seq.setMasterTrackIndex(-3); // Should clamp to 0
    expectEqual(seq.masterTrackIndex(), 0, "masterTrackIndex should clamp to 0");

    // Test all HarmonyRole values
    seq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    expectEqual(static_cast<int>(seq.harmonyRole()), static_cast<int>(NoteSequence::HarmonyFollowerRoot), "harmonyRole should be HarmonyFollowerRoot");

    seq.setHarmonyRole(NoteSequence::HarmonyFollower5th);
    expectEqual(static_cast<int>(seq.harmonyRole()), static_cast<int>(NoteSequence::HarmonyFollower5th), "harmonyRole should be HarmonyFollower5th");

    seq.setHarmonyRole(NoteSequence::HarmonyFollower7th);
    expectEqual(static_cast<int>(seq.harmonyRole()), static_cast<int>(NoteSequence::HarmonyFollower7th), "harmonyRole should be HarmonyFollower7th");
}

} // UNIT_TEST("NoteSequence")
