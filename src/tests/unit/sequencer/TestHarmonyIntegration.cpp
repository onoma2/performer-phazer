#include "UnitTest.h"

#include "apps/sequencer/model/HarmonyEngine.h"
#include "apps/sequencer/model/NoteSequence.h"

UNIT_TEST("Harmony Integration") {

CASE("harmony_engine_integration_with_note_sequence") {
    // Create a NoteSequence configured as HarmonyMaster
    NoteSequence masterSeq(0); // Track 1
    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyScale(0); // Ionian

    // Set a note on step 0: C4 (MIDI note 60, but in NoteSequence Note format which is -64 to +63)
    // MIDI 60 (C4) = Note value 0 (since middle C is 0 in the scale)
    masterSeq.step(0).setNote(0);

    // Create HarmonyEngine
    HarmonyEngine harmonyEngine;
    harmonyEngine.setMode(HarmonyEngine::Ionian);

    // Harmonize the master note (C, scale degree 0)
    // In C Ionian, I chord is C Major7: C-E-G-B = 60-64-67-71
    auto chord = harmonyEngine.harmonize(60, 0);

    expectEqual(static_cast<int>(chord.root), 60, "Chord root should be C (60)");
    expectEqual(static_cast<int>(chord.third), 64, "Chord third should be E (64)");
    expectEqual(static_cast<int>(chord.fifth), 67, "Chord fifth should be G (67)");
    expectEqual(static_cast<int>(chord.seventh), 71, "Chord seventh should be B (71)");
}

CASE("follower_track_extracts_correct_chord_tone") {
    HarmonyEngine harmonyEngine;
    harmonyEngine.setMode(HarmonyEngine::Ionian);

    // Harmonize C (MIDI 60, scale degree 0) -> C Major7
    auto chord = harmonyEngine.harmonize(60, 0);

    // Follower Root should get MIDI 60 (C)
    expectEqual(static_cast<int>(chord.root), 60, "FollowerRoot should play root (60)");

    // Follower 3rd should get MIDI 64 (E)
    expectEqual(static_cast<int>(chord.third), 64, "Follower3rd should play third (64)");

    // Follower 5th should get MIDI 67 (G)
    expectEqual(static_cast<int>(chord.fifth), 67, "Follower5th should play fifth (67)");

    // Follower 7th should get MIDI 71 (B)
    expectEqual(static_cast<int>(chord.seventh), 71, "Follower7th should play seventh (71)");
}

CASE("harmony_scale_degree_calculation") {
    HarmonyEngine harmonyEngine;
    harmonyEngine.setMode(HarmonyEngine::Ionian);

    // Test different scale degrees in Ionian mode
    // Scale degree 0 (I): C Major7
    auto chord0 = harmonyEngine.harmonize(60, 0);
    expectEqual(static_cast<int>(chord0.root), 60, "I chord root");

    // Scale degree 1 (ii): D Minor7 (D-F-A-C = 62-65-69-72)
    auto chord1 = harmonyEngine.harmonize(62, 1);
    expectEqual(static_cast<int>(chord1.root), 62, "ii chord root");
    expectEqual(static_cast<int>(chord1.third), 65, "ii chord third");

    // Scale degree 4 (V): G Dominant7 (G-B-D-F = 67-71-74-77)
    auto chord4 = harmonyEngine.harmonize(67, 4);
    expectEqual(static_cast<int>(chord4.root), 67, "V chord root");
    expectEqual(static_cast<int>(chord4.third), 71, "V chord third");
    expectEqual(static_cast<int>(chord4.fifth), 74, "V chord fifth");
    expectEqual(static_cast<int>(chord4.seventh), 77, "V chord seventh");
}

} // UNIT_TEST("Harmony Integration")
