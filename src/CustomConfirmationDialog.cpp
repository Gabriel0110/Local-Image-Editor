#include "CustomConfirmationDialog.h"

CustomConfirmationDialog::CustomConfirmationDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Confirm Mask");
    setWindowModality(Qt::NonModal);

    QVBoxLayout* layout = new QVBoxLayout(this);

    imageLabel = new QLabel(this);
    layout->addWidget(imageLabel);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    confirmButton = new QPushButton("Confirm", this);
    connect(confirmButton, &QPushButton::clicked, this, &CustomConfirmationDialog::onConfirmClicked);
    buttonLayout->addWidget(confirmButton);

    denyButton = new QPushButton("Deny", this);
    connect(denyButton, &QPushButton::clicked, this, &CustomConfirmationDialog::onDenyClicked);
    buttonLayout->addWidget(denyButton);

    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void CustomConfirmationDialog::setImage(const QImage& image) {
    QPixmap pixmap = QPixmap::fromImage(image);
    imageLabel->setPixmap(pixmap.scaled(400, 400, Qt::KeepAspectRatio));
}

void CustomConfirmationDialog::onConfirmClicked() {
    emit confirmed();
    accept();
}

void CustomConfirmationDialog::onDenyClicked() {
    emit denied();
    reject();
}
