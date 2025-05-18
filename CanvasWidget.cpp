#include "CanvasWidget.h"
#include"ArrowShape.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include <QMenu>
#include <QLineEdit>
#include <QFileDialog>
#include <QPixmap>
#include <QFileDialog>
#include <QtSvg/QSvgGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>


CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: white;");
    setFocusPolicy(Qt::StrongFocus);  
}

void CanvasWidget::addCircle()
{
    saveToUndoStack(); 
    QPoint center(width() / 2, height() / 2); 
    auto circle = std::make_shared<CircleShape>(center);
    shapes.append(circle);
    update();  
}

void CanvasWidget::addRect() {
    saveToUndoStack();
    QPoint center(width() / 2, height() / 2);
    auto rect = std::make_shared<RectShape>(center);
    shapes.append(rect);
    update();
}

void CanvasWidget::addText() {
    saveToUndoStack();
    QPoint center(width() / 2, height() / 2);
    auto text = std::make_shared<TextShape>(center);
    text->setText("input..."); 
    shapes.append(text);
    update();
}

void CanvasWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    painter.translate(panOffset);
    painter.fillRect(rect(), backgroundColor);
    painter.scale(scaleFactor, scaleFactor);

    if (showGrid) {
        painter.setPen(QPen(Qt::lightGray, 1));
        for (int x = 0; x < width(); x += 20)
            painter.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += 20)
            painter.drawLine(0, y, width(), y);
    }

    for (const auto& shape : shapes) {
        shape->draw(painter);
        if (shape->isSelected()) {
            drawConnectionPoints(painter, *shape);  
        }
    }
}


void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    setFocus();  //控件会成为焦点控件，接下来来自键盘的事件（如按键、输入）都会发送到该控件
    QPoint logicPos = QPointF((event->pos() - panOffset) / scaleFactor).toPoint();
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && QApplication::keyboardModifiers() & Qt::ShiftModifier)) {
        panning = true;
        lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }


    if (event->button() == Qt::MiddleButton) {
        panning = true;
        lastPanPos = event->pos(); 
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    for (int i = shapes.size() - 1; i >= 0; --i) {
        auto arrow = std::dynamic_pointer_cast<ArrowShape>(shapes[i]);
        if (arrow) {
            QPoint start = arrow->getStart();
            QPoint end = arrow->getEnd();

            if (QRect(start - QPoint(5, 5), QSize(10, 10)).contains(logicPos)) {
                selectedShape = arrow;
                selectedShape->setSelected(true);
                update();
                arrowDragging = true;
                draggingStart = true;
                draggingEnd = false;
                return; 
            }

            if (QRect(end - QPoint(5, 5), QSize(10, 10)).contains(logicPos)) {
                selectedShape = arrow;
                selectedShape->setSelected(true);
                update();
                arrowDragging = true;
                draggingStart = false;
                draggingEnd = true;
                return; 
            }
        }
    }

    if (selectedShape) {
        selectedShape->setSelected(false);
        selectedShape = nullptr;
    }

    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i]->contains(logicPos)) {
            selectedShape = shapes[i];
            selectedShape->setSelected(true);

            if (auto arrow = std::dynamic_pointer_cast<ArrowShape>(selectedShape)) {
                if (!arrowDragging) {
                    dragging = true;
                    dragOffset = logicPos - arrow->getStart(); 
                }
            }
            else {
                dragging = true;
                dragOffset = logicPos - selectedShape->position(); 
            }

            break;
        }

    }
    update();
}

void CanvasWidget::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) && selectedShape) {
        saveToUndoStack();
        shapes.erase(std::remove(shapes.begin(), shapes.end(), selectedShape), shapes.end());
        selectedShape = nullptr;
        update();
        return;
    }

    if (event->matches(QKeySequence::Copy) && selectedShape) {
        clipboard = selectedShape->clone();
        return;
    }

    if (event->matches(QKeySequence::Cut) && selectedShape) {
        clipboard = selectedShape->clone();
        shapes.erase(std::remove(shapes.begin(), shapes.end(), selectedShape), shapes.end());
        selectedShape = nullptr;
        update();
        return;
    }

    if (event->matches(QKeySequence::Paste) && clipboard) {
        saveToUndoStack();
        auto newShape = clipboard->clone();

        if (selectedShape) {
            newShape->moveTo(selectedShape->position() + QPoint(30, 30));
        }
        else {
            newShape->moveTo(QPoint(width() / 2, height() / 2));
        }

        shapes.append(newShape);
        update();
        return;
    }

}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint logicPos = QPointF((event->pos() - panOffset) / scaleFactor).toPoint();

    if (panning) {
        QPoint delta = logicPos - lastPanPos;
        panOffset += delta;
        lastPanPos = logicPos;
        update();
        return;
    }

    if (arrowDragging && selectedShape) {
        auto arrow = std::dynamic_pointer_cast<ArrowShape>(selectedShape);
        if (!arrow) return;

        QPoint target = logicPos;

        for (const auto& shape : shapes) {
            if (shape.get() == arrow.get()) continue;

            for (const auto& anchor : shape->connectionPoints()) {
                if ((anchor - target).manhattanLength() < 15) {
                    target = anchor;

                    if (draggingStart) {
                        arrow->bindFrom(shape);
                    }
                    if (draggingEnd) {
                        arrow->bindTo(shape);
                    }

                    break;
                }
            }
        }

        if (draggingStart) {
            arrow->setStart(target);
        }
        else if (draggingEnd) {
            arrow->setEnd(target);
        }

 
        update();
    }

    if (!arrowDragging && dragging && selectedShape) {
        if (auto arrow = std::dynamic_pointer_cast<ArrowShape>(selectedShape)) {
            QPoint offset = logicPos - dragOffset;  
            arrow->moveTo(offset);
        }
        else {
            selectedShape->moveTo(logicPos - dragOffset);
        }
        update();
    }
}


