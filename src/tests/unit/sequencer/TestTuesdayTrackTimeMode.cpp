#include "UnitTest.h"

#include "apps/sequencer/model/TuesdaySequence.h"

UNIT_TEST("TuesdayTrackTimeMode") {

CASE("timeMode_default_value") {
    TuesdaySequence sequence;
    expectEqual(sequence.timeMode(), 0, "default timeMode should be 0 (FREE)");
}

CASE("timeMode_setter_getter") {
    TuesdaySequence sequence;
    sequence.setTimeMode(1);
    expectEqual(sequence.timeMode(), 1, "timeMode should be 1 (QRT)");
    sequence.setTimeMode(2);
    expectEqual(sequence.timeMode(), 2, "timeMode should be 2 (1.5Q)");
    sequence.setTimeMode(3);
    expectEqual(sequence.timeMode(), 3, "timeMode should be 3 (3QRT)");
    sequence.setTimeMode(0);
    expectEqual(sequence.timeMode(), 0, "timeMode should be 0 (FREE)");
}

CASE("timeMode_clamping_upper") {
    TuesdaySequence sequence;
    sequence.setTimeMode(5);
    expectEqual(sequence.timeMode(), 3, "timeMode should clamp to 3 (max)");
}

CASE("timeMode_clamping_lower") {
    TuesdaySequence sequence;
    sequence.setTimeMode(-1);
    expectEqual(sequence.timeMode(), 0, "timeMode should clamp to 0 (min)");
}

CASE("timeMode_edit_cycles") {
    TuesdaySequence sequence;
    sequence.setTimeMode(0);
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 1, "should cycle to 1 (QRT)");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 2, "should cycle to 2 (1.5Q)");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 3, "should cycle to 3 (3QRT)");
    sequence.editTimeMode(1, false);
    expectEqual(sequence.timeMode(), 0, "should cycle back to 0 (FREE)");
}

CASE("timeMode_print_free") {
    TuesdaySequence sequence;
    sequence.setTimeMode(0);
    StringBuilder str;
    sequence.printTimeMode(str);
    expectEqual(strcmp(str, "FREE"), 0, "should print 'FREE' when timeMode is 0");
}

CASE("timeMode_print_qrt") {
    TuesdaySequence sequence;
    sequence.setTimeMode(1);
    StringBuilder str;
    sequence.printTimeMode(str);
    expectEqual(strcmp(str, "QRT"), 0, "should print 'QRT' when timeMode is 1");
}

CASE("timeMode_print_15q") {
    TuesdaySequence sequence;
    sequence.setTimeMode(2);
    StringBuilder str;
    sequence.printTimeMode(str);
    expectEqual(strcmp(str, "1.5Q"), 0, "should print '1.5Q' when timeMode is 2");
}

CASE("timeMode_print_3qrt") {
    TuesdaySequence sequence;
    sequence.setTimeMode(3);
    StringBuilder str;
    sequence.printTimeMode(str);
    expectEqual(strcmp(str, "3QRT"), 0, "should print '3QRT' when timeMode is 3");
}

CASE("timeMode_clear_resets_to_default") {
    TuesdaySequence sequence;
    sequence.setTimeMode(2);  // Set to non-default value
    sequence.clear();
    expectEqual(sequence.timeMode(), 0, "timeMode should reset to 0 after clear");
}

} // UNIT_TEST("TuesdayTrackTimeMode")