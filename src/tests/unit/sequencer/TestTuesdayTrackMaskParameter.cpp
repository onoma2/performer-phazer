#include "UnitTest.h"

#include "apps/sequencer/model/TuesdaySequence.h"

UNIT_TEST("TuesdayTrackMaskParameter") {

CASE("maskParameter_default_value") {
    TuesdaySequence sequence;
    expectEqual(sequence.maskParameter(), 0, "default maskParameter should be 0 (ALL)");
}

CASE("maskParameter_0_means_ALL") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(0);
    expectEqual(sequence.maskParameter(), 0, "maskParameter 0 should be ALL");

    StringBuilder str;
    sequence.printMaskParameter(str);
    expectEqual(strcmp(str, "ALL"), 0, "should print 'ALL' when parameter is 0");
}

CASE("maskParameter_15_means_NONE") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(15);
    expectEqual(sequence.maskParameter(), 15, "maskParameter 15 should be NONE");

    StringBuilder str;
    sequence.printMaskParameter(str);
    expectEqual(strcmp(str, "NONE"), 0, "should print 'NONE' when parameter is 15");
}

CASE("maskParameter_1_maps_to_2") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(1);
    expectEqual(sequence.maskParameter(), 1, "maskParameter 1 should map to value 2");

    StringBuilder str;
    sequence.printMaskParameter(str);
    expectEqual(strcmp(str, "2"), 0, "should print '2' when parameter is 1");
}

CASE("maskParameter_2_maps_to_3") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(2);
    expectEqual(sequence.maskParameter(), 2, "maskParameter 2 should map to value 3");

    StringBuilder str;
    sequence.printMaskParameter(str);
    expectEqual(strcmp(str, "3"), 0, "should print '3' when parameter is 2");
}

CASE("maskParameter_3_maps_to_5") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(3);
    expectEqual(sequence.maskParameter(), 3, "maskParameter 3 should map to value 5");

    StringBuilder str;
    sequence.printMaskParameter(str);
    expectEqual(strcmp(str, "5"), 0, "should print '5' when parameter is 3");
}

CASE("timeMode_still_exists") {
    TuesdaySequence sequence;
    expectEqual(sequence.timeMode(), 0, "default timeMode should still be 0 (FREE)");

    sequence.setTimeMode(3);
    expectEqual(sequence.timeMode(), 3, "timeMode should still be configurable");
}

CASE("maskParameter_clamping") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(20);
    expectEqual(sequence.maskParameter(), 15, "maskParameter should clamp to 15 (max)");

    sequence.setMaskParameter(-1);
    expectEqual(sequence.maskParameter(), 0, "maskParameter should clamp to 0 (min)");
}

CASE("maskParameter_edit") {
    TuesdaySequence sequence;
    sequence.setMaskParameter(0);
    sequence.editMaskParameter(5, false);
    expectEqual(sequence.maskParameter(), 5, "should increment by 5");

    sequence.editMaskParameter(-2, false);
    expectEqual(sequence.maskParameter(), 3, "should decrement by 2");
}

CASE("timeMode_edit_cycles") {
    TuesdaySequence sequence;
    sequence.setTimeMode(0);
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 1, "should cycle to 1");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 2, "should cycle to 2");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 3, "should cycle to 3");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 0, "should cycle back to 0");
}

} // UNIT_TEST("TuesdayTrackPrimeMaskSimplified")