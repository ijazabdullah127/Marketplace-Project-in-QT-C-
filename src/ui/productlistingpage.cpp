#include "productlistingpage.h"
#include "../database/databasemanager.h"
#include "../auth/authmanager.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QComboBox>
#include <QHBoxLayout>
#include <QScrollArea>

ProductListingPage::ProductListingPage(QWidget *parent)
    : ProtectedPage(parent)
    , nameEdit(nullptr)
    , descriptionEdit(nullptr)
    , priceSpinBox(nullptr)
    , selectImageButton(nullptr)
    , imagePreviewLabel(nullptr)
    , addProductButton(nullptr)
    , mainLayout(nullptr)
    , selectedImagePath("")
    , priceEdit(nullptr)
    , categoryEdit(nullptr)
    , stockSpinBox(nullptr)
    , imageData()
    , imageUrl("")
{
    setupUI();
}

void ProductListingPage::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Navigation buttons at the top
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setAlignment(Qt::AlignRight);
    navLayout->setSpacing(10);
    
    QPushButton* homeBtn = new QPushButton("🏠 Home", this);
    QPushButton* logoutBtn = new QPushButton("🚪 Logout", this);
    
    QString navButtonStyle =
        "QPushButton {"
        "    background-color: %1;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %2;"
        "}";
    
    homeBtn->setStyleSheet(navButtonStyle.arg("#2ecc71", "#27ae60"));
    logoutBtn->setStyleSheet(navButtonStyle.arg("#e74c3c", "#c0392b"));
    
    connect(homeBtn, &QPushButton::clicked, this, [this]() { emit navigateHome(); });
    connect(logoutBtn, &QPushButton::clicked, this, [this]() { emit logout(); });
    
    navLayout->addWidget(homeBtn);
    navLayout->addWidget(logoutBtn);
    mainLayout->addLayout(navLayout);
    
    // Page title
    QLabel* titleLabel = new QLabel("📝 List New Product", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    padding: 10px 0;"
        "    border-bottom: 2px solid #3498db;"
        "    margin-bottom: 20px;"
        "}"
    );
    mainLayout->addWidget(titleLabel);

    // Create scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    border: none;"
        "    background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #f0f0f0;"
        "    width: 10px;"
        "    margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #bdc3c7;"
        "    min-height: 20px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: none;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );

    // Form container
    QWidget* formContainer = new QWidget(scrollArea);
    formContainer->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 10px;"
        "}"
    );
    QVBoxLayout* formContainerLayout = new QVBoxLayout(formContainer);
    formContainerLayout->setSpacing(20);
    formContainerLayout->setContentsMargins(20, 20, 20, 20);

    // Product Information Section
    QGroupBox* infoGroup = new QGroupBox("Product Information", formContainer);
    infoGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    margin-top: 10px;"
        "    background-color: white;"
        "    padding: 20px;"
        "}"
        "QGroupBox::title {"
        "    color: #2980b9;"
        "    padding: 0 10px;"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 10px;"
        "}"
    );
    QFormLayout* formLayout = new QFormLayout(infoGroup);
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(15, 25, 15, 15);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);

    // Label style
    QString labelStyle = 
        "QLabel {"
        "    color: #34495e;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}";

    // Input field style
    QString inputStyle = 
        "QLineEdit, QTextEdit, QSpinBox, QComboBox {"
        "    padding: 10px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    background-color: white;"
        "    font-size: 14px;"
        "    min-width: 300px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit::placeholder, QTextEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QComboBox:focus {"
        "    border: 2px solid #3498db;"
        "    background-color: #f8f9fa;"
        "    color: #2c3e50;"
        "}";

    // Name field
    QLabel* nameLabel = new QLabel("Product Name:", infoGroup);
    nameLabel->setStyleSheet(labelStyle);
    nameEdit = new QLineEdit(infoGroup);
    nameEdit->setPlaceholderText("Enter product name");
    nameEdit->setStyleSheet(inputStyle);
    formLayout->addRow(nameLabel, nameEdit);

    // Description field
    QLabel* descLabel = new QLabel("Description:", infoGroup);
    descLabel->setStyleSheet(labelStyle);
    descriptionEdit = new QTextEdit(infoGroup);
    descriptionEdit->setPlaceholderText("Enter product description");
    descriptionEdit->setStyleSheet(inputStyle);
    descriptionEdit->setMinimumHeight(100);
    formLayout->addRow(descLabel, descriptionEdit);

    // Price field
    QLabel* priceLabel = new QLabel("Price ($):", infoGroup);
    priceLabel->setStyleSheet(labelStyle);
    priceEdit = new QLineEdit(infoGroup);
    priceEdit->setPlaceholderText("Enter price");
    priceEdit->setStyleSheet(inputStyle);
    formLayout->addRow(priceLabel, priceEdit);

    // Category dropdown
    QLabel* categoryLabel = new QLabel("Category:", infoGroup);
    categoryLabel->setStyleSheet(labelStyle);
    categoryEdit = new QComboBox(infoGroup);
    categoryEdit->addItems({"Electronics", "Clothing", "Books", "Home & Garden", "Sports", "Other"});
    categoryEdit->setStyleSheet(
        "QComboBox {"
        "    padding: 8px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    background-color: white;"
        "    color: #2c3e50;"
        "    min-width: 200px;"
        "}"
        "QComboBox:hover {"
        "    border-color: #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    padding-right: 10px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: url(:/images/down-arrow.png);"
        "    width: 12px;"
        "    height: 12px;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    background-color: white;"
        "    color: #2c3e50;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "    padding: 5px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "    min-height: 25px;"
        "    padding: 5px;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "    background-color: #ecf0f1;"
        "}"
    );
    formLayout->addRow(categoryLabel, categoryEdit);

    // Stock field
    QLabel* stockLabel = new QLabel("Stock Quantity:", infoGroup);
    stockLabel->setStyleSheet(labelStyle);
    stockSpinBox = new QSpinBox(infoGroup);
    stockSpinBox->setRange(0, 9999);
    stockSpinBox->setStyleSheet(inputStyle);
    formLayout->addRow(stockLabel, stockSpinBox);

    formContainerLayout->addWidget(infoGroup);

    // Image Section
    QGroupBox* imageGroup = new QGroupBox("Product Image", formContainer);
    imageGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    margin-top: 10px;"
        "    background-color: white;"
        "    padding: 20px;"
        "}"
        "QGroupBox::title {"
        "    color: #2980b9;"
        "    padding: 0 10px;"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    left: 10px;"
        "}"
    );
    QVBoxLayout* imageLayout = new QVBoxLayout(imageGroup);
    imageLayout->setSpacing(15);
    imageLayout->setContentsMargins(15, 25, 15, 15);

    // Image preview
    imagePreviewLabel = new QLabel(imageGroup);
    imagePreviewLabel->setMinimumSize(300, 300);
    imagePreviewLabel->setMaximumSize(300, 300);
    imagePreviewLabel->setAlignment(Qt::AlignCenter);
    imagePreviewLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #f8f9fa;"
        "    border: 2px dashed #bdc3c7;"
        "    border-radius: 8px;"
        "    color: #95a5a6;"
        "    font-size: 14px;"
        "}"
    );
    imagePreviewLabel->setText("No image selected\nClick 'Select Image' to add");
    imageLayout->addWidget(imagePreviewLabel, 0, Qt::AlignCenter);

    // Select image button
    selectImageButton = new QPushButton("📷 Select Image", imageGroup);
    selectImageButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 12px 25px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
    );
    connect(selectImageButton, &QPushButton::clicked, this, &ProductListingPage::onSelectImageClicked);
    imageLayout->addWidget(selectImageButton, 0, Qt::AlignCenter);

    formContainerLayout->addWidget(imageGroup);

    // Add Product button
    addProductButton = new QPushButton("✅ Add Product", formContainer);
    addProductButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 15px 30px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    min-width: 200px;"
        "    margin-top: 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
    );
    connect(addProductButton, &QPushButton::clicked, this, &ProductListingPage::onAddProductClicked);
    formContainerLayout->addWidget(addProductButton, 0, Qt::AlignCenter);

    // Set the form container as the scroll area widget
    scrollArea->setWidget(formContainer);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
}

