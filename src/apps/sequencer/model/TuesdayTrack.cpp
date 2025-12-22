#include "TuesdayTrack.h"
#include "ProjectVersion.h"

void TuesdayTrack::gateOutputName(int index, StringBuilder &str) const {
    str("G%d", _trackIndex + 1);
}

void TuesdayTrack::cvOutputName(int index, StringBuilder &str) const {
    str("CV%d", _trackIndex + 1);
}

void TuesdayTrack::clear() {
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void TuesdayTrack::write(VersionedSerializedWriter &writer) const {
    writeArray(writer, _sequences);
}

void TuesdayTrack::read(VersionedSerializedReader &reader) {
    readArray(reader, _sequences);
}