void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (dragging || arrowDragging) {
        saveToUndoStack(); 
    }
    if (event->button() == Qt::MiddleButton && panning) {
        panning = false;
        setCursor(Qt::ArrowCursor);
        return;
    }

    arrowDragging = false;
    draggingStart = false;
    draggingEnd = false;
    dragging = false;

    panning = false;
    setCursor(Qt::ArrowCursor);
}

void CanvasWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QPoint logicPos = QPointF((event->pos() - panOffset) / scaleFactor).toPoint();
    selectedShape = nullptr;
    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i]->contains(logicPos)) {
            selectedShape = shapes[i];
            selectedShape->setSelected(true);
            break;
        }
    }
    if (!selectedShape)
        return;

    QMenu menu(this);
    QAction* copyAct = menu.addAction("copy");
    QAction* cutAct = menu.addAction("cut");
    QAction* pasteAct = menu.addAction("paste");
    QAction* deleteAct = menu.addAction("delete");

    QAction* chosen = menu.exec(event->globalPos());

    if (chosen == copyAct) {
        clipboard = selectedShape->clone();
    }
    else if (chosen == cutAct) {
        saveToUndoStack();
        clipboard = selectedShape->clone();
        shapes.erase(std::remove(shapes.begin(), shapes.end(), selectedShape), shapes.end());
        selectedShape = nullptr;
        update();
    }
    else if (chosen == pasteAct) {
        if (clipboard) {
            auto newShape = clipboard->clone();
            newShape->moveTo(event->pos() + QPoint(30, 30));  
            shapes.append(newShape);
            update();
        }
    }
    else if (chosen == deleteAct) {
        saveToUndoStack();
        shapes.erase(std::remove(shapes.begin(), shapes.end(), selectedShape), shapes.end());
        selectedShape = nullptr;
        update();
    }
    this->setFocus(); 
}

void CanvasWidget::drawConnectionPoints(QPainter& painter, const ShapeBase& shape) {
    painter.setPen(QPen(Qt::blue, 1));
    painter.setBrush(Qt::NoBrush); 
    const int size = 8;

    for (const QPoint& pt : shape.connectionPoints()) {
        QRect rect(pt.x() - size / 2, pt.y() - size / 2, size, size);
        painter.drawRect(rect);
    }
}

void CanvasWidget::addArrow() {
    saveToUndoStack();
    auto arrow = std::make_shared<ArrowShape>(
        QPoint(width() / 2 - 50, height() / 2),
        QPoint(width() / 2 + 50, height() / 2)
        );
    arrow->setColor(currentColor);
    arrow->setLineWidth(currentLineWidth);
    shapes.append(arrow);
    selectedShape = arrow;
    arrowDragging = true;
    draggingEnd = true;
    update();
}

void CanvasWidget::setCurrentColor(const QColor& color) {
    currentColor = color;
}
void CanvasWidget::setCurrentLineWidth(int width) {
    currentLineWidth = width;
}

std::shared_ptr<ShapeBase> CanvasWidget::getSelectedShape() const {
   return selectedShape;
}

void CanvasWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    QPoint logicPos = QPointF((event->pos() - panOffset) / scaleFactor).toPoint();
    qDebug() << "Double Click !!!!!!!";
  
    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i]->contains(logicPos)) {
            selectedShape = shapes[i];
            selectedShape->setSelected(true);
            update();

            QLineEdit* editor = new QLineEdit(this);
            setActiveEditor(editor); 

            editor->setText(selectedShape->text());
            editor->setFont(selectedShape->font());
            editor->setStyleSheet("background:white; border:1px solid gray;");
            editor->setFocus();
            editor->selectAll();

            QFontMetrics metrics(selectedShape->font());
            int textWidth = metrics.horizontalAdvance(editor->text());
            int editorWidth = qMax(textWidth + 20, 80);  
            int editorHeight = 30;

            QPoint screenPos = selectedShape->position() * scaleFactor + panOffset;
            QPoint topLeft = screenPos - QPoint(editorWidth / 2, editorHeight / 2);
            editor->setGeometry(QRect(topLeft, QSize(editorWidth, editorHeight)));

            editor->show();   
            editor->raise();   
            editor->setFocus();

            connect(editor, &QLineEdit::textChanged, this, [=](const QString& text) {
                int newWidth = qMax(QFontMetrics(editor->font()).horizontalAdvance(text) + 20, 80);
                QPoint newTopLeft = selectedShape->position() * scaleFactor + panOffset - QPoint(newWidth / 2, editorHeight / 2);
                editor->setGeometry(QRect(newTopLeft, QSize(newWidth, editorHeight)));
                });

            connect(editor, &QLineEdit::editingFinished, this, [=]() {
                saveToUndoStack();
                selectedShape->setText(editor->text());
                editor->deleteLater();
                update();
                setActiveEditor(nullptr); 
                });

            return;
        }
    }
}

