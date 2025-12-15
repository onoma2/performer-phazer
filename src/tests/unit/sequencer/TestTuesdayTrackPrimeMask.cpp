#include "UnitTest.h"

#include "apps/sequencer/model/TuesdaySequence.h"

UNIT_TEST("TuesdayTrackPrimeMask") {

//----------------------------------------
// PrimeMaskPattern Parameter Default Value
//----------------------------------------

CASE("primeMaskPattern_default_value") {
    TuesdaySequence sequence;
    expectEqual(sequence.primeMaskPattern(), 1, "default primeMaskPattern should be 1 (ALLOW ALL)");
}

//----------------------------------------
// PrimeMaskPattern Parameter Setters/Getters
//----------------------------------------

CASE("primeMaskPattern_setter_getter") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(0);
    expectEqual(sequence.primeMaskPattern(), 0, "primeMaskPattern should be 0 (MASK ALL)");
    sequence.setPrimeMaskPattern(2);
    expectEqual(sequence.primeMaskPattern(), 2, "primeMaskPattern should be 2 (PRIME-BASED)");
    sequence.setPrimeMaskPattern(3);
    expectEqual(sequence.primeMaskPattern(), 3, "primeMaskPattern should be 3 (CUSTOM-BASED)");
}

CASE("primeMaskPattern_clamping_upper") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(5);
    expectEqual(sequence.primeMaskPattern(), 3, "primeMaskPattern should clamp to 3 (max)");
}

CASE("primeMaskPattern_clamping_lower") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(-1);
    expectEqual(sequence.primeMaskPattern(), 0, "primeMaskPattern should clamp to 0 (min)");
}

CASE("primeMaskPattern_edit_cycles") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(0);
    sequence.editPrimeMaskPattern(1, false);
    expectEqual(sequence.primeMaskPattern(), 1, "should cycle to 1");
    sequence.editPrimeMaskPattern(1, false);
    expectEqual(sequence.primeMaskPattern(), 2, "should cycle to 2");
    sequence.editPrimeMaskPattern(1, false);
    expectEqual(sequence.primeMaskPattern(), 3, "should cycle to 3");
    sequence.editPrimeMaskPattern(1, false);
    expectEqual(sequence.primeMaskPattern(), 0, "should cycle back to 0");
}

CASE("primeMaskPattern_print") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(0);
    StringBuilder str;
    sequence.printPrimeMaskPattern(str);
    expectEqual(strcmp(str, "MASK ALL"), 0, "should print 'MASK ALL' when pattern is 0");
    
    sequence.setPrimeMaskPattern(1);
    str.clear();
    sequence.printPrimeMaskPattern(str);
    expectEqual(strcmp(str, "ALLOW ALL"), 0, "should print 'ALLOW ALL' when pattern is 1");
    
    sequence.setPrimeMaskPattern(2);
    str.clear();
    sequence.printPrimeMaskPattern(str);
    expectEqual(strcmp(str, "PRIME-PAT"), 0, "should print 'PRIME-PAT' when pattern is 2");
    
    sequence.setPrimeMaskPattern(3);
    str.clear();
    sequence.printPrimeMaskPattern(str);
    expectEqual(strcmp(str, "CUSTOM-PAT"), 0, "should print 'CUSTOM-PAT' when pattern is 3");
}

//----------------------------------------
// PrimeMaskParameter Parameter Default Value
//----------------------------------------

CASE("primeMaskParameter_default_value") {
    TuesdaySequence sequence;
    expectEqual(sequence.primeMaskParameter(), 0, "default primeMaskParameter should be 0");
}

//----------------------------------------
// PrimeMaskParameter Parameter Setters/Getters
//----------------------------------------

CASE("primeMaskParameter_setter_getter") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskParameter(5);
    expectEqual(sequence.primeMaskParameter(), 5, "primeMaskParameter should be 5");
    sequence.setPrimeMaskParameter(12);
    expectEqual(sequence.primeMaskParameter(), 12, "primeMaskParameter should be 12");
}

CASE("primeMaskParameter_clamping_upper") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskParameter(20);
    expectEqual(sequence.primeMaskParameter(), 16, "primeMaskParameter should clamp to 16 (max)");
}

CASE("primeMaskParameter_clamping_lower") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskParameter(-5);
    expectEqual(sequence.primeMaskParameter(), 0, "primeMaskParameter should clamp to 0 (min)");
}

CASE("primeMaskParameter_edit") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskParameter(5);
    sequence.editPrimeMaskParameter(3, false);
    expectEqual(sequence.primeMaskParameter(), 8, "should increment by 3");
    sequence.editPrimeMaskParameter(-2, false);
    expectEqual(sequence.primeMaskParameter(), 6, "should decrement by 2");
}

CASE("primeMaskParameter_print") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskParameter(7);
    StringBuilder str;
    sequence.printPrimeMaskParameter(str);
    expectEqual(strcmp(str, "7"), 0, "should print '7' when parameter is 7");
}

//----------------------------------------
// Compatibility with Old tickMask API
//----------------------------------------

CASE("compatibility_old_tickMask_api") {
    TuesdaySequence sequence;
    sequence.setPrimeMaskPattern(1); // ALLOW ALL
    expectEqual(sequence.tickMask(), 1, "compatibility tickMask should return value from first table entry");
    
    // Set first entry in table to 0 to test compatibility
    sequence.setTickMaskTable(0, 0);
    expectEqual(sequence.tickMask(), 0, "compatibility tickMask should return value from first table entry");
}

//----------------------------------------
// Clear Method Updates
//----------------------------------------

CASE("clear_resets_new_parameters") {
    TuesdaySequence sequence;
    
    // Set non-default values
    sequence.setPrimeMaskPattern(2);
    sequence.setPrimeMaskParameter(10);
    
    // Clear
    sequence.clear();
    
    // Verify defaults restored
    expectEqual(sequence.primeMaskPattern(), 1, "primeMaskPattern should reset to 1 after clear");
    expectEqual(sequence.primeMaskParameter(), 0, "primeMaskParameter should reset to 0 after clear");
}

} // UNIT_TEST("TuesdayTrackPrimeMask")