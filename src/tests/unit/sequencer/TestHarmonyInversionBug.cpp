#include "UnitTest.h"

#include "apps/sequencer/model/HarmonyEngine.h"
#include "apps/sequencer/model/NoteSequence.h"

// =============================================================================
// BUG HYPOTHESIS: evalStepNote() reads sequence-level inversion from the WRONG
// sequence (follower instead of master)
//
// In NoteTrackEngine.cpp lines 118-119:
//   int inversion = (inversionValue == 0) ? sequence.harmonyInversion() : (inversionValue - 1);
//   int voicing = (voicingValue == 0) ? sequence.harmonyVoicing() : (voicingValue - 1);
//
// Here 'sequence' is the FOLLOWER's NoteSequence, but it should be reading
// from 'masterSequence' for the fallback case.
//
// This explains why:
// - Per-step inversion works (reads from masterStep.inversionOverride())
// - Sequence-level inversion doesn't work (reads from wrong sequence)
// =============================================================================

UNIT_TEST("Harmony Inversion Bug - Master vs Follower Sequence") {

// =============================================================================
// SECTION 1: Demonstrate the Bug Logic
// =============================================================================

CASE("bug_demonstration_reading_from_wrong_sequence") {
    // Set up master and follower sequences
    NoteSequence masterSeq(0);  // Track 1 - the master
    NoteSequence followerSeq(1); // Track 2 - the follower

    // Configure master
    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(2); // Master has 2nd inversion

    // Configure follower
    followerSeq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    followerSeq.setMasterTrackIndex(0);
    followerSeq.setHarmonyInversion(0); // Follower has root position (default)

    // Verify setup
    expectEqual(masterSeq.harmonyInversion(), 2, "master should have inversion 2");
    expectEqual(followerSeq.harmonyInversion(), 0, "follower should have inversion 0");

    // Simulate the CURRENT (buggy) code logic from evalStepNote():
    // int inversionValue = masterStep.inversionOverride(); // = 0 (no override)
    // int inversion = (inversionValue == 0) ? sequence.harmonyInversion() : (inversionValue - 1);
    //                                         ^^^^^^^^
    //                                         BUG: 'sequence' is the follower!

    int inversionValue = masterSeq.step(0).inversionOverride(); // = 0

    // BUGGY CODE: reads from follower (sequence parameter in evalStepNote)
    int buggyInversion = (inversionValue == 0) ? followerSeq.harmonyInversion() : (inversionValue - 1);

    // CORRECT CODE: should read from master
    int correctInversion = (inversionValue == 0) ? masterSeq.harmonyInversion() : (inversionValue - 1);

    // This test DEMONSTRATES the bug by showing the two values differ
    expectEqual(buggyInversion, 0, "buggy code returns follower's inversion (0)");
    expectEqual(correctInversion, 2, "correct code returns master's inversion (2)");

    // The bug causes us to use 0 instead of 2
    expectTrue(buggyInversion != correctInversion, "BUG: buggy and correct values differ!");
}

CASE("bug_only_affects_sequence_level_not_per_step") {
    // Set up master and follower
    NoteSequence masterSeq(0);
    NoteSequence followerSeq(1);

    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(3); // Master sequence-level = 3rd inversion
    masterSeq.step(0).setInversionOverride(2); // Master per-step override = 1st inversion

    followerSeq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    followerSeq.setHarmonyInversion(0); // Follower's inversion (should be ignored)

    // Test 1: Per-step override is set (value 2 = 1st inversion)
    int inversionValue = masterSeq.step(0).inversionOverride(); // = 2
    expectEqual(inversionValue, 2, "per-step override should be 2");

    // Both buggy and correct code work the same when override is set
    int buggyInversion = (inversionValue == 0) ? followerSeq.harmonyInversion() : (inversionValue - 1);
    int correctInversion = (inversionValue == 0) ? masterSeq.harmonyInversion() : (inversionValue - 1);

    expectEqual(buggyInversion, 1, "buggy code: per-step works (1)");
    expectEqual(correctInversion, 1, "correct code: per-step works (1)");
    expectEqual(buggyInversion, correctInversion, "per-step override: both return same value");

    // Test 2: Per-step override is 0 (use sequence-level)
    int noOverride = masterSeq.step(1).inversionOverride(); // = 0
    expectEqual(noOverride, 0, "step 1 has no override");

    int buggyFallback = (noOverride == 0) ? followerSeq.harmonyInversion() : (noOverride - 1);
    int correctFallback = (noOverride == 0) ? masterSeq.harmonyInversion() : (noOverride - 1);

    expectEqual(buggyFallback, 0, "buggy code: fallback returns follower's 0");
    expectEqual(correctFallback, 3, "correct code: fallback returns master's 3");
    expectTrue(buggyFallback != correctFallback, "BUG: fallback values differ!");
}

// =============================================================================
// SECTION 2: Test the Correct Behavior Expected
// =============================================================================

CASE("correct_behavior_master_inversion_affects_all_followers") {
    // Master sets inversion, all followers should use it
    NoteSequence masterSeq(0);
    NoteSequence follower1(1);
    NoteSequence follower2(2);
    NoteSequence follower3(3);

    // Configure master with 2nd inversion
    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(2);

    // Configure followers (each with different local inversion that should be IGNORED)
    follower1.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    follower1.setMasterTrackIndex(0);
    follower1.setHarmonyInversion(0); // Should be ignored

    follower2.setHarmonyRole(NoteSequence::HarmonyFollower3rd);
    follower2.setMasterTrackIndex(0);
    follower2.setHarmonyInversion(1); // Should be ignored

    follower3.setHarmonyRole(NoteSequence::HarmonyFollower5th);
    follower3.setMasterTrackIndex(0);
    follower3.setHarmonyInversion(3); // Should be ignored

    // CORRECT behavior: all followers should use master's inversion
    int expectedInversion = masterSeq.harmonyInversion();
    expectEqual(expectedInversion, 2, "all followers should use master's inversion (2)");

    // The harmonization should use this inversion for ALL followers
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Ionian);
    engine.setInversion(expectedInversion);

    auto chord = engine.harmonize(60, 0);

    // 2nd inversion: G(67)-B(71)-C(72)-E(76)
    expectEqual(static_cast<int>(chord.root), 72, "2nd inv: root at 72");
    expectEqual(static_cast<int>(chord.third), 76, "2nd inv: third at 76");
    expectEqual(static_cast<int>(chord.fifth), 67, "2nd inv: fifth at 67");
}

