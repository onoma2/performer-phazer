#include "UnitTest.h"
#include "apps/sequencer/model/CurveTrack.h"
#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"
#include "../core/io/MemoryReaderWriter.h"

UNIT_TEST("CurveTrackPhaseOffset") {

CASE("default_value") {
    CurveTrack track;
    expectEqual(track.phaseOffset(), 0, "default phase offset should be 0");
}

CASE("setter_getter") {
    CurveTrack track;
    track.setPhaseOffset(50);
    expectEqual(track.phaseOffset(), 50, "phase offset should be 50");
}

CASE("clamping_upper") {
    CurveTrack track;
    track.setPhaseOffset(200);
    expectEqual(track.phaseOffset(), 100, "phase offset should clamp to 100");
}

CASE("clamping_lower") {
    CurveTrack track;
    track.setPhaseOffset(-10);
    expectEqual(track.phaseOffset(), 0, "phase offset should clamp to 0");
}

CASE("edit_normal") {
    CurveTrack track;
    track.setPhaseOffset(50);
    track.editPhaseOffset(5, false);
    expectEqual(track.phaseOffset(), 55, "phase offset should be 55 after editing");
}

CASE("edit_with_shift") {
    CurveTrack track;
    track.setPhaseOffset(50);
    track.editPhaseOffset(1, true);
    expectEqual(track.phaseOffset(), 60, "phase offset should be 60 after shift editing");
}

CASE("edit_clamping") {
    CurveTrack track;
    track.setPhaseOffset(95);
    track.editPhaseOffset(10, false);
    expectEqual(track.phaseOffset(), 100, "phase offset should clamp to 100 after editing");
}

CASE("clear_resets_phase_offset") {
    CurveTrack track;
    track.setPhaseOffset(75);
    track.clear();
    expectEqual(track.phaseOffset(), 0, "phase offset should be 0 after clear");
}

CASE("serialization") {
    uint8_t buf[4096];
    std::memset(buf, 0, sizeof(buf));

    CurveTrack track;
    track.setPhaseOffset(75);

    MemoryWriter memoryWriter(buf, sizeof(buf));
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
        memoryWriter.write(data, len);
    }, 35);
    track.write(writer);

    MemoryReader memoryReader(buf, sizeof(buf));
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
        memoryReader.read(data, len);
    }, 35);

    CurveTrack loaded;
    loaded.read(reader);

    expectEqual(loaded.phaseOffset(), 75, "phase offset should persist after serialization");
}

CASE("serialization_boundary_values") {
    uint8_t buf1[4096];
    std::memset(buf1, 0, sizeof(buf1));

    CurveTrack track1;
    track1.setPhaseOffset(0);

    MemoryWriter memoryWriter1(buf1, sizeof(buf1));
    VersionedSerializedWriter writer1([&memoryWriter1] (const void *data, size_t len) {
        memoryWriter1.write(data, len);
    }, 35);
    track1.write(writer1);

    MemoryReader memoryReader1(buf1, sizeof(buf1));
    VersionedSerializedReader reader1([&memoryReader1] (void *data, size_t len) {
        memoryReader1.read(data, len);
    }, 35);

    CurveTrack loaded1;
    loaded1.read(reader1);

    expectEqual(loaded1.phaseOffset(), 0, "phase offset 0 should persist");

    uint8_t buf2[4096];
    std::memset(buf2, 0, sizeof(buf2));

    CurveTrack track2;
    track2.setPhaseOffset(100);

    MemoryWriter memoryWriter2(buf2, sizeof(buf2));
    VersionedSerializedWriter writer2([&memoryWriter2] (const void *data, size_t len) {
        memoryWriter2.write(data, len);
    }, 35);
    track2.write(writer2);

    MemoryReader memoryReader2(buf2, sizeof(buf2));
    VersionedSerializedReader reader2([&memoryReader2] (void *data, size_t len) {
        memoryReader2.read(data, len);
    }, 35);

    CurveTrack loaded2;
    loaded2.read(reader2);

    expectEqual(loaded2.phaseOffset(), 100, "phase offset 100 should persist");
}

} // UNIT_TEST("CurveTrackPhaseOffset")
