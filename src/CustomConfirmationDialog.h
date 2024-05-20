#ifndef CUSTOMCONFIRMATIONDIALOG_H
#define CUSTOMCONFIRMATIONDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class CustomConfirmationDialog : public QDialog {
    Q_OBJECT

public:
    CustomConfirmationDialog(QWidget* parent = nullptr);

    void setImage(const QImage& image);

signals:
    void confirmed();
    void denied();

private slots:
    void onConfirmClicked();
    void onDenyClicked();

private:
    QLabel* imageLabel;
    QPushButton* confirmButton;
    QPushButton* denyButton;
};

#endif // CUSTOMCONFIRMATIONDIALOG_H
