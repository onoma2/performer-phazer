#include "UnitTest.h"

// Test that TuesdaySequenceListModel exists and works
#include "ui/model/TuesdayTrackListModel.h"
#include "apps/sequencer/model/TuesdaySequence.h"
#include "core/utils/StringBuilder.h"
#include <cstring>

UNIT_TEST("TuesdaySequenceListModel") {

//----------------------------------------
// Basic structure tests
//----------------------------------------

CASE("header_compiles") {
    // This test passes if the header can be included
    expectTrue(true, "TuesdayTrackListModel.h compiles");
}

CASE("rows_count") {
    // TuesdaySequence has 9 parameters in list: Algorithm, Flow, Ornament, Power, LoopLength, Rotate, Glide, Skew, CvUpdateMode
    TuesdaySequenceListModel model;
    // Assuming Last enum value is 9
    expectTrue(model.rows() == 0, "should have 0 rows when no sequence set");
    TuesdaySequence sequence;
    model.setSequence(&sequence);
    expectEqual(model.rows(), 9, "should have 9 rows for parameters");
}

CASE("columns_count") {
    TuesdaySequenceListModel model;
    expectEqual(model.columns(), 2, "should have 2 columns (name, value)");
}

CASE("set_sequence") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);
    // Just verify it doesn't crash
    expectTrue(true, "setSequence works");
}

//----------------------------------------
// Cell name tests
//----------------------------------------

CASE("cell_names") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    FixedStringBuilder<32> str;
    
    model.cell(0, 0, str);
    expectEqual(strcmp(str, "Algorithm"), 0, "row 0 should be Algorithm");
    
    str.reset();
    model.cell(1, 0, str);
    expectEqual(strcmp(str, "Flow"), 0, "row 1 should be Flow");
    
    str.reset();
    model.cell(2, 0, str);
    expectEqual(strcmp(str, "Ornament"), 0, "row 2 should be Ornament");
    
    str.reset();
    model.cell(3, 0, str);
    expectEqual(strcmp(str, "Power"), 0, "row 3 should be Power");
    
    str.reset();
    model.cell(4, 0, str);
    expectEqual(strcmp(str, "Loop Length"), 0, "row 4 should be Loop Length");
}

//----------------------------------------
// Cell value tests (defaults)
//----------------------------------------

CASE("cell_values_defaults") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    FixedStringBuilder<32> str;
    
    model.cell(0, 1, str);
    expectEqual(strcmp(str, "TEST"), 0, "default algorithm should be TEST");
    
    str.reset();
    model.cell(1, 1, str);
    expectEqual(strcmp(str, "0"), 0, "default flow should be 0");
    
    str.reset();
    model.cell(3, 1, str);
    expectEqual(strcmp(str, "0"), 0, "default power should be 0");
}

//----------------------------------------
// Edit tests
//----------------------------------------

CASE("edit_algorithm") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    // Edit algorithm up by 1
    model.edit(0, 1, 1, false);
    expectEqual(sequence.algorithm(), 1, "algorithm should be 1 after edit");

    // Edit up by 1 more
    model.edit(0, 1, 1, false);
    expectEqual(sequence.algorithm(), 2, "algorithm should be 2 after second edit");
}

CASE("edit_flow") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    model.edit(1, 1, 5, false);
    expectEqual(sequence.flow(), 5, "flow should be 5 after edit");
}

CASE("edit_ornament") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    model.edit(2, 1, 8, false);
    expectEqual(sequence.ornament(), 8, "ornament should be 8 after edit");
}

CASE("edit_power") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    model.edit(3, 1, 10, false);
    expectEqual(sequence.power(), 10, "power should be 10 after edit");
}

CASE("edit_loop_length") {
    TuesdaySequence sequence;
    TuesdaySequenceListModel model;
    model.setSequence(&sequence);

    // Default loopLength index is 16, edit down by 1
    model.edit(4, 1, -1, false);
    expectEqual(sequence.loopLength(), 15, "loopLength should be 15 after edit");
}

//----------------------------------------
// Routing target tests
//----------------------------------------

CASE("routing_targets") {
    TuesdaySequenceListModel model;
    
    expectEqual(static_cast<int>(model.routingTarget(0)), static_cast<int>(Routing::Target::Algorithm), "algorithm should have Algorithm routing target");
    expectEqual(static_cast<int>(model.routingTarget(1)), static_cast<int>(Routing::Target::Flow), "flow should have Flow routing target");
    expectEqual(static_cast<int>(model.routingTarget(2)), static_cast<int>(Routing::Target::Ornament), "ornament should have Ornament routing target");
    expectEqual(static_cast<int>(model.routingTarget(3)), static_cast<int>(Routing::Target::Power), "power should have Power routing target");
    expectEqual(static_cast<int>(model.routingTarget(4)), static_cast<int>(Routing::Target::None), "loopLength should have no routing target");
}

} // UNIT_TEST("TuesdaySequenceListModel")