CASE("correct_behavior_per_step_override_from_master_step") {
    NoteSequence masterSeq(0);
    NoteSequence followerSeq(1);

    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(1); // Sequence-level = 1st inversion
    masterSeq.step(5).setInversionOverride(4); // Step 5 override = 3rd inversion

    followerSeq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    followerSeq.setMasterTrackIndex(0);

    // Step 0: no override, use master's sequence-level
    int override0 = masterSeq.step(0).inversionOverride();
    int effective0 = (override0 == 0) ? masterSeq.harmonyInversion() : (override0 - 1);
    expectEqual(effective0, 1, "step 0: should use master seq-level (1)");

    // Step 5: has override, use per-step value
    int override5 = masterSeq.step(5).inversionOverride();
    int effective5 = (override5 == 0) ? masterSeq.harmonyInversion() : (override5 - 1);
    expectEqual(effective5, 3, "step 5: should use per-step override (3)");
}

// =============================================================================
// SECTION 3: Test What the Buggy Code Actually Does
// =============================================================================

CASE("buggy_behavior_ignores_master_sequence_level") {
    NoteSequence masterSeq(0);
    NoteSequence followerSeq(1);

    // Master has rich inversion setup
    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(3); // 3rd inversion

    // Follower has default inversion
    followerSeq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    followerSeq.setHarmonyInversion(0); // Root position

    // When no per-step override, the BUGGY code reads from follower
    int inversionValue = masterSeq.step(0).inversionOverride(); // 0

    // Buggy logic (current code)
    int result = (inversionValue == 0) ? followerSeq.harmonyInversion() : (inversionValue - 1);

    // The bug causes the master's 3rd inversion to be IGNORED
    expectEqual(result, 0, "BUGGY: returns follower's 0, not master's 3");

    // Verify this produces different harmonization
    HarmonyEngine engineBuggy;
    engineBuggy.setMode(HarmonyEngine::Ionian);
    engineBuggy.setInversion(result); // 0 (root position)

    HarmonyEngine engineCorrect;
    engineCorrect.setMode(HarmonyEngine::Ionian);
    engineCorrect.setInversion(masterSeq.harmonyInversion()); // 3 (3rd inversion)

    auto chordBuggy = engineBuggy.harmonize(60, 0);
    auto chordCorrect = engineCorrect.harmonize(60, 0);

    // Root position: C(60)-E(64)-G(67)-B(71)
    expectEqual(static_cast<int>(chordBuggy.root), 60, "buggy: root at 60");
    expectEqual(static_cast<int>(chordBuggy.seventh), 71, "buggy: seventh at 71");

    // 3rd inversion: B(71)-C(72)-E(76)-G(79)
    expectEqual(static_cast<int>(chordCorrect.root), 72, "correct: root at 72");
    expectEqual(static_cast<int>(chordCorrect.seventh), 71, "correct: seventh at 71");

    // These are clearly different
    expectTrue(chordBuggy.root != chordCorrect.root, "root notes differ between buggy and correct");
}

// =============================================================================
// SECTION 4: Same Bug Applies to Voicing
// =============================================================================

