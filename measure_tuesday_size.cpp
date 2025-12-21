#include "apps/sequencer/model/IndexedSequence.h"
#include "apps/sequencer/model/IndexedTrack.h"
#include "apps/sequencer/model/NoteSequence.h"
#include "apps/sequencer/model/NoteTrack.h"
#include <iostream>

int main() {
    std::cout << "=== IndexedTrack Memory Footprint ===" << std::endl;
    std::cout << "IndexedSequence::Step: " << sizeof(IndexedSequence::Step) << " bytes" << std::endl;
    std::cout << "IndexedSequence::RouteConfig: " << sizeof(IndexedSequence::RouteConfig) << " bytes" << std::endl;
    std::cout << "IndexedSequence (1 pattern): " << sizeof(IndexedSequence) << " bytes" << std::endl;
    std::cout << "IndexedTrack (all 8 patterns): " << sizeof(IndexedTrack) << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Comparison to NoteTrack ===" << std::endl;
    std::cout << "NoteSequence::Step: " << sizeof(NoteSequence::Step) << " bytes" << std::endl;
    std::cout << "NoteSequence (1 pattern): " << sizeof(NoteSequence) << " bytes" << std::endl;
    std::cout << "NoteTrack (all 8 patterns): " << sizeof(NoteTrack) << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Per-Track Overhead (8 tracks) ===" << std::endl;
    std::cout << "IndexedTrack * 8: " << sizeof(IndexedTrack) * 8 << " bytes (" 
              << (sizeof(IndexedTrack) * 8) / 1024.0 << " KB)" << std::endl;
    std::cout << "NoteTrack * 8: " << sizeof(NoteTrack) * 8 << " bytes (" 
              << (sizeof(NoteTrack) * 8) / 1024.0 << " KB)" << std::endl;
    
    return 0;
}
