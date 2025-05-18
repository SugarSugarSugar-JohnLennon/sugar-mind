#include "ArrowShape.h"
#include <QPainter>
#include <QtMath>
#include <QJsonArray>

ArrowShape::ArrowShape(const QPoint& start, const QPoint& end)
    : m_start(start), m_end(end)
{}

void ArrowShape::draw(QPainter& painter) {
    QPoint start = getEffectiveStart();
    QPoint end = getEffectiveEnd();

    if (auto from = fromBound.lock()) {
        auto points = from->connectionPoints();
        if (!points.isEmpty()) {
            start = points[0];
            int minDist = (end - start).manhattanLength();
            for (const auto& pt : points) {
                int d = (end - pt).manhattanLength();
                if (d < minDist) {
                    minDist = d;
                    start = pt;
                }
            }
        }
    }

    if (auto to = toBound.lock()) {
        auto points = to->connectionPoints();
        if (!points.isEmpty()) {
            end = points[0];
            int minDist = (start - end).manhattanLength();
            for (const auto& pt : points) {
                int d = (start - pt).manhattanLength();
                if (d < minDist) {
                    minDist = d;
                    end = pt;
                }
            }
        }
    }

    painter.setPen(QPen(m_color, m_lineWidth));
    painter.drawLine(start, end);

    QLineF line(start, end);
    double angle = std::atan2(line.dy(), line.dx());
    const int arrowSize = 10;
    QPointF p1 = end - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
        std::sin(angle - M_PI / 6) * arrowSize);
    QPointF p2 = end - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
        std::sin(angle + M_PI / 6) * arrowSize);
    QPolygonF arrowHead;
    arrowHead << end << p1 << p2;
    painter.setBrush(Qt::black);
    painter.drawPolygon(arrowHead);
}


bool ArrowShape::contains(const QPoint& point) {
    QLineF line(getEffectiveStart(), getEffectiveEnd());

    double dist = QLineF(line.p1(), point).length() +
        QLineF(point, line.p2()).length() -
        line.length();

    return std::abs(dist) < 6;
}

void ArrowShape::moveTo(const QPoint& pos) {
    QPoint offset = pos - m_start;
    m_start += offset;
    m_end += offset;
}

std::shared_ptr<ShapeBase> ArrowShape::clone() const {
    auto copy = std::make_shared<ArrowShape>(m_start, m_end);
    copy->setColor(m_color);
    copy->setLineWidth(m_lineWidth);
    return copy;
}

void ArrowShape::setStart(const QPoint& pt) { m_start = pt; }
void ArrowShape::setEnd(const QPoint& pt) { m_end = pt; }

QPoint ArrowShape::getStart() const { return m_start; }
QPoint ArrowShape::getEnd() const { return m_end; }

QVector<QPoint> ArrowShape::connectionPoints() const {
    return { getEffectiveStart(), getEffectiveEnd() };  
}

void ArrowShape::bindFrom(const std::shared_ptr<ShapeBase>& shape) {
    fromBound = shape;
}

void ArrowShape::bindTo(const std::shared_ptr<ShapeBase>& shape) {
    toBound = shape;
}

QPoint ArrowShape::getEffectiveStart() const {
    QPoint start = m_start;
    if (auto from = fromBound.lock()) {
        auto pts = from->connectionPoints();
        if (!pts.isEmpty()) {
            int min = (m_end - pts[0]).manhattanLength();
            for (const auto& pt : pts) {
                if ((m_end - pt).manhattanLength() < min) {
                    start = pt;
                    min = (m_end - pt).manhattanLength();
                }
            }
        }
    }
    return start;
}

QPoint ArrowShape::getEffectiveEnd() const {
    QPoint end = m_end;
    if (auto to = toBound.lock()) {
        auto pts = to->connectionPoints();
        if (!pts.isEmpty()) {
            int min = (m_start - pts[0]).manhattanLength();
            for (const auto& pt : pts) {
                if ((m_start - pt).manhattanLength() < min) {
                    end = pt;
                    min = (m_start - pt).manhattanLength();
                }
            }
        }
    }
    return end;
}

void ArrowShape::setColor(const QColor& color) {
    m_color = color;
}

QColor ArrowShape::color() const {
    return m_color;
}

void ArrowShape::setLineWidth(int width) {
    m_lineWidth = width;
}

int ArrowShape::lineWidth() const {
    return m_lineWidth;
}

QString ArrowShape::type() const {
    return "arrow";
}

QJsonObject ArrowShape::toJson() const {
    QJsonObject obj;
    obj["type"] = type();
    obj["startX"] = m_start.x();
    obj["startY"] = m_start.y();
    obj["endX"] = m_end.x();
    obj["endY"] = m_end.y();
    obj["color"] = m_color.name();
    obj["lineWidth"] = m_lineWidth;
    return obj;
}


void ArrowShape::fromJson(const QJsonObject& obj) {
    m_start.setX(obj["startX"].toInt());
    m_start.setY(obj["startY"].toInt());
    m_end.setX(obj["endX"].toInt());
    m_end.setY(obj["endY"].toInt());
    m_color = QColor(obj["color"].toString());
    m_lineWidth = obj["lineWidth"].toInt();
}
