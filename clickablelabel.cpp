// https://wiki.qt.io/Clickable_QLabel

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, [[maybe_unused]]Qt::WindowFlags f)
    : QLabel(parent) {

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent([[maybe_unused]]QMouseEvent* event) {
    emit clicked();
}
