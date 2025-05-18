#ifndef TEXT_SHAPE_H_
#define TEXT_SHAPE_H_
#include "ShapeBase.h"
#include <QPainter>

class TextShape : public ShapeBase {
public:
    explicit TextShape(const QPoint& pos);

    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) override;
    std::shared_ptr<ShapeBase> clone() const override;
    QVector<QPoint> connectionPoints() const override;
    QString type() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};
#endif