#include "UnitTest.h"

// Test that TuesdayTrackListModel exists and works
#include "ui/model/TuesdayTrackListModel.h"
#include "apps/sequencer/model/TuesdayTrack.h"
#include "core/utils/StringBuilder.h"

UNIT_TEST("TuesdayTrackListModel") {

//----------------------------------------
// Basic structure tests
//----------------------------------------

CASE("header_compiles") {
    // This test passes if the header can be included
    expectTrue(true, "TuesdayTrackListModel.h compiles");
}

CASE("rows_count") {
    // TuesdayTrack has 5 parameters: Algorithm, Flow, Ornament, Power, LoopLength
    TuesdayTrackListModel model;
    expectEqual(model.rows(), 5, "should have 5 rows for 5 parameters");
}

CASE("columns_count") {
    TuesdayTrackListModel model;
    expectEqual(model.columns(), 2, "should have 2 columns (name, value)");
}

CASE("set_track") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);
    // Just verify it doesn't crash
    expectTrue(true, "setTrack works");
}

//----------------------------------------
// Cell name tests
//----------------------------------------

CASE("cell_algorithm_name") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(0, 0, str);
    expectEqual(strcmp(str, "Algorithm"), 0, "row 0 should be Algorithm");
}

CASE("cell_flow_name") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(1, 0, str);
    expectEqual(strcmp(str, "Flow"), 0, "row 1 should be Flow");
}

CASE("cell_ornament_name") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(2, 0, str);
    expectEqual(strcmp(str, "Ornament"), 0, "row 2 should be Ornament");
}

CASE("cell_power_name") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(3, 0, str);
    expectEqual(strcmp(str, "Power"), 0, "row 3 should be Power");
}

CASE("cell_loop_length_name") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(4, 0, str);
    expectEqual(strcmp(str, "Loop Length"), 0, "row 4 should be Loop Length");
}

//----------------------------------------
// Cell value tests (defaults)
//----------------------------------------

CASE("cell_algorithm_default_value") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(0, 1, str);
    // Default algorithm is 0 = "TEST"
    expectEqual(strcmp(str, "TEST"), 0, "default algorithm should be TEST");
}

CASE("cell_flow_default_value") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(1, 1, str);
    expectEqual(strcmp(str, "0"), 0, "default flow should be 0");
}

CASE("cell_power_default_value") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    FixedStringBuilder<32> str;
    model.cell(3, 1, str);
    expectEqual(strcmp(str, "0"), 0, "default power should be 0");
}

//----------------------------------------
// Edit tests
//----------------------------------------

CASE("edit_algorithm") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    // Edit algorithm up by 1
    model.edit(0, 1, 1, false);
    expectEqual(track.algorithm(), 1, "algorithm should be 1 after edit");

    // Edit up by 1 more
    model.edit(0, 1, 1, false);
    expectEqual(track.algorithm(), 2, "algorithm should be 2 after second edit");
}

CASE("edit_flow") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    model.edit(1, 1, 5, false);
    expectEqual(track.flow(), 5, "flow should be 5 after edit");
}

CASE("edit_ornament") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    model.edit(2, 1, 8, false);
    expectEqual(track.ornament(), 8, "ornament should be 8 after edit");
}

CASE("edit_power") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    model.edit(3, 1, 10, false);
    expectEqual(track.power(), 10, "power should be 10 after edit");
}

CASE("edit_loop_length") {
    TuesdayTrack track;
    TuesdayTrackListModel model;
    model.setTrack(track);

    // Default loopLength index is 16, edit down by 1
    model.edit(4, 1, -1, false);
    expectEqual(track.loopLength(), 15, "loopLength should be 15 after edit");
}

//----------------------------------------
// Routing target tests
//----------------------------------------

CASE("routing_target_none") {
    TuesdayTrackListModel model;
    // Tuesday track parameters don't have routing targets (for now)
    expectEqual(static_cast<int>(model.routingTarget(0)), static_cast<int>(Routing::Target::None), "algorithm should have no routing target");
    expectEqual(static_cast<int>(model.routingTarget(1)), static_cast<int>(Routing::Target::None), "flow should have no routing target");
    expectEqual(static_cast<int>(model.routingTarget(2)), static_cast<int>(Routing::Target::None), "ornament should have no routing target");
    expectEqual(static_cast<int>(model.routingTarget(3)), static_cast<int>(Routing::Target::None), "power should have no routing target");
    expectEqual(static_cast<int>(model.routingTarget(4)), static_cast<int>(Routing::Target::None), "loopLength should have no routing target");
}

} // UNIT_TEST("TuesdayTrackListModel")
