#ifndef PRODUCTLISTINGPAGE_H
#define PRODUCTLISTINGPAGE_H

#include "protectedpage.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include "../database/databasemanager.h"

class ProductListingPage : public ProtectedPage {
    Q_OBJECT

public:
    explicit ProductListingPage(QWidget *parent = nullptr);

private slots:
    void onAddProductClicked();
    void onSelectImageClicked();
    void onProductAddedSuccess();
    void onProductAddedFailed(const QString& error);

private:
    void setupUI();
    void clearForm();

    QLineEdit* nameEdit;
    QTextEdit* descriptionEdit;
    QDoubleSpinBox* priceSpinBox;
    QPushButton* selectImageButton;
    QLabel* imagePreviewLabel;
    QPushButton* addProductButton;
    QVBoxLayout* mainLayout;
    QString selectedImagePath;
    QLineEdit *priceEdit;
    QComboBox *categoryEdit;
    QSpinBox *stockSpinBox;
    QPushButton *imageUploadButton;
    QByteArray imageData;
    QString imageUrl;
};

#endif // PRODUCTLISTINGPAGE_H 