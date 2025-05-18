#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include "CanvasWidget.h"  

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void createToolBar();
    void createLeftToolBar();  
    void MainWindow::createMenuBar();

    CanvasWidget* canvas; 
};

#endif 
