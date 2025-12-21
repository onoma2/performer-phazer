#include "IndexedTrack.h"
#include "ProjectVersion.h"

void IndexedTrack::gateOutputName(int index, StringBuilder &str) const {
    str("G%d", _trackIndex + 1);
}

void IndexedTrack::cvOutputName(int index, StringBuilder &str) const {
    str("CV%d", _trackIndex + 1);
}

void IndexedTrack::clear() {
    _cvUpdateMode = CvUpdateMode::Gate;
    _routedSync = 0.f;
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void IndexedTrack::write(VersionedSerializedWriter &writer) const {
    writer.write(_cvUpdateMode);
    writeArray(writer, _sequences);
}

void IndexedTrack::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() >= ProjectVersion::Version67) {
        reader.read(_cvUpdateMode);
    } else {
        _cvUpdateMode = CvUpdateMode::Gate;
    }
    readArray(reader, _sequences);
}

void IndexedTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    // For now, forward sequence-level routable params to all patterns
    // In the future, we might add track-level routable params here
    switch (target) {
    case Routing::Target::DiscreteMapSync:
        _routedSync = floatValue;
        break;
    default:
        for (auto &sequence : _sequences) {
            sequence.writeRouted(target, intValue, floatValue);
        }
        break;
    }
}
