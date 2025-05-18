#include "ShapeBase.h"

void ShapeBase::moveTo(const QPoint& pos) {
    m_pos = pos;
}

QPoint ShapeBase::position() const {
    return m_pos;
}

void ShapeBase::setSelected(bool selected) {
    m_selected = selected;
}

bool ShapeBase::isSelected() const {
    return m_selected;
}

void ShapeBase::setText(const QString& text) {
    m_text = text;
}

QString ShapeBase::text() const {
    return m_text;
}

void ShapeBase::setFont(const QFont& font) {
    m_font = font;
}

QFont ShapeBase::font() const {
    return m_font;
}

void ShapeBase::setTextColor(const QColor& color) {
    m_textColor = color;
}

QColor ShapeBase::textColor() const {
    return m_textColor;
}


void ShapeBase::setBold(bool bold) {
    m_font.setBold(bold);
}

bool ShapeBase::isBold() const {
    return m_font.bold();
}

void ShapeBase::setItalic(bool italic) {
    m_font.setItalic(italic);
}

bool ShapeBase::isItalic() const {
    return m_font.italic();
}

void ShapeBase::setUnderline(bool underline) {
    m_font.setUnderline(underline);
}

bool ShapeBase::isUnderline() const {
    return m_font.underline();
}
