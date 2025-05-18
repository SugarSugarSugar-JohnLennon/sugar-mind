#ifndef CANVAS_WIDGET_H_
#define CANVAS_WIDGET_H_
#include <QWidget>
#include <QLineEdit>
#include "ShapeBase.h" 
#include "RectShape.h" 
#include "CircleShape.h"
#include "TextShape.h"

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget* parent = nullptr);
    void addCircle();  
    void addRect();
    void addArrow();
    void addText();
    void setCurrentColor(const QColor& color);
    void setCurrentLineWidth(int width);
    std::shared_ptr<ShapeBase> getSelectedShape() const;
    QLineEdit* getActiveEditor() const;
    void setActiveEditor(QLineEdit* editor);
    void saveToUndoStack();  
    void undo();
    void redo();
    void exitPanMode();
    void exportToSvgDialog();
    void exportToPngDialog();
    void saveToFlowFile();
    void loadFromFlowFile();

    QColor backgroundColor = Qt::white; 
    void setCanvasBackground(const QColor& color);
    void toggleGrid();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void drawConnectionPoints(QPainter& painter, const ShapeBase& shape);
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void CanvasWidget::wheelEvent(QWheelEvent* event);
    
private:
    QVector<std::shared_ptr<ShapeBase>> shapes; 
    std::shared_ptr<ShapeBase> selectedShape = nullptr;
    bool dragging = false;
    QPoint dragOffset;  
    std::shared_ptr<ShapeBase> clipboard = nullptr;
    bool arrowDragging = false;
    bool draggingStart = false;
    bool draggingEnd = false;
    QColor currentColor = Qt::black;
    int currentLineWidth = 2;        

    QLineEdit* activeEditor = nullptr;  

    QVector<QVector<std::shared_ptr<ShapeBase>>> undoStack;
    QVector<QVector<std::shared_ptr<ShapeBase>>> redoStack;

    double scaleFactor = 1.0;      
    QPoint panOffset = QPoint(0, 0);
    bool panning = false;          
    QPoint lastPanPos;             
    bool showGrid = true;  
};

#endif