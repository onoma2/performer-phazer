#pragma once

#include "Config.h"
#include "RoutableListModel.h"
#include "model/IndexedTrack.h"

class IndexedTrackListModel : public RoutableListModel {
public:
    void setTrack(IndexedTrack *track) {
        _track = track;
    }

    virtual int rows() const override {
        return _track ? Last : 0;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case Octave:
            return Routing::Target::Octave;
        case Transpose:
            return Routing::Target::Transpose;
        default:
            return Routing::Target::None;
        }
    }

private:
    enum Item {
        CvUpdateMode,
        Octave,
        Transpose,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case CvUpdateMode: return "CV Update";
        case Octave:       return "Octave";
        case Transpose:    return "Transpose";
        case Last:         break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case CvUpdateMode:
            _track->printCvUpdateMode(str);
            break;
        case Octave:
            _track->printOctave(str);
            break;
        case Transpose:
            _track->printTranspose(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case CvUpdateMode:
            _track->editCvUpdateMode(value, shift);
            break;
        case Octave:
            _track->editOctave(value, shift);
            break;
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case Last:
            break;
        }
    }

    IndexedTrack *_track = nullptr;
};
