#include "TextShape.h"

TextShape::TextShape(const QPoint& pos) {
    m_pos = pos;
}

void TextShape::draw(QPainter& painter) {
    painter.setFont(m_font);
    painter.setPen(m_textColor);

    QFontMetrics metrics(m_font);
    int textWidth = metrics.horizontalAdvance(m_text);
    int textHeight = metrics.height();

    QRect rect(
        m_pos.x() - textWidth / 2,
        m_pos.y() - textHeight / 2,
        textWidth,
        textHeight
    );

    painter.drawText(rect, Qt::AlignCenter, m_text);
}


bool TextShape::contains(const QPoint& point) {
    QFontMetrics metrics(m_font);
    int textWidth = metrics.horizontalAdvance(m_text);
    int textHeight = metrics.height();

    QRect rect(
        m_pos.x() - textWidth / 2,
        m_pos.y() - textHeight / 2,
        textWidth,
        textHeight
    );

    return rect.contains(point);
}


std::shared_ptr<ShapeBase> TextShape::clone() const {
    auto copy = std::make_shared<TextShape>(m_pos);
    copy->setText(m_text);
    copy->setFont(m_font);
    copy->setTextColor(m_textColor);
    return copy;
}

QVector<QPoint> TextShape::connectionPoints() const {
    return {};
}


QString TextShape::type() const {
    return "text";
}

QJsonObject TextShape::toJson() const {
    QJsonObject obj;
    obj["type"] = type();
    obj["x"] = m_pos.x();
    obj["y"] = m_pos.y();
    obj["text"] = m_text;
    obj["font"] = m_font.toString();
    obj["textColor"] = m_textColor.name();
    return obj;
}

void TextShape::fromJson(const QJsonObject& obj) {
    m_pos.setX(obj["x"].toInt());
    m_pos.setY(obj["y"].toInt());

    m_text = obj["text"].toString();
    m_textColor = QColor(obj["textColor"].toString());

    QJsonObject fontObj = obj["font"].toObject();
    QFont font;
    font.setFamily(fontObj["family"].toString());
    font.setPointSize(fontObj["size"].toInt());
    font.setBold(fontObj["bold"].toBool());
    font.setItalic(fontObj["italic"].toBool());
    font.setUnderline(fontObj["underline"].toBool());
    m_font = font;
}
