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
    setOctave(0);
    setTranspose(0);
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void IndexedTrack::write(VersionedSerializedWriter &writer) const {
    writer.write(_cvUpdateMode);
    writer.write(_octave.base);
    writer.write(_transpose.base);
    writeArray(writer, _sequences);
}

void IndexedTrack::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() >= ProjectVersion::Version67) {
        reader.read(_cvUpdateMode);
    } else {
        _cvUpdateMode = CvUpdateMode::Gate;
    }
    if (reader.dataVersion() >= ProjectVersion::Version68) {
        reader.read(_octave.base);
        reader.read(_transpose.base);
    } else {
        _octave.base = 0;
        _transpose.base = 0;
    }
    _octave.routed = 0;
    _transpose.routed = 0;
    readArray(reader, _sequences);
}

void IndexedTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    // For now, forward sequence-level routable params to all patterns
    // In the future, we might add track-level routable params here
    switch (target) {
    case Routing::Target::DiscreteMapSync:
        _routedSync = floatValue;
        break;
    case Routing::Target::Octave:
        setOctave(intValue, true);
        break;
    case Routing::Target::Transpose:
        setTranspose(intValue, true);
        break;
    default:
        for (auto &sequence : _sequences) {
            sequence.writeRouted(target, intValue, floatValue);
        }
        break;
    }
}
