#ifndef CIRCLE_SHAPE_H_
#define CIRCLE_SHAPE_H_

#include "ShapeBase.h"

class CircleShape : public ShapeBase {
public:
    explicit CircleShape(const QPoint& pos);

    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) override;
    std::shared_ptr<ShapeBase> clone() const override;

    QVector<QPoint> connectionPoints() const override;

    QString type() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
    
    int getRadius() const { return m_radius; }

private:
    int m_radius = 100;
};
#endif