void ProductListingPage::onSelectImageClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Select Product Image",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp)");
        
    if (!filePath.isEmpty()) {
        selectedImagePath = filePath;
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            imagePreviewLabel->setPixmap(pixmap.scaled(
                imagePreviewLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            ));
        }
    }
}

void ProductListingPage::onAddProductClicked()
{
    if (!checkAccess()) {
        qDebug() << "Access check failed";
        return;
    }
    
    QString name = nameEdit->text().trimmed();
    QString description = descriptionEdit->toPlainText().trimmed();
    QString priceText = priceEdit->text().trimmed();
    double price = priceText.toDouble();
    QString category = categoryEdit->currentText().trimmed();
    int stock = stockSpinBox->value();
    
    qDebug() << "Product details before validation:";
    qDebug() << "Name:" << name;
    qDebug() << "Description:" << description;
    qDebug() << "Price text:" << priceText << "Converted price:" << price;
    qDebug() << "Category:" << category;
    qDebug() << "Stock:" << stock;
    qDebug() << "Image path:" << selectedImagePath;
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a product name");
        return;
    }
    
    if (description.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a product description");
        return;
    }
    
    if (price <= 0) {
        QMessageBox::warning(this, "Error", "Please enter a valid product price");
        return;
    }
    
    if (category.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a product category");
        return;
    }
    
    if (selectedImagePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a product image");
        return;
    }
    
    // Convert image to QByteArray for database storage
    QImage image(selectedImagePath);
    if (image.isNull()) {
        qDebug() << "Failed to load image from path:" << selectedImagePath;
        QMessageBox::warning(this, "Error", "Failed to load selected image");
        return;
    }
    
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "PNG")) {
        qDebug() << "Failed to save image to buffer";
        QMessageBox::warning(this, "Error", "Failed to process image");
        return;
    }
    
    // Create product object
    Product product;
    product.name = name;
    product.description = description;
    product.price = price;
    product.category = category;
    product.stock = stock;
    product.imageData = imageData;
    product.sellerId = AuthManager::getInstance().getCurrentUserId();
    
    qDebug() << "Product object created:";
    qDebug() << "Name:" << product.name;
    qDebug() << "Price:" << product.price;
    qDebug() << "Stock:" << product.stock;
    qDebug() << "SellerId:" << product.sellerId;
    qDebug() << "Image data size:" << product.imageData.size();
    
    // Add product to database
    if (DatabaseManager::getInstance().addProduct(product)) {
        qDebug() << "Product added successfully";
        onProductAddedSuccess();
    } else {
        qDebug() << "Failed to add product to database";
        onProductAddedFailed("Failed to add product to database");
    }
}

void ProductListingPage::onProductAddedSuccess()
{
    QMessageBox::information(this, "Success", "Product added successfully!");
    clearForm();
}

void ProductListingPage::onProductAddedFailed(const QString& error)
{
    QMessageBox::warning(this, "Error", "Failed to add product: " + error);
}

void ProductListingPage::clearForm()
{
    nameEdit->clear();
    descriptionEdit->clear();
    priceEdit->clear();
    categoryEdit->setCurrentIndex(0);
    stockSpinBox->setValue(0);
    selectedImagePath.clear();
    imagePreviewLabel->setText("No image selected");
    imageData.clear();
} 