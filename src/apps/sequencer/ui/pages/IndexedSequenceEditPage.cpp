#include "IndexedSequenceEditPage.h"

#include "ui/painters/WindowPainter.h"

IndexedSequenceEditPage::IndexedSequenceEditPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void IndexedSequenceEditPage::enter() {
}

void IndexedSequenceEditPage::exit() {
}

void IndexedSequenceEditPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "INDEXED EDIT");
    WindowPainter::drawFooter(canvas);
}

void IndexedSequenceEditPage::updateLeds(Leds &leds) {
}

void IndexedSequenceEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.pageModifier()) {
        return;
    }
}

void IndexedSequenceEditPage::encoder(EncoderEvent &event) {
    event.consume();
}
