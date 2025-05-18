#include "RectShape.h"
#include <QPainter>

RectShape::RectShape(const QPoint& pos) {
    m_pos = pos;
}

void RectShape::draw(QPainter& painter) {
    QRect rect(m_pos.x() - m_width / 2, m_pos.y() - m_height / 2, m_width, m_height);

    painter.setBrush(Qt::NoBrush);
    if (isSelected()) {
        painter.setPen(QPen(Qt::blue, 2));
    }
    else {
        painter.setPen(Qt::black);
    }

    painter.drawRect(rect);

    painter.setFont(m_font);
    painter.setPen(m_textColor);
    painter.drawText(rect, Qt::AlignCenter, m_text);
}

bool RectShape::contains(const QPoint& point) {
    QRect rect(m_pos.x() - m_width / 2, m_pos.y() - m_height / 2, m_width, m_height);
    return rect.contains(point);
}

std::shared_ptr<ShapeBase> RectShape::clone() const {
    auto copy = std::make_shared<RectShape>(m_pos);
    copy->setSelected(false);
    copy->setText(this->text());
    copy->setFont(this->font());
    copy->setTextColor(this->textColor());
    return copy;
}

QVector<QPoint> RectShape::connectionPoints() const {
    return {
        m_pos + QPoint(0, -m_height / 2),  
        m_pos + QPoint(0, m_height / 2),   
        m_pos + QPoint(-m_width / 2, 0),   
        m_pos + QPoint(m_width / 2, 0)    
    };
}


void RectShape::moveTo(const QPoint& pos) {
    m_pos = pos;
}

QString RectShape::type() const {
    return "rect";
}

QJsonObject RectShape::toJson() const {
    QJsonObject obj;
    obj["type"] = type();
    obj["x"] = m_pos.x();
    obj["y"] = m_pos.y();
    obj["width"] = m_width;
    obj["height"] = m_height;
    obj["text"] = m_text;
    obj["font"] = m_font.toString();
    obj["textColor"] = m_textColor.name();
    return obj;
}
void RectShape::fromJson(const QJsonObject& obj) {
    m_pos.setX(obj["x"].toInt());
    m_pos.setY(obj["y"].toInt());
    m_width = obj["width"].toInt(200); 
    m_height = obj["height"].toInt(100); 
    m_text = obj["text"].toString();

    QJsonObject fontObj = obj["font"].toObject();
    QFont font;
    font.setFamily(fontObj["family"].toString());
    font.setPointSize(fontObj["size"].toInt());
    font.setBold(fontObj["bold"].toBool());
    font.setItalic(fontObj["italic"].toBool());
    font.setUnderline(fontObj["underline"].toBool());
    m_font = font;

    m_textColor = QColor(obj["textColor"].toString());
}