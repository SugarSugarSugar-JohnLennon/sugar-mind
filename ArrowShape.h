#ifndef ARROW_SHAPE_H_
#define ARROW_SHAPE_H_
#include "ShapeBase.h"

class ArrowShape : public ShapeBase {
public:
    ArrowShape(const QPoint& start, const QPoint& end);

    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) override;
    void moveTo(const QPoint& pos) override;  
    std::shared_ptr<ShapeBase> clone() const override;
    QVector<QPoint> connectionPoints() const override;

    void setStart(const QPoint& pt);
    void setEnd(const QPoint& pt);

    QPoint getStart() const;
    QPoint getEnd() const;

    void bindFrom(const std::shared_ptr<ShapeBase>& shape);
    void bindTo(const std::shared_ptr<ShapeBase>& shape);
    QPoint getEffectiveStart() const;
    QPoint getEffectiveEnd() const;
    void setColor(const QColor& color);
    QColor color() const;

    void setLineWidth(int width);
    int lineWidth() const;

    QString type() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QPoint m_start;
    QPoint m_end;
    std::weak_ptr<ShapeBase> fromBound;
    std::weak_ptr<ShapeBase> toBound;
    QColor m_color = Qt::black;
    int m_lineWidth = 2;
};

#endif