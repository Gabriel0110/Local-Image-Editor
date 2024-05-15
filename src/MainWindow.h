#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MyOpenGLWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    MyOpenGLWidget* openGLWidget;

private slots:
    void uploadImage();
};

#endif // MAINWINDOW_H
