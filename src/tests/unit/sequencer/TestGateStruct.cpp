#include "UnitTest.h"

#include "apps/sequencer/engine/NoteTrackEngine.h"
#include "apps/sequencer/Config.h"

UNIT_TEST("GateStruct") {

CASE("basic gate fields") {
    NoteTrackEngine::Gate gate;
    gate.tick = 100;
    gate.gate = true;

    expectEqual(gate.tick, uint32_t(100), "tick should be 100");
    expectEqual(gate.gate, true, "gate should be true");
}

CASE("different tick values") {
    NoteTrackEngine::Gate gate;
    gate.tick = 48;
    gate.gate = false;

    expectEqual(gate.tick, uint32_t(48), "tick should be 48");
    expectEqual(gate.gate, false, "gate should be false");
}

CASE("gate struct size constraint") {
    // Ensure struct doesn't exceed memory constraints
#if CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS
    // With experimental fields: should be <= 16 bytes
    expectTrue(sizeof(NoteTrackEngine::Gate) <= 16, "Gate struct should be <= 16 bytes with experimental fields");
#else
    // Without experimental fields: should be 8 bytes
    expectEqual(sizeof(NoteTrackEngine::Gate), 8ul, "Gate struct should be 8 bytes without experimental fields");
#endif
}

CASE("basic 2-arg construction") {
    NoteTrackEngine::Gate gate = { 100, true };

    expectEqual(gate.tick, uint32_t(100), "tick should be 100");
    expectEqual(gate.gate, true, "gate should be true");
}

#if CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS

CASE("experimental shouldTickAccumulator field") {
    NoteTrackEngine::Gate gate;
    gate.tick = 100;
    gate.gate = true;
    gate.shouldTickAccumulator = true;
    gate.sequenceId = 0;

    expectEqual(gate.tick, uint32_t(100), "tick should be 100");
    expectEqual(gate.gate, true, "gate should be true");
    expectEqual(gate.shouldTickAccumulator, true, "shouldTickAccumulator should be true");
    expectEqual((unsigned int)gate.sequenceId, 0u, "sequenceId should be 0");
}

CASE("experimental shouldTickAccumulator defaults to false") {
    NoteTrackEngine::Gate gate = { 100, true };

    // With 2-arg constructor, experimental fields should default
    expectEqual(gate.shouldTickAccumulator, false, "shouldTickAccumulator should default to false");
    expectEqual((unsigned int)gate.sequenceId, 0u, "sequenceId should default to 0");
}

CASE("experimental sequenceId field") {
    NoteTrackEngine::Gate mainGate;
    mainGate.tick = 100;
    mainGate.gate = true;
    mainGate.shouldTickAccumulator = false;
    mainGate.sequenceId = 0;  // MainSequenceId

    NoteTrackEngine::Gate fillGate;
    fillGate.tick = 200;
    fillGate.gate = true;
    fillGate.shouldTickAccumulator = true;
    fillGate.sequenceId = 1;  // FillSequenceId

    expectEqual((unsigned int)mainGate.sequenceId, 0u, "main gate sequenceId should be 0");
    expectEqual((unsigned int)fillGate.sequenceId, 1u, "fill gate sequenceId should be 1");
    expectTrue(mainGate.sequenceId != fillGate.sequenceId, "main and fill sequenceIds should differ");
}

CASE("experimental 4-arg constructor") {
    NoteTrackEngine::Gate gate = { 100, true, true, 1 };

    expectEqual(gate.tick, uint32_t(100), "tick should be 100");
    expectEqual(gate.gate, true, "gate should be true");
    expectEqual(gate.shouldTickAccumulator, true, "shouldTickAccumulator should be true");
    expectEqual((unsigned int)gate.sequenceId, 1u, "sequenceId should be 1");
}

CASE("experimental sequence ID constants") {
    expectEqual((unsigned int)NoteTrackEngine::MainSequenceId, 0u, "MainSequenceId should be 0");
    expectEqual((unsigned int)NoteTrackEngine::FillSequenceId, 1u, "FillSequenceId should be 1");
    expectTrue(NoteTrackEngine::MainSequenceId != NoteTrackEngine::FillSequenceId,
               "MainSequenceId and FillSequenceId should differ");
}

#endif // CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS

}
