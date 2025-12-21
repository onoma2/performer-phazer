#include "IndexedSequenceEditPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/painters/SequencePainter.h"

#include "model/Scale.h"
#include "model/Routing.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Route,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "ROUTE" },
};

IndexedSequenceEditPage::IndexedSequenceEditPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void IndexedSequenceEditPage::enter() {
}

void IndexedSequenceEditPage::exit() {
}

void IndexedSequenceEditPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "INDEXED EDIT");

    const auto &sequence = _project.selectedIndexedSequence();
    const auto &trackEngine = _engine.selectedTrackEngine().as<IndexedTrackEngine>();

    // 1. Top Section: Timeline Bar
    // Calculate total duration of active steps
    int totalTicks = 0;
    int activeLength = sequence.activeLength();
    for (int i = 0; i < activeLength; ++i) {
        totalTicks += sequence.step(i).duration();
    }

    if (totalTicks > 0) {
        const int barX = 8;
        const int barY = 14;
        const int barW = 240;
        const int barH = 16;

        int currentX = barX;
        float pixelsPerTick = (float)barW / totalTicks;

        for (int i = 0; i < activeLength; ++i) {
            const auto &step = sequence.step(i);
            int stepW = (int)(step.duration() * pixelsPerTick);
            // Ensure at least 1 pixel width if duration > 0, but handle truncation
            if (stepW < 1 && step.duration() > 0) stepW = 1;
            
            // Correction for last step to fit exactly?
            // Accumulate error or just clamp? Simple approach for now.
            if (i == activeLength - 1) {
                stepW = (barX + barW) - currentX;
            }

            // Visual state
            bool selected = _stepSelection[i];
            bool active = (trackEngine.currentStep() == i);

            // Draw Step Outline
            canvas.setColor(selected ? Color::Bright : (active ? Color::MediumBright : Color::Medium));
            canvas.drawRect(currentX, barY, stepW, barH);

            // Draw Gate Fill
            int gateW = (int)(stepW * (step.gateLength() / 100.0f));
            if (gateW > 0) {
                canvas.setColor(selected ? Color::Bright : (active ? Color::MediumBright : Color::Low));
                canvas.fillRect(currentX + 1, barY + 1, gateW, barH - 2);
            }

            currentX += stepW;
        }
    }

    // 2. Bottom Section: Info & Edit (F1, F2, F3)
    // Coords aligned with F-keys (approx centers: 25, 76, 127)
    // We display info for the FIRST selected step (or nothing if none)
    
    if (_stepSelection.any()) {
        int stepIndex = _stepSelection.first();
        const auto &step = sequence.step(stepIndex);
        
        const int y = 40;
        
        canvas.setFont(Font::Small);
        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Bright);

        // F1: Note
        FixedStringBuilder<16> noteStr;
        const auto &scale = sequence.selectedScale(_project.selectedScale());
        // Using rootNote from sequence or project? sequence has rootNote.
        scale.noteName(noteStr, step.noteIndex(), sequence.rootNote(), Scale::Format::Short1);
        canvas.drawTextCentered(0, y, 51, 16, noteStr);

        // F2: Duration
        FixedStringBuilder<16> durStr;
        durStr("%d", step.duration());
        canvas.drawTextCentered(51, y, 51, 16, durStr);

        // F3: Gate
        FixedStringBuilder<16> gateStr;
        gateStr("%d%%", step.gateLength());
        canvas.drawTextCentered(102, y, 51, 16, gateStr);
    } else {
        canvas.setColor(Color::Medium);
        canvas.drawTextCentered(0, 40, 256, 16, "SELECT STEP");
    }

    // Footer Labels
    const char *footerLabels[] = { "NOTE", "DUR", "GATE", "", "NEXT" };
    WindowPainter::drawFooter(canvas, footerLabels, pageKeyState(), (int)_editMode);
}

void IndexedSequenceEditPage::updateLeds(Leds &leds) {
    const auto &sequence = _project.selectedIndexedSequence();
    const auto &trackEngine = _engine.selectedTrackEngine().as<IndexedTrackEngine>();
    int currentStep = trackEngine.currentStep();

    // Map 16 physical keys to 16 steps of current section
    int stepOffset = this->stepOffset();
    
    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset + i;
        if (stepIndex >= sequence.activeLength()) break;

        bool selected = _stepSelection[stepIndex];
        bool playing = (stepIndex == currentStep);

        // Green = Playing
        // Red = Selected
        // Yellow = Both
        bool green = playing;
        bool red = selected;

        leds.set(MatrixMap::fromStep(i), red, green);
    }

    LedPainter::drawSelectedSequenceSection(leds, _section);
}

void IndexedSequenceEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    // Step Selection (Pagination handled)
    if (key.isStep()) {
        int stepIndex = stepOffset() + key.step();
        if (stepIndex < _project.selectedIndexedSequence().activeLength()) {
            _stepSelection.keyPress(event, stepOffset());
        }
        event.consume();
        return;
    }

    // Function Keys
    if (key.isFunction()) {
        // ... Future: Page switching with F5
        event.consume();
        return;
    }
    
    // Pagination (Left/Right)
    if (key.isLeft()) {
        _section = std::max(0, _section - 1);
        event.consume();
    }
    if (key.isRight()) {
        // Max 2 sections (32 steps / 16 = 2) -> index 0 and 1
        _section = std::min(1, _section + 1);
        event.consume();
    }
}

void IndexedSequenceEditPage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedIndexedSequence();
    
    if (!_stepSelection.any()) return;

    // Determine which parameter to edit based on active F-Key?
    // User didn't specify holding F-key to edit. 
    // Usually standard behavior is: 
    // - Encoder 1 -> Param 1
    // - Encoder 2 -> Param 2 ...
    // But Performer usually has 1 main encoder.
    // So we need a "Selected Parameter" state (Column selection)?
    // OR we detect which F-key is held?
    // "F1 Left side... F2 Middle... F3 Right" suggests columns.
    // In Performer, usually you select a "Column" or "Layer".
    
    // For now, let's assume we need a "Layer" or "Focus" state.
    // Let's use F1/F2/F3 to select the edit target?
    // "F1 Left side... F2 Middle... F3 Right... F5 Next page"
    // This implies F-keys SELECT the parameter to edit with the MAIN encoder.
    
    // Let's infer the active parameter from _activeLayer/Mode.
    
    // BUT: "F1... F2... F3" labels in footer often imply they ARE the buttons to press to select.
    // So:
    // Press F1 -> Select Note Edit Mode.
    // Press F2 -> Select Duration Edit Mode.
    // Press F3 -> Select Gate Edit Mode.
    
    for (int i = 0; i < IndexedSequence::MaxSteps; ++i) {
        if (_stepSelection[i]) {
            auto &step = sequence.step(i);
            bool shift = globalKeyState()[Key::Shift];
            
            switch (_editMode) {
            case EditMode::Note:
                step.setNoteIndex(step.noteIndex() + event.value() * (shift ? 12 : 1));
                break;
            case EditMode::Duration: {
                int div = sequence.divisor();
                int stepVal = shift ? 1 : div;
                int newDur = step.duration() + event.value() * stepVal;
                step.setDuration(clamp(newDur, 0, 65535));
                break;
            }
            case EditMode::Gate:
                // Shift: +/- 1%, Normal: +/- 5%
                step.setGateLength(clamp(step.gateLength() + event.value() * (shift ? 1 : 5), 0, 100));
                break;
            }
        }
    }
    
    event.consume();
}

void IndexedSequenceEditPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();
    if (key.isStep()) {
        _stepSelection.keyDown(event, stepOffset());
    }
    
    // Function keys select edit mode
    if (key.isFunction()) {
        int fn = key.function();
        if (fn == 0) _editMode = EditMode::Note;
        if (fn == 1) _editMode = EditMode::Duration;
        if (fn == 2) _editMode = EditMode::Gate;
        
        if (fn == 4) {
            // Next Page Toggle logic (placeholder)
        }
    }
}

void IndexedSequenceEditPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();
    if (key.isStep()) {
        _stepSelection.keyUp(event, stepOffset());
    }
}

void IndexedSequenceEditPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void IndexedSequenceEditPage::contextAction(int index) {
    auto &sequence = _project.selectedIndexedSequence();
    switch (ContextAction(index)) {
    case ContextAction::Init:
        sequence.clear();
        showMessage("SEQUENCE CLEARED");
        break;
    case ContextAction::Copy:
        // Copy selected steps? Or whole sequence?
        // Usually steps if selection exists.
        if (_stepSelection.any()) {
             ClipBoard::SelectedSteps steps = _stepSelection.selected().to_ullong();
            _model.clipBoard().copyIndexedSequenceSteps(sequence, steps);
            showMessage("STEPS COPIED");
        }
        break;
    case ContextAction::Paste:
        if (_stepSelection.any()) {
             ClipBoard::SelectedSteps steps = _stepSelection.selected().to_ullong();
            _model.clipBoard().pasteIndexedSequenceSteps(sequence, steps);
            showMessage("STEPS PASTED");
        }
        break;
    case ContextAction::Route:
        // Route something?
        break;
    case ContextAction::Last:
        break;
    }
}

bool IndexedSequenceEditPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteIndexedSequenceSteps();
    default:
        return true;
    }
}

IndexedSequence::Step& IndexedSequenceEditPage::step(int index) {
    return _project.selectedIndexedSequence().step(index);
}

const IndexedSequence::Step& IndexedSequenceEditPage::step(int index) const {
    return _project.selectedIndexedSequence().step(index);
}