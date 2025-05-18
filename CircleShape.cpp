#include "CircleShape.h"

#include <cmath> 

CircleShape::CircleShape(const QPoint& pos) {
    m_pos = pos;
}

void CircleShape::draw(QPainter& painter) {
    painter.setBrush(Qt::NoBrush); //设置当前绘图工具QPainter不填充任何颜色，即只画轮廓，不填充内容
    if (isSelected()) {
        painter.setPen(QPen(Qt::blue, 2));  // 被选中蓝色， 2粗线条
    }
    else {
        painter.setPen(Qt::black); // 没选择 黑色         
    }

    painter.drawEllipse(m_pos, m_radius, m_radius); // 画出圆形，弧度默认100

    painter.setFont(m_font);        
    painter.setPen(m_textColor);    
    QRect textRect(
        m_pos.x() - m_radius,
        m_pos.y() - m_radius,   
        m_radius * 2,
        m_radius * 2
    );
    painter.drawText(textRect, Qt::AlignCenter, m_text);
}


bool CircleShape::contains(const QPoint& point) {
    return QLineF(m_pos, point).length() <= m_radius;
}

// Shape对象可以在 ： 被画布保存 / 被选中放入剪贴板 /被暂存用于撤销，重做
std::shared_ptr<ShapeBase> CircleShape::clone() const {
    auto copy = std::make_shared<CircleShape>(m_pos);
    copy->setSelected(false);
    copy->setText(this->text());              
    copy->setFont(this->font());            
    copy->setTextColor(this->textColor());  
    return copy;
}

// 渲染出连接点
QVector<QPoint> CircleShape::connectionPoints() const {
    int r = m_radius;
    return {
        m_pos + QPoint(0, -r),  
        m_pos + QPoint(0, r),    
        m_pos + QPoint(-r, 0),  
        m_pos + QPoint(r, 0)   
    };
}


QString CircleShape::type() const {
    return "circle";
}

QJsonObject CircleShape::toJson() const {
    QJsonObject obj;
    obj["type"] = type();
    obj["x"] = m_pos.x();
    obj["y"] = m_pos.y();
    obj["radius"] = m_radius;
    obj["text"] = m_text;

    QJsonObject fontObj;
    fontObj["family"] = m_font.family();
    fontObj["size"] = m_font.pointSize();
    fontObj["bold"] = m_font.bold();
    fontObj["italic"] = m_font.italic();
    fontObj["underline"] = m_font.underline();
    obj["font"] = fontObj;

    obj["textColor"] = m_textColor.name();
    return obj;
}

void CircleShape::fromJson(const QJsonObject& obj) {
    m_pos.setX(obj["x"].toInt());
    m_pos.setY(obj["y"].toInt());
    m_radius = obj["radius"].toInt(100); 
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