CASE("bug_also_affects_voicing") {
    NoteSequence masterSeq(0);
    NoteSequence followerSeq(1);

    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyVoicing(2); // Master: Drop2 voicing

    followerSeq.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    followerSeq.setHarmonyVoicing(0); // Follower: Close voicing

    int voicingValue = masterSeq.step(0).voicingOverride(); // 0

    // Buggy code reads from follower
    int buggyVoicing = (voicingValue == 0) ? followerSeq.harmonyVoicing() : (voicingValue - 1);

    // Correct code reads from master
    int correctVoicing = (voicingValue == 0) ? masterSeq.harmonyVoicing() : (voicingValue - 1);

    expectEqual(buggyVoicing, 0, "buggy: returns follower's voicing (0)");
    expectEqual(correctVoicing, 2, "correct: returns master's voicing (2)");
    expectTrue(buggyVoicing != correctVoicing, "BUG: voicing also affected!");
}

// =============================================================================
// SECTION 5: The Fix
// =============================================================================

CASE("fix_description") {
    // This test documents the required fix

    // In NoteTrackEngine.cpp around line 118, change:
    //
    // BEFORE (buggy):
    // int inversion = (inversionValue == 0) ? sequence.harmonyInversion() : (inversionValue - 1);
    // int voicing = (voicingValue == 0) ? sequence.harmonyVoicing() : (voicingValue - 1);
    //
    // AFTER (fixed):
    // int inversion = (inversionValue == 0) ? masterSequence.harmonyInversion() : (inversionValue - 1);
    // int voicing = (voicingValue == 0) ? masterSequence.harmonyVoicing() : (voicingValue - 1);

    // Demonstrate the fix works
    NoteSequence masterSeq(0);
    NoteSequence followerSeq(1);

    masterSeq.setHarmonyInversion(2);
    followerSeq.setHarmonyInversion(0);

    int inversionValue = 0; // No per-step override

    // Fixed code
    int fixedInversion = (inversionValue == 0) ? masterSeq.harmonyInversion() : (inversionValue - 1);

    expectEqual(fixedInversion, 2, "fixed code correctly returns master's inversion");
}

// =============================================================================
// SECTION 6: Edge Cases for the Fix
// =============================================================================

CASE("fix_works_with_multiple_followers_same_master") {
    NoteSequence masterSeq(0);
    NoteSequence follower1(1);
    NoteSequence follower2(2);

    // Master has 3rd inversion
    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(3);

    // Both followers point to same master
    follower1.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    follower1.setMasterTrackIndex(0);

    follower2.setHarmonyRole(NoteSequence::HarmonyFollower5th);
    follower2.setMasterTrackIndex(0);

    // Fixed code: both read from masterSeq
    int override1 = masterSeq.step(0).inversionOverride();
    int effective1 = (override1 == 0) ? masterSeq.harmonyInversion() : (override1 - 1);

    int override2 = masterSeq.step(0).inversionOverride();
    int effective2 = (override2 == 0) ? masterSeq.harmonyInversion() : (override2 - 1);

    expectEqual(effective1, 3, "follower 1 uses master's inversion");
    expectEqual(effective2, 3, "follower 2 uses master's inversion");
    expectEqual(effective1, effective2, "both followers use same inversion");
}

CASE("fix_works_with_different_masters") {
    // Two harmony groups with different masters
    NoteSequence master1(0);
    NoteSequence follower1(1);
    NoteSequence master2(4);
    NoteSequence follower2(5);

    // Group 1: master with 1st inversion
    master1.setHarmonyRole(NoteSequence::HarmonyMaster);
    master1.setHarmonyInversion(1);

    follower1.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    follower1.setMasterTrackIndex(0);

    // Group 2: master with 2nd inversion
    master2.setHarmonyRole(NoteSequence::HarmonyMaster);
    master2.setHarmonyInversion(2);

    follower2.setHarmonyRole(NoteSequence::HarmonyFollowerRoot);
    follower2.setMasterTrackIndex(4);

    // Fixed code: each follower reads from its own master
    int effective1 = master1.harmonyInversion();
    int effective2 = master2.harmonyInversion();

    expectEqual(effective1, 1, "follower 1 uses master 1's inversion (1)");
    expectEqual(effective2, 2, "follower 2 uses master 2's inversion (2)");
}

CASE("fix_preserves_per_step_override_behavior") {
    NoteSequence masterSeq(0);

    masterSeq.setHarmonyRole(NoteSequence::HarmonyMaster);
    masterSeq.setHarmonyInversion(1); // Sequence-level
    masterSeq.step(3).setInversionOverride(3); // Per-step override for step 3

    // Step without override
    int override0 = masterSeq.step(0).inversionOverride();
    int effective0 = (override0 == 0) ? masterSeq.harmonyInversion() : (override0 - 1);
    expectEqual(effective0, 1, "step 0: uses sequence-level (1)");

    // Step with override
    int override3 = masterSeq.step(3).inversionOverride();
    int effective3 = (override3 == 0) ? masterSeq.harmonyInversion() : (override3 - 1);
    expectEqual(effective3, 2, "step 3: uses per-step override (2)");
}

} // UNIT_TEST("Harmony Inversion Bug")