QLineEdit* CanvasWidget::getActiveEditor() const {
    return activeEditor;
}

void CanvasWidget::setActiveEditor(QLineEdit* editor) {
    activeEditor = editor;
}

void CanvasWidget::saveToUndoStack() {
    QVector<std::shared_ptr<ShapeBase>> snapshot;
    for (const auto& shape : shapes) {
        snapshot.push_back(shape->clone());  
    }
    undoStack.push_back(snapshot);
    redoStack.clear(); 
}

void CanvasWidget::undo() {
    if (undoStack.isEmpty()) return;

    QVector<std::shared_ptr<ShapeBase>> current;
    for (const auto& shape : shapes)
        current.push_back(shape->clone());
    redoStack.push_back(current);

    shapes = undoStack.takeLast();
    selectedShape = nullptr;
    update();
}

void CanvasWidget::redo() {
    if (redoStack.isEmpty()) return;

    QVector<std::shared_ptr<ShapeBase>> current;
    for (const auto& shape : shapes)
        current.push_back(shape->clone());
    undoStack.push_back(current);

    shapes = redoStack.takeLast();
    selectedShape = nullptr;
    update();
}

void CanvasWidget::wheelEvent(QWheelEvent* event)
{
    constexpr double zoomInFactor = 1.1; // 扩大10%
    constexpr double zoomOutFactor = 1.0 / zoomInFactor; // 缩小10%
 
    QPointF cursorPos = event->position(); 
    double oldScale = scaleFactor;

    if (event->angleDelta().y() > 0) // 扩大
        scaleFactor *= zoomInFactor;
    else // 缩小
        scaleFactor *= zoomOutFactor;

    panOffset = (cursorPos - (cursorPos - panOffset) * (scaleFactor / oldScale)).toPoint();

    update();
}

void CanvasWidget::exitPanMode() {
    panning = false;
    setCursor(Qt::ArrowCursor); // 回复鼠标
}

void CanvasWidget::exportToPngDialog() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export as PNG", "", "PNG Files (*.png)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(size()); // 创建一张和当前画布大小一样的白纸
        render(&pixmap); // 把画布当前的所有图像都“画”到这张纸上
        pixmap.save(fileName, "PNG");
    }
}

void CanvasWidget::exportToSvgDialog() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export as SVG", "", "SVG Files (*.svg)");
    if (!fileName.isEmpty()) {
        QSvgGenerator generator; // svg对象
        generator.setFileName(fileName);
        generator.setSize(size());
        generator.setViewBox(rect());
        generator.setTitle("SVG Export");
        generator.setDescription("Created with sugar-mind");

        // 创建一个 QPainter，目标不是画在屏幕，而是画到 SVG 文件
        QPainter painter(&generator);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.scale(scaleFactor, scaleFactor);// 缩放比例
        painter.translate(panOffset);  // 平移偏移

        for (const auto& shape : shapes) {
            shape->draw(painter);
        }
    }
}

void CanvasWidget::saveToFlowFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Project", "", "Flow Files (*.flow)");
    if (fileName.isEmpty()) return;

    QJsonArray shapeArray; //jsonarray 存放shapes
    for (const auto& shape : shapes) {
        shapeArray.append(shape->toJson());  
    }

    QJsonObject root; 
    root["shapes"] = shapeArray;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}


void CanvasWidget::loadFromFlowFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Flow File", "", "Flow Files (*.flow)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    file.close();

    if (!doc.isObject()) return;
    QJsonObject root = doc.object();

    if (!root.contains("shapes") || !root["shapes"].isArray()) return;
    QJsonArray shapeArray = root["shapes"].toArray();

    shapes.clear();
    undoStack.clear();
    redoStack.clear();
    panOffset = QPoint(0, 0);
    scaleFactor = 1.0;

    for (const QJsonValue& val : shapeArray) {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();

        std::shared_ptr<ShapeBase> shape;
        if (type == "circle")
            shape = std::make_shared<CircleShape>(QPoint());
        else if (type == "rect")
            shape = std::make_shared<RectShape>(QPoint());
        else if (type == "text")
            shape = std::make_shared<TextShape>(QPoint());
        else if (type == "arrow")
            shape = std::make_shared<ArrowShape>(QPoint(), QPoint());

        if (shape) {
            shape->fromJson(obj);
            shapes.append(shape);
        }
    }

    update();
}

void CanvasWidget::setCanvasBackground(const QColor& color) {
    backgroundColor = color;
    update();
}

void CanvasWidget::toggleGrid() {
    showGrid = !showGrid;
    update();  
}