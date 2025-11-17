#include "UnitTest.h"

#include "apps/sequencer/model/NoteSequence.h"

#include <string>

UNIT_TEST("PulseCount") {

CASE("infrastructure") {
    // Placeholder test to verify test infrastructure is working
    expectTrue(true, "test infrastructure working");
}

// ============================================================================
// Phase 1: Model Layer Tests
// ============================================================================

// Test 1.1: Basic Storage and Retrieval
CASE("step stores and retrieves pulse count") {
    NoteSequence::Step step;

    // Default pulse count should be 0
    expectEqual(step.pulseCount(), 0, "default pulse count should be 0");

    // Can set and get pulse count
    step.setPulseCount(3);
    expectEqual(step.pulseCount(), 3, "should store pulse count 3");

    // Can set minimum value
    step.setPulseCount(0);
    expectEqual(step.pulseCount(), 0, "should store minimum value 0");

    // Can set maximum value
    step.setPulseCount(7);
    expectEqual(step.pulseCount(), 7, "should store maximum value 7");
}

// Test 1.2: Value Clamping
CASE("pulse count clamps out-of-range values") {
    NoteSequence::Step step;

    // Negative values should clamp to 0
    step.setPulseCount(-5);
    expectEqual(step.pulseCount(), 0, "negative value should clamp to 0");

    step.setPulseCount(-1);
    expectEqual(step.pulseCount(), 0, "-1 should clamp to 0");

    // Values above max should clamp to 7
    step.setPulseCount(10);
    expectEqual(step.pulseCount(), 7, "10 should clamp to 7");

    step.setPulseCount(100);
    expectEqual(step.pulseCount(), 7, "100 should clamp to 7");

    step.setPulseCount(255);
    expectEqual(step.pulseCount(), 7, "255 should clamp to 7");

    // Values within range should be preserved
    for (int i = 0; i <= 7; ++i) {
        step.setPulseCount(i);
        expectEqual(step.pulseCount(), i, "value within range should be preserved");
    }
}

// Test 1.3: Bitfield Packing - No Interference with Other Fields
CASE("pulse count bitfield does not interfere with other fields") {
    NoteSequence::Step step;

    // Test 1: Set other fields first, then pulse count
    step.setRetrigger(2);
    step.setRetriggerProbability(5);
    step.setCondition(Types::Condition::Fill);
    step.setAccumulatorTrigger(true);
    step.setPulseCount(4);

    expectEqual(step.retrigger(), 2, "retrigger should be unchanged");
    expectEqual(step.retriggerProbability(), 5, "retriggerProbability should be unchanged");
    expectEqual(static_cast<int>(step.condition()), static_cast<int>(Types::Condition::Fill), "condition should be unchanged");
    expectTrue(step.isAccumulatorTrigger(), "accumulatorTrigger should be unchanged");
    expectEqual(step.pulseCount(), 4, "pulseCount should be stored");

    // Test 2: Set pulse count first, then other fields
    step.setPulseCount(6);
    step.setRetrigger(3);
    step.setRetriggerProbability(4);

    expectEqual(step.pulseCount(), 6, "pulseCount should be unchanged after setting other fields");

    // Test 3: All fields to max (bitfield independence)
    step.setPulseCount(7);
    step.setRetrigger(3);
    step.setRetriggerProbability(7);
    step.setAccumulatorTrigger(true);

    expectEqual(step.pulseCount(), 7, "pulseCount should be 7");
    expectEqual(step.retrigger(), 3, "retrigger should be 3");
    expectEqual(step.retriggerProbability(), 7, "retriggerProbability should be 7");
    expectTrue(step.isAccumulatorTrigger(), "accumulatorTrigger should be true");
}

// Test 1.4: Layer Integration
CASE("pulse count integrates with Layer system") {
    // Test 1: PulseCount layer exists in enum
    auto pulseCountLayer = NoteSequence::Layer::PulseCount;
    expectTrue(pulseCountLayer < NoteSequence::Layer::Last, "PulseCount should be a valid layer");

    // Test 2: layerName returns correct name
    const char* name = NoteSequence::layerName(NoteSequence::Layer::PulseCount);
    expectTrue(name != nullptr, "layerName should return non-null");
    expectEqual(std::string(name), std::string("PULSE COUNT"), "layer name should be 'PULSE COUNT'");

    // Test 3: layerRange returns {0, 7}
    auto range = NoteSequence::layerRange(NoteSequence::Layer::PulseCount);
    expectEqual(range.min, 0, "layer range min should be 0");
    expectEqual(range.max, 7, "layer range max should be 7");

    // Test 4: layerDefaultValue returns 0
    int defaultValue = NoteSequence::layerDefaultValue(NoteSequence::Layer::PulseCount);
    expectEqual(defaultValue, 0, "layer default value should be 0");

    // Test 5: Step::layerValue and setLayerValue work correctly
    NoteSequence::Step step;

    // Get value through layer interface
    int value = step.layerValue(NoteSequence::Layer::PulseCount);
    expectEqual(value, 0, "layerValue should return 0 initially");

    // Set value through layer interface
    step.setLayerValue(NoteSequence::Layer::PulseCount, 5);
    expectEqual(step.pulseCount(), 5, "setPulseCount via layerValue should work");
    expectEqual(step.layerValue(NoteSequence::Layer::PulseCount), 5, "layerValue should return 5");

    // Test clamping through layer interface
    step.setLayerValue(NoteSequence::Layer::PulseCount, 100);
    expectEqual(step.layerValue(NoteSequence::Layer::PulseCount), 7, "layerValue should clamp to 7");
}

}
