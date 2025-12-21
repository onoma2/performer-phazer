#include "IndexedSequence.h"
#include "ProjectVersion.h"

void IndexedSequence::writeRouted(Routing::Target target, int intValue, float floatValue) {
    // Debug: log routing writes
    if (target == Routing::Target::IndexedA || target == Routing::Target::IndexedB) {
        printf("[INDEXED] writeRouted target=%d (%s) intValue=%d floatValue=%f\n",
               static_cast<int>(target),
               target == Routing::Target::IndexedA ? "IndexedA" : "IndexedB",
               intValue, floatValue);
    }

    switch (target) {
    case Routing::Target::Divisor:
        setDivisor(intValue);
        break;
    case Routing::Target::Scale:
        setScale(intValue);
        break;
    case Routing::Target::RootNote:
        setRootNote(intValue);
        break;
    case Routing::Target::FirstStep:
        setFirstStep(intValue, true);
        break;
    case Routing::Target::IndexedA:
        _routedIndexedA = floatValue * 0.01f;  // Normalize from -100..100 to -1..1
        break;
    case Routing::Target::IndexedB:
        _routedIndexedB = floatValue * 0.01f;  // Normalize from -100..100 to -1..1
        break;
    default:
        break;
    }
}
