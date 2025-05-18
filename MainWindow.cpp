#include "MainWindow.h"
#include "CanvasWidget.h"
#include "ArrowShape.h"
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDebug>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDockWidget>
#include <QToolButton>
#include <QMenu>
#include <QColorDialog>
#include <QFontComboBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("sugar-mind");
    setGeometry(100, 100, 1000, 800);

    canvas = new CanvasWidget(this);
    setCentralWidget(canvas);

    createToolBar();  
    createLeftToolBar();
    createMenuBar();
}

MainWindow::~MainWindow() {}

void MainWindow::createMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu("Files");

    QAction* saveFlowAction = new QAction("Save Project (.flow)", this);
    fileMenu->addAction(saveFlowAction);
    connect(saveFlowAction, &QAction::triggered, this, [=]() {
        canvas->saveToFlowFile();
        });

    QAction* openFlowAction = new QAction("Open Flow File", this);
    fileMenu->addAction(openFlowAction);
    connect(openFlowAction, &QAction::triggered, this, [=]() {
        canvas->loadFromFlowFile();  
        });

    QAction* exportPngAction = new QAction("Export as PNG", this);
    fileMenu->addAction(exportPngAction);
    connect(exportPngAction, &QAction::triggered, this, [=]() {
        canvas->exportToPngDialog();  
        });

    QAction* exportSvgAction = new QAction("Export as SVG", this);
    fileMenu->addAction(exportSvgAction);
    connect(exportSvgAction, &QAction::triggered, this, [=]() {
        canvas->exportToSvgDialog(); 
        });
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar("Main Toolbar");

    QAction* xiahuaxianAction = new QAction(QIcon(":/images/xiahuaxian.png"), "xiahuaxian", this);
    QAction* xietiAction = new QAction(QIcon(":/images/xieti.png"), "xieti", this);
    QAction* jiacuAction = new QAction(QIcon(":/images/jiacu.png"), "jiacu", this);
    QAction* chongzuoAction = new QAction(QIcon(":/images/chongzuo.png"), "chongzuo", this);
    QAction* fanhuiAction = new QAction(QIcon(":/images/fanhui.png"), "fanhui", this);
    QAction* fontColorAction = new QAction(QIcon(":/images/font-color.png"), "fontColor", this);
    QAction* pointerAction = new QAction(QIcon(":/images/pointer.png"), "Pointer", this);

    fanhuiAction->setShortcut(QKeySequence::Undo);    
    chongzuoAction->setShortcut(QKeySequence::Redo);   

    toolBar->addAction(fanhuiAction);
    toolBar->addAction(chongzuoAction);
    toolBar->addAction(jiacuAction);
    toolBar->addAction(xietiAction);
    toolBar->addAction(xiahuaxianAction);
    toolBar->addAction(fontColorAction);
    toolBar->addAction(pointerAction);

    QToolButton* fontSizeButton = new QToolButton(this);
    fontSizeButton->setIcon(QIcon(":/images/font-size.png"));
    fontSizeButton->setToolTip("choose font size");
    fontSizeButton->setPopupMode(QToolButton::InstantPopup);
    fontSizeButton->setAutoRaise(true);
    toolBar->addWidget(fontSizeButton);

    QFontComboBox* fontCombo = new QFontComboBox(this);
    fontCombo->setToolTip("font");
    fontCombo->setMaximumWidth(150); 
    toolBar->addWidget(fontCombo);


    connect(fontCombo, &QFontComboBox::currentFontChanged, this, [=](const QFont& font) {
        if (auto editor = canvas->getActiveEditor()) {
            QFont f = editor->font();
            f.setFamily(font.family());
            editor->setFont(f);
        }
        else if (auto shape = canvas->getSelectedShape()) {
            canvas->saveToUndoStack();
            QFont f = shape->font();
            f.setFamily(font.family());
            shape->setFont(f);
            canvas->update();
        }
        });

    QToolButton* colorButton = new QToolButton(this);
    colorButton->setIcon(QIcon(":/images/xianse.png"));
    colorButton->setToolTip("chose arrow size");
    colorButton->setAutoRaise(true);
    toolBar->addWidget(colorButton);

    connect(pointerAction, &QAction::triggered, this, [=]() {
        canvas->exitPanMode();  
        });

    connect(colorButton, &QToolButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::black, this, "choose color");
        if (color.isValid()) {
            if (auto arrow = std::dynamic_pointer_cast<ArrowShape>(canvas->getSelectedShape())) {
                arrow->setColor(color);
                canvas->update();
            }
            else {
                canvas->setCurrentColor(color); 
            }
        }
        });

    connect(jiacuAction, &QAction::triggered, this, [=]() {
        if (auto editor = canvas->getActiveEditor()) {
            QFont f = editor->font();
            f.setBold(!f.bold());     
            editor->setFont(f);
        }
        else if (auto shape = canvas->getSelectedShape()) {
            QFont f = shape->font();
            f.setBold(!f.bold());
            shape->setFont(f);
            canvas->update();
        }
        });

    connect(xietiAction, &QAction::triggered, this, [=]() {
        if (auto editor = canvas->getActiveEditor()) {
            QFont f = editor->font();
            f.setItalic(!f.italic()); 
            editor->setFont(f);
        }
        else if (auto shape = canvas->getSelectedShape()) {
            QFont f = shape->font();
            f.setItalic(!f.italic());
            shape->setFont(f);
            canvas->update();
        }
        });

    connect(xiahuaxianAction, &QAction::triggered, this, [=]() {
        if (auto editor = canvas->getActiveEditor()) {
            QFont f = editor->font();
            f.setUnderline(!f.underline()); 
            editor->setFont(f);
        }
        else if (auto shape = canvas->getSelectedShape()) {
            QFont f = shape->font();
            f.setUnderline(!f.underline());
            shape->setFont(f);
            canvas->update();
        }
        });

    QToolButton* widthButton = new QToolButton(this);
    widthButton->setIcon(QIcon(":/images/xiankuan.png"));
    widthButton->setToolTip("choose arror size");
    widthButton->setPopupMode(QToolButton::InstantPopup);
    widthButton->setAutoRaise(true);
    toolBar->addWidget(widthButton);

    QMenu* widthMenu = new QMenu(this);
    QActionGroup* widthGroup = new QActionGroup(this);

    for (int w : {1, 2, 3, 4, 5}) {
        QAction* act = widthMenu->addAction(QString::number(w) + " px");
        act->setCheckable(true);
        if (w == 2) act->setChecked(true);
        widthGroup->addAction(act);

        connect(act, &QAction::triggered, this, [=]() {
            if (auto arrow = std::dynamic_pointer_cast<ArrowShape>(canvas->getSelectedShape())) {
                arrow->setLineWidth(w);
                canvas->update();
            }
            else {
                canvas->setCurrentLineWidth(w);
            }
            });
    }

    widthButton->setMenu(widthMenu);
    connect(fontColorAction, &QAction::triggered, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::black, this, "choose font color");
        if (color.isValid()) {
            if (auto editor = canvas->getActiveEditor()) {
                QPalette p = editor->palette();
                p.setColor(QPalette::Text, color);
                editor->setPalette(p);
            }
            else if (auto shape = canvas->getSelectedShape()) {
                shape->setTextColor(color);
                canvas->update();
            }
        }
        });

    QMenu* fontSizeMenu = new QMenu(this);
    for (int size : {8, 10, 12, 14, 16, 18, 20, 24, 28, 32}) {
        QAction* act = fontSizeMenu->addAction(QString::number(size) + " pt");
        connect(act, &QAction::triggered, this, [=]() {
            if (auto editor = canvas->getActiveEditor()) {
                QFont f = editor->font();
                f.setPointSize(size);
                editor->setFont(f);
            }
            else if (auto shape = canvas->getSelectedShape()) {
                QFont f = shape->font();
                f.setPointSize(size);
                shape->setFont(f);
                canvas->update();
            }
            });
    }
    fontSizeButton->setMenu(fontSizeMenu);

    connect(fanhuiAction, &QAction::triggered, this, [=]() {
        canvas->undo();
        });
    connect(chongzuoAction, &QAction::triggered, this, [=]() {
        canvas->redo();
        });

    QAction* fillBgAction = new QAction(QIcon(":/images/fill.png"), "Set Background Color", this);
    toolBar->addAction(fillBgAction);

    connect(fillBgAction, &QAction::triggered, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "background color");
        if (color.isValid()) {
            canvas->setCanvasBackground(color);
        }
        });

    QAction* gridToggleAction = new QAction(QIcon(":/images/grid.png"), "Toggle Grid", this);
    toolBar->addAction(gridToggleAction);
    connect(gridToggleAction, &QAction::triggered, canvas, &CanvasWidget::toggleGrid);
}
void MainWindow::createLeftToolBar()
{
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton* toggleButton = new QPushButton("Select Shapes Below");
    toggleButton->setCheckable(true);
    toggleButton->setChecked(true);
    toggleButton->setStyleSheet("text-align: left; padding: 4px;");

    QWidget* collapsibleWidget = new QWidget;
    QVBoxLayout* toolButtonLayout = new QVBoxLayout(collapsibleWidget);
    toolButtonLayout->setContentsMargins(4, 4, 4, 4);

    QPushButton* circleBtn = new QPushButton(QIcon(":/images/yuanxing.png"), "", this);
    QPushButton* rectBtn = new QPushButton(QIcon(":/images/juxing.png"), "", this);
    QPushButton* arrowBtn = new QPushButton(QIcon(":/images/jiantou.png"), "", this);
    QPushButton* textBtn = new QPushButton(QIcon(":/images/wenzi.png"), "", this);

    QSize iconSize(150,150);
    circleBtn->setIconSize(iconSize);
    rectBtn->setIconSize(iconSize);
    arrowBtn->setIconSize(iconSize);
    textBtn->setIconSize(iconSize);

    for (QPushButton* btn : { textBtn, rectBtn, circleBtn, arrowBtn }) {
        btn->setIconSize(iconSize);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolButtonLayout->addWidget(btn);
        toolButtonLayout->setSpacing(0);
    }


    leftLayout->addWidget(toggleButton);
    leftLayout->addWidget(collapsibleWidget);

    connect(toggleButton, &QPushButton::toggled, collapsibleWidget, &QWidget::setVisible);
    connect(circleBtn, &QPushButton::clicked, this, [=]() {
        canvas->addCircle(); 
    });
    connect(arrowBtn, &QPushButton::clicked, this, [=]() {
        canvas->addArrow();
    });
    connect(rectBtn, &QPushButton::clicked, this, [=]() {
        canvas->addRect(); 
        });
    connect(textBtn, &QPushButton::clicked, this, [=]() {
        canvas->addText();  
        });
    
    QDockWidget* leftDock = new QDockWidget("Tools", this);
    leftDock->setTitleBarWidget(new QWidget()); 
    leftDock->setWidget(leftPanel);
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);
}