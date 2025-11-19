#pragma once

#include "ListModel.h"

#include "model/NoteSequence.h"

class HarmonyListModel : public ListModel {
public:
    enum Item {
        HarmonyRole,
        MasterTrack,
        HarmonyScale,
        Last
    };

    HarmonyListModel() : _sequence(nullptr) {}

    void setSequence(NoteSequence *sequence) {
        _sequence = sequence;
    }

    virtual int rows() const override {
        return _sequence ? Last : 0;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (!_sequence) return;

        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (!_sequence || column != 1) return;

        // Check if this row has indexed values (HarmonyRole or HarmonyScale)
        int count = indexedCount(row);
        if (count > 0) {
            // For indexed values, cycle through the options
            int current = indexed(row);
            if (current >= 0) {
                int next = (current + value) % count;
                // Handle negative values properly
                if (next < 0) next += count;
                setIndexed(row, next);
            }
        } else {
            // For non-indexed values, use the original editValue method
            editValue(Item(row), value, shift);
        }
    }

    virtual int indexedCount(int row) const override {
        if (!_sequence) return 0;

        switch (Item(row)) {
        case HarmonyRole:
            return 6; // Off, Master, FollowerRoot, Follower3rd, Follower5th, Follower7th
        case HarmonyScale:
            return 7; // Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian
        default:
            return 0;
        }
    }

    virtual int indexed(int row) const override {
        if (!_sequence) return -1;

        switch (Item(row)) {
        case HarmonyRole:
            return static_cast<int>(_sequence->harmonyRole());
        case HarmonyScale:
            return _sequence->harmonyScale();
        default:
            return -1;
        }
    }

    virtual void setIndexed(int row, int index) override {
        if (!_sequence || index < 0) return;

        if (index < indexedCount(row)) {
            switch (Item(row)) {
            case HarmonyRole:
                _sequence->setHarmonyRole(static_cast<NoteSequence::HarmonyRole>(index));
                break;
            case HarmonyScale:
                _sequence->setHarmonyScale(index);
                break;
            default:
                break;
            }
        }
    }

private:
    static const char *itemName(Item item) {
        switch (item) {
        case HarmonyRole:   return "ROLE";
        case MasterTrack:   return "MASTER";
        case HarmonyScale:  return "SCALE";
        case Last:          break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        if (!_sequence) return;

        switch (item) {
        case HarmonyRole: {
            auto role = _sequence->harmonyRole();
            switch (role) {
            case NoteSequence::HarmonyOff:          str("OFF"); break;
            case NoteSequence::HarmonyMaster:       str("MASTER"); break;
            case NoteSequence::HarmonyFollowerRoot: str("ROOT"); break;
            case NoteSequence::HarmonyFollower3rd:  str("3RD"); break;
            case NoteSequence::HarmonyFollower5th:  str("5TH"); break;
            case NoteSequence::HarmonyFollower7th:  str("7TH"); break;
            }
            break;
        }
        case MasterTrack:
            str("T%d", _sequence->masterTrackIndex() + 1); // Display as 1-8
            break;
        case HarmonyScale: {
            int scale = _sequence->harmonyScale();
            switch (scale) {
            case 0: str("IONIAN"); break;
            case 1: str("DORIAN"); break;
            case 2: str("PHRYGN"); break;
            case 3: str("LYDIAN"); break;
            case 4: str("MIXOLY"); break;
            case 5: str("AEOLIN"); break;
            case 6: str("LOCRIN"); break;
            default: str("---"); break;
            }
            break;
        }
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        if (!_sequence) return;

        switch (item) {
        case MasterTrack:
            _sequence->setMasterTrackIndex(
                clamp(_sequence->masterTrackIndex() + value, 0, 7)); // 0-7 for 8 tracks
            break;
        default:
            break;
        }
    }

    int clamp(int value, int min, int max) const {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    NoteSequence *_sequence;
};
