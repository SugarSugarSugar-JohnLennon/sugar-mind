#ifndef RECT_SHAPE_H_
#define RECT_SHAPE_H_
#include "ShapeBase.h"

class RectShape : public ShapeBase {
public:
    explicit RectShape(const QPoint& pos);
    void RectShape::moveTo(const QPoint& pos);
    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) override;
    std::shared_ptr<ShapeBase> clone() const override;
    QVector<QPoint> connectionPoints() const override;
    QString type() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width = 200;
    int m_height = 100;
};
#endif