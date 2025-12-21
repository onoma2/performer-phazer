#include "IndexedRouteConfigPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

#include "model/Track.h"

IndexedRouteConfigPage::IndexedRouteConfigPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void IndexedRouteConfigPage::enter() {
    _activeRoute = ActiveRoute::RouteA;
    _editParam = EditParam::Enabled;
}

void IndexedRouteConfigPage::exit() {
}

void IndexedRouteConfigPage::draw(Canvas &canvas) {
    if (_project.selectedTrack().trackMode() != Track::TrackMode::Indexed) {
        close();
        return;
    }

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ROUTE CONFIG");

    auto &sequence = _project.selectedIndexedSequence();

    // Draw Route A
    drawRouteConfig(canvas, sequence.routeA(), 16, _activeRoute == ActiveRoute::RouteA);

    // Draw Route B
    drawRouteConfig(canvas, sequence.routeB(), 36, _activeRoute == ActiveRoute::RouteB);

    // Footer: F1-F4 for parameter selection, F5 to exit
    const char *footerLabels[] = { "ENABLE", "GROUPS", "TARGET", "AMOUNT", "BACK" };
    WindowPainter::drawFooter(canvas, footerLabels, pageKeyState(), (int)_editParam);
}

void IndexedRouteConfigPage::drawRouteConfig(Canvas &canvas, const IndexedSequence::RouteConfig &cfg, int y, bool active) {
    canvas.setFont(Font::Small);
    canvas.setBlendMode(BlendMode::Set);

    // Route label
    canvas.setColor(active ? Color::Bright : Color::Medium);
    const char* routeName = (active && _activeRoute == ActiveRoute::RouteA) ? "ROUTE A:" : "ROUTE B:";
    canvas.drawText(8, y, routeName);

    // Enabled status
    canvas.setColor((_editParam == EditParam::Enabled && active) ? Color::Bright : Color::Medium);
    canvas.drawText(64, y, cfg.enabled ? "ON" : "OFF");

    if (!cfg.enabled) {
        // Don't show other params if disabled
        return;
    }

    // Target Groups
    canvas.setColor((_editParam == EditParam::TargetGroups && active) ? Color::Bright : Color::Medium);
    canvas.drawText(96, y, "G:");
    drawGroupMask(canvas, cfg.targetGroups, 112, y);

    // Target Parameter
    canvas.setColor((_editParam == EditParam::TargetParam && active) ? Color::Bright : Color::Medium);
    const char* targetName = "?";
    switch (cfg.targetParam) {
    case IndexedSequence::ModTarget::Duration:   targetName = "DUR"; break;
    case IndexedSequence::ModTarget::GateLength: targetName = "GATE"; break;
    case IndexedSequence::ModTarget::NoteIndex:  targetName = "NOTE"; break;
    case IndexedSequence::ModTarget::Last:       break;
    }
    canvas.drawText(152, y, targetName);

    // Amount
    canvas.setColor((_editParam == EditParam::Amount && active) ? Color::Bright : Color::Medium);
    FixedStringBuilder<16> amountStr;
    amountStr("%+.0f%%", cfg.amount);
    canvas.drawText(192, y, amountStr);
}

void IndexedRouteConfigPage::drawGroupMask(Canvas &canvas, uint8_t groupMask, int x, int y) {
    const char* groupLabels[] = {"A", "B", "C", "D"};
    for (int i = 0; i < 4; ++i) {
        bool inGroup = (groupMask & (1 << i)) != 0;
        canvas.setColor(inGroup ? Color::Bright : Color::Low);
        canvas.drawText(x + i * 8, y, groupLabels[i]);
    }
}

void IndexedRouteConfigPage::updateLeds(Leds &leds) {
    LedPainter::drawSelectedSequenceSection(leds, 0);
}

void IndexedRouteConfigPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.pageModifier()) {
        return;
    }

    // F1-F4: Select parameter to edit
    if (key.isFunction()) {
        int fn = key.function();
        if (fn >= 0 && fn <= 3) {
            _editParam = static_cast<EditParam>(fn);
        }

        // F5: Go back to previous page (IndexedSequenceEditPage)
        if (fn == 4) {
            _manager.pop();
        }

        event.consume();
        return;
    }

    // Left/Right: Switch between Route A and Route B
    if (key.isLeft()) {
        _activeRoute = ActiveRoute::RouteA;
        event.consume();
        return;
    }
    if (key.isRight()) {
        _activeRoute = ActiveRoute::RouteB;
        event.consume();
        return;
    }
}

void IndexedRouteConfigPage::encoder(EncoderEvent &event) {
    auto &cfg = activeRouteConfig();

    switch (_editParam) {
    case EditParam::Enabled:
        cfg.enabled = !cfg.enabled;
        break;

    case EditParam::TargetGroups: {
        // Cycle through group combinations
        // For simplicity, increment the bitmask (0-15)
        int newMask = cfg.targetGroups + event.value();
        cfg.targetGroups = clamp(newMask, 0, 15) & 0x0F;
        break;
    }

    case EditParam::TargetParam: {
        int param = static_cast<int>(cfg.targetParam);
        param += event.value();
        if (param < 0) param = static_cast<int>(IndexedSequence::ModTarget::Last) - 1;
        if (param >= static_cast<int>(IndexedSequence::ModTarget::Last)) param = 0;
        cfg.targetParam = static_cast<IndexedSequence::ModTarget>(param);
        break;
    }

    case EditParam::Amount: {
        bool shift = globalKeyState()[Key::Shift];
        float step = shift ? 1.0f : 10.0f;
        cfg.amount = clamp(cfg.amount + event.value() * step, -200.0f, 200.0f);
        break;
    }
    }

    event.consume();
}

IndexedSequence::RouteConfig& IndexedRouteConfigPage::activeRouteConfig() {
    auto &sequence = _project.selectedIndexedSequence();
    if (_activeRoute == ActiveRoute::RouteA) {
        return sequence.routeA();
    } else {
        return sequence.routeB();
    }
}

const IndexedSequence::RouteConfig& IndexedRouteConfigPage::activeRouteConfig() const {
    auto &sequence = _project.selectedIndexedSequence();
    if (_activeRoute == ActiveRoute::RouteA) {
        return sequence.routeA();
    } else {
        return sequence.routeB();
    }
}
