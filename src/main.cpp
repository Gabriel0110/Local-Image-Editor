#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Get the directory of the executable
    QString executableDir = QCoreApplication::applicationDirPath();

    // Try to move up two directories to the project root
    QDir::setCurrent(executableDir + "/../../");

    // Verify that the current path is at the root of the project by checking for the existence of the "resources" directory
    if (!QDir("resources").exists()) {
        QMessageBox::critical(nullptr, "Error", "The application must be run from the 'Local-Image-Editor' project root directory.");
        return -1;
    }

    // Debugging line to verify the current working directory
    qDebug() << "Current working directory: " << QDir::currentPath();

    // Create and show the main window
    MainWindow window;
    window.resize(1200, 800);
    window.show();

    return app.exec();
}
