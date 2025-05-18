// done

#ifndef SHAPE_BASE_H_
#define SHAPE_BASE_H_

#include <QPainter>      // 用于绘图操作
#include <QPoint>        // 表示二维坐标点
#include <QJsonObject>   // 用于 JSON 序列化与反序列化

// 图形基类，定义所有图形的共通接口与属性
class ShapeBase {
public:
    // 虚析构函数，确保子类析构函数被正确调用
    virtual ~ShapeBase() = default;

    // 纯虚函数：绘制图形。子类必须实现，传入 QPainter 用于绘图
    virtual void draw(QPainter& painter) = 0;

    // 判断某个点是否在图形内部，用于点击选择等交互逻辑
    virtual bool contains(const QPoint& point) = 0;

    // 移动图形到某个位置（左上角或中心由子类解释）
    virtual void moveTo(const QPoint& pos);

    // 获取图形的位置（通常是左上角或中心）
    QPoint position() const;

    // 设置/获取图形是否被选中（用于高亮、交互等）
    void setSelected(bool selected);
    bool isSelected() const;

    // 克隆图形对象，返回一个相同副本（用于复制粘贴等）
    virtual std::shared_ptr<ShapeBase> clone() const = 0;

    // 返回连接点列表（如用于连线功能的锚点）
    virtual QVector<QPoint> connectionPoints() const = 0;

    // 设置/获取图形上的文字
    void setText(const QString& text);
    QString text() const;

    // 设置/获取文字字体样式
    void setFont(const QFont& font);
    QFont font() const;

    // 设置/获取文字颜色
    void setTextColor(const QColor& color);
    QColor textColor() const;

    // 设置/获取文字加粗属性
    void setBold(bool bold);
    bool isBold() const;

    // 设置/获取文字斜体属性
    void setItalic(bool italic);
    bool isItalic() const;

    // 设置/获取文字下划线属性
    void setUnderline(bool underline);
    bool isUnderline() const;

    // 获取图形的类型名（如 "Circle"、"Arrow" 等，用于识别类型）
    virtual QString type() const = 0;

    // 序列化为 JSON 对象（用于保存、导出）
    virtual QJsonObject toJson() const = 0;

    // 从 JSON 对象中恢复图形属性（用于加载、导入）
    virtual void fromJson(const QJsonObject& obj) = 0;

protected:
    // 图形的位置（通常为左上角）
    QPoint m_pos;

    // 是否处于选中状态
    bool m_selected = false;

    // 图形上的文本内容
    QString m_text;

    // 文本字体，默认是 Arial 12pt
    QFont m_font = QFont("Arial", 12);

    // 文本颜色，默认黑色
    QColor m_textColor = Qt::black;
};

#endif // SHAPE_BASE_H_
