#include "UnitTest.h"

#include "apps/sequencer/model/HarmonyEngine.h"

UNIT_TEST("HarmonyEngine") {

CASE("default_construction") {
    HarmonyEngine engine;

    expectEqual(static_cast<int>(engine.mode()), static_cast<int>(HarmonyEngine::Ionian), "default mode should be Ionian");
    expectTrue(engine.diatonicMode(), "default diatonicMode should be true");
    expectEqual(static_cast<int>(engine.inversion()), 0, "default inversion should be 0");
    expectEqual(static_cast<int>(engine.voicing()), static_cast<int>(HarmonyEngine::Close), "default voicing should be Close");
    expectEqual(static_cast<int>(engine.transpose()), 0, "default transpose should be 0");
}

CASE("ionian_scale_intervals") {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Ionian);

    // Ionian intervals: W-W-H-W-W-W-H = 0-2-4-5-7-9-11
    const int16_t expected[7] = {0, 2, 4, 5, 7, 9, 11};

    for (int degree = 0; degree < 7; degree++) {
        expectEqual(engine.getScaleInterval(degree), expected[degree], "Ionian scale interval mismatch");
    }
}

CASE("ionian_diatonic_chord_qualities") {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Ionian);
    engine.setDiatonicMode(true);

    // Ionian: I∆7, ii-7, iii-7, IV∆7, V7, vi-7, viiø
    expectEqual(static_cast<int>(engine.getDiatonicQuality(0)), static_cast<int>(HarmonyEngine::Major7), "I should be Major7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(1)), static_cast<int>(HarmonyEngine::Minor7), "ii should be Minor7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(2)), static_cast<int>(HarmonyEngine::Minor7), "iii should be Minor7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(3)), static_cast<int>(HarmonyEngine::Major7), "IV should be Major7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(4)), static_cast<int>(HarmonyEngine::Dominant7), "V should be Dominant7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(5)), static_cast<int>(HarmonyEngine::Minor7), "vi should be Minor7");
    expectEqual(static_cast<int>(engine.getDiatonicQuality(6)), static_cast<int>(HarmonyEngine::HalfDim7), "vii should be HalfDim7");
}

CASE("chord_intervals") {
    HarmonyEngine engine;

    // Major7: 0-4-7-11
    auto maj7Intervals = engine.getChordIntervals(HarmonyEngine::Major7);
    expectEqual(static_cast<int>(maj7Intervals[0]), 0, "Major7 root interval");
    expectEqual(static_cast<int>(maj7Intervals[1]), 4, "Major7 third interval");
    expectEqual(static_cast<int>(maj7Intervals[2]), 7, "Major7 fifth interval");
    expectEqual(static_cast<int>(maj7Intervals[3]), 11, "Major7 seventh interval");

    // Minor7: 0-3-7-10
    auto min7Intervals = engine.getChordIntervals(HarmonyEngine::Minor7);
    expectEqual(static_cast<int>(min7Intervals[0]), 0, "Minor7 root interval");
    expectEqual(static_cast<int>(min7Intervals[1]), 3, "Minor7 third interval");
    expectEqual(static_cast<int>(min7Intervals[2]), 7, "Minor7 fifth interval");
    expectEqual(static_cast<int>(min7Intervals[3]), 10, "Minor7 seventh interval");

    // Dominant7: 0-4-7-10
    auto dom7Intervals = engine.getChordIntervals(HarmonyEngine::Dominant7);
    expectEqual(static_cast<int>(dom7Intervals[0]), 0, "Dominant7 root interval");
    expectEqual(static_cast<int>(dom7Intervals[1]), 4, "Dominant7 third interval");
    expectEqual(static_cast<int>(dom7Intervals[2]), 7, "Dominant7 fifth interval");
    expectEqual(static_cast<int>(dom7Intervals[3]), 10, "Dominant7 seventh interval");

    // HalfDim7: 0-3-6-10
    auto halfDimIntervals = engine.getChordIntervals(HarmonyEngine::HalfDim7);
    expectEqual(static_cast<int>(halfDimIntervals[0]), 0, "HalfDim7 root interval");
    expectEqual(static_cast<int>(halfDimIntervals[1]), 3, "HalfDim7 third interval");
    expectEqual(static_cast<int>(halfDimIntervals[2]), 6, "HalfDim7 fifth interval");
    expectEqual(static_cast<int>(halfDimIntervals[3]), 10, "HalfDim7 seventh interval");
}

CASE("basic_harmonization") {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Ionian);

    // C Major 7 (I in C): C-E-G-B = 60-64-67-71
    auto cChord = engine.harmonize(60, 0);
    expectEqual(static_cast<int>(cChord.root), 60, "C Major7 root");
    expectEqual(static_cast<int>(cChord.third), 64, "C Major7 third");
    expectEqual(static_cast<int>(cChord.fifth), 67, "C Major7 fifth");
    expectEqual(static_cast<int>(cChord.seventh), 71, "C Major7 seventh");

    // D minor 7 (ii in C): D-F-A-C = 62-65-69-72
    auto dChord = engine.harmonize(62, 1);
    expectEqual(static_cast<int>(dChord.root), 62, "D minor7 root");
    expectEqual(static_cast<int>(dChord.third), 65, "D minor7 third");
    expectEqual(static_cast<int>(dChord.fifth), 69, "D minor7 fifth");
    expectEqual(static_cast<int>(dChord.seventh), 72, "D minor7 seventh");

    // G Dominant 7 (V in C): G-B-D-F = 67-71-74-77
    auto gChord = engine.harmonize(67, 4);
    expectEqual(static_cast<int>(gChord.root), 67, "G Dominant7 root");
    expectEqual(static_cast<int>(gChord.third), 71, "G Dominant7 third");
    expectEqual(static_cast<int>(gChord.fifth), 74, "G Dominant7 fifth");
    expectEqual(static_cast<int>(gChord.seventh), 77, "G Dominant7 seventh");
}

CASE("midi_range_clamping") {
    HarmonyEngine engine;
    engine.setMode(HarmonyEngine::Ionian);

    // High note that would exceed MIDI 127
    auto highChord = engine.harmonize(120, 0); // G#8 Major7

    // All notes should clamp to 127
    expectTrue(highChord.root <= 127, "high chord root should clamp to 127");
    expectTrue(highChord.third <= 127, "high chord third should clamp to 127");
    expectTrue(highChord.fifth <= 127, "high chord fifth should clamp to 127");
    expectTrue(highChord.seventh <= 127, "high chord seventh should clamp to 127");

    // Low note boundary
    auto lowChord = engine.harmonize(0, 0); // C-1 Major7

    // All notes should be >= 0
    expectTrue(lowChord.root >= 0, "low chord root should be >= 0");
    expectTrue(lowChord.third >= 0, "low chord third should be >= 0");
    expectTrue(lowChord.fifth >= 0, "low chord fifth should be >= 0");
    expectTrue(lowChord.seventh >= 0, "low chord seventh should be >= 0");
}

} // UNIT_TEST("HarmonyEngine")
