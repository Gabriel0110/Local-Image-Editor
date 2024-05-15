#include "MainWindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // Create the OpenGL widget
    openGLWidget = new MyOpenGLWidget(this);
    setCentralWidget(openGLWidget);

    // Create the menu bar
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* fileMenu = menuBar->addMenu("File");
    QAction* uploadAction = fileMenu->addAction("Upload Image");

    connect(uploadAction, &QAction::triggered, openGLWidget, &MyOpenGLWidget::uploadImage);

    setMenuBar(menuBar);
}

void MainWindow::uploadImage() {
    openGLWidget->uploadImage();
}
