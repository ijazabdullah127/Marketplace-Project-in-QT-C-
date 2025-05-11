#include "productbrowsepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QMouseEvent>
#include <QDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include "../database/databasemanager.h"
#include "../auth/authmanager.h"

// ProductWidget implementation
ProductWidget::ProductWidget(const Product& product, QWidget* parent)
    : QWidget(parent)
    , product(product)
    , imageLabel(nullptr)
    , nameLabel(nullptr)
    , priceLabel(nullptr)
    , ratingLabel(nullptr)
    , dbManager(DatabaseManager::getInstance())
{
    setupUI();
}

void ProductWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(product);
    }
    QWidget::mousePressEvent(event);
}

void ProductWidget::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(8);
    layout->setContentsMargins(15, 15, 15, 15);
    
    // Image container with fixed size and shadow effect
    QWidget* imageContainer = new QWidget(this);
    imageContainer->setFixedSize(220, 220);
    imageContainer->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 10px;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
    QVBoxLayout* imageLayout = new QVBoxLayout(imageContainer);
    imageLayout->setContentsMargins(10, 10, 10, 10);
    
    imageLabel = new QLabel(imageContainer);
    imageLabel->setAlignment(Qt::AlignCenter);
    QPixmap pixmap;
    if (!product.imageData.isEmpty()) {
        pixmap.loadFromData(product.imageData);
    } else {
        pixmap = QPixmap(":/images/no-image.png");
    }
    pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);
    imageLayout->addWidget(imageLabel);
    layout->addWidget(imageContainer);
    
    // Category label with background
    QLabel* categoryLabel = new QLabel(product.category, this);
    categoryLabel->setAlignment(Qt::AlignCenter);
    categoryLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #f0f0f0;"
        "    color: #666666;"
        "    padding: 4px 8px;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
    );
    layout->addWidget(categoryLabel);
    
    // Product name with improved visibility
    nameLabel = new QLabel(product.name, this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "QLabel {"
        "    color: #2c3e50;"
        "    font-weight: bold;"
        "    font-size: 16px;"
        "    margin: 5px 0;"
        "}"
    );
    nameLabel->setWordWrap(true);
    layout->addWidget(nameLabel);
    
    // Seller name with icon
    QString sellerName = dbManager.getUserById(product.sellerId).getUsername();
    QLabel* sellerLabel = new QLabel(QString("👤 %1").arg(sellerName), this);
    sellerLabel->setAlignment(Qt::AlignCenter);
    sellerLabel->setStyleSheet(
        "QLabel {"
        "    color: #7f8c8d;"
        "    font-size: 13px;"
        "}"
    );
    layout->addWidget(sellerLabel);
    
    // Price with improved visibility
    priceLabel = new QLabel(QString("$%1").arg(product.price, 0, 'f', 2), this);
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet(
        "QLabel {"
        "    color: #27ae60;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    margin: 5px 0;"
        "}"
    );
    layout->addWidget(priceLabel);
    
    // Stock information with icon
    QLabel* quantityLabel = new QLabel(QString("📦 Available: %1").arg(product.stock), this);
    quantityLabel->setAlignment(Qt::AlignCenter);
    quantityLabel->setStyleSheet(
        "QLabel {"
        "    color: #e74c3c;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
    );
    layout->addWidget(quantityLabel);
    
    // Rating with improved visibility
    double avgRating = dbManager.getProductAverageRating(product.id);
    QList<Review> reviews = dbManager.getProductReviews(product.id);
    QString ratingText = avgRating > 0 
        ? QString("★ %1/5 (%2 reviews)").arg(avgRating, 0, 'f', 1).arg(reviews.size())
        : "No reviews yet";
    ratingLabel = new QLabel(ratingText, this);
    ratingLabel->setAlignment(Qt::AlignCenter);
    ratingLabel->setStyleSheet(
        "QLabel {"
        "    color: #f39c12;"
        "    font-size: 13px;"
        "    margin: 5px 0;"
        "}"
    );
    layout->addWidget(ratingLabel);
    
    // Buttons container with improved styling
    QWidget* buttonsContainer = new QWidget(this);
    QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsContainer);
    buttonsLayout->setSpacing(8);
    
    // Add to Cart button with improved styling
    QPushButton* addToCartButton = new QPushButton("🛒 Add to Cart", this);
    addToCartButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #219a52;"
        "}"
    );
    connect(addToCartButton, &QPushButton::clicked, [this]() {
        emit addToCartClicked(product);
    });
    buttonsLayout->addWidget(addToCartButton);
    
    // View Reviews button with improved styling
    QPushButton* viewReviewsBtn = new QPushButton("⭐ View Reviews", this);
    viewReviewsBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2472a4;"
        "}"
    );
    connect(viewReviewsBtn, &QPushButton::clicked, this, &ProductWidget::showReviewsDialog);
    buttonsLayout->addWidget(viewReviewsBtn);
    
    layout->addWidget(buttonsContainer);
    
    // Main widget styling
    setLayout(layout);
    setFixedSize(300, 520); // Increased size for better spacing
    setStyleSheet(
        "ProductWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 12px;"
        "    padding: 5px;"
        "}"
        "ProductWidget:hover {"
        "    border: 2px solid #3498db;"
        "    background-color: #f8f9fa;"
        "}"
    );
    setCursor(Qt::PointingHandCursor);
}

void ProductWidget::showReviewsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString("Reviews for %1").arg(product.name));
    dialog.setModal(true);
    dialog.resize(500, 400);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Product info section
    QHBoxLayout* productInfoLayout = new QHBoxLayout();
    
    // Product image
    QLabel* productImage = new QLabel(&dialog);
    QPixmap pixmap;
    if (!product.imageData.isEmpty()) {
        pixmap.loadFromData(product.imageData);
    } else {
        pixmap = QPixmap(":/images/no-image.png");
    }
    pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    productImage->setPixmap(pixmap);
    productInfoLayout->addWidget(productImage);
    
    // Product details
    QVBoxLayout* productDetailsLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel(product.name, &dialog);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    productDetailsLayout->addWidget(nameLabel);
    
    double avgRating = dbManager.getProductAverageRating(product.id);
    QList<Review> reviews = dbManager.getProductReviews(product.id);
    
    QString ratingText = avgRating > 0 
        ? QString("★ %1/5 (%2 reviews)").arg(avgRating, 0, 'f', 1).arg(reviews.size())
        : "No reviews yet";
    QLabel* ratingLabel = new QLabel(ratingText, &dialog);
    ratingLabel->setStyleSheet("color: #f39c12;");
    productDetailsLayout->addWidget(ratingLabel);
    
    productInfoLayout->addLayout(productDetailsLayout);
    productInfoLayout->addStretch();
    layout->addLayout(productInfoLayout);
    
    // Add Review button (only if user is logged in)
    AuthManager& authManager = AuthManager::getInstance();
    if (authManager.getCurrentUserId() != -1) {
        QPushButton* addReviewBtn = new QPushButton("Write a Review", &dialog);
        addReviewBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #2ecc71;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px;"
            "    border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #27ae60;"
            "}"
        );
        connect(addReviewBtn, &QPushButton::clicked, [this, &dialog]() {
            dialog.close();
            showAddReviewDialog();
        });
        layout->addWidget(addReviewBtn);
    }
    
    // Separator line
    QFrame* line = new QFrame(&dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);
    
    // Reviews section
    QScrollArea* scrollArea = new QScrollArea(&dialog);
    QWidget* scrollContent = new QWidget(scrollArea);
    QVBoxLayout* reviewsLayout = new QVBoxLayout(scrollContent);
    
    setupReviewsSection(reviewsLayout, reviews);
    
    reviewsLayout->addStretch();
    scrollContent->setLayout(reviewsLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);
    
    // Close button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    dialog.setLayout(layout);
    dialog.exec();
}

void ProductWidget::setupReviewsSection(QVBoxLayout* layout, const QList<Review>& reviews) {
    for (const Review& review : reviews) {
        QFrame* reviewCard = new QFrame();
        reviewCard->setFrameStyle(QFrame::StyledPanel);
        reviewCard->setStyleSheet("QFrame { background-color: #f5f5f5; border-radius: 8px; margin: 4px; padding: 8px; }"
                                "QLabel { color: #333333; }");

        QVBoxLayout* reviewLayout = new QVBoxLayout(reviewCard);

        // Header with user and date
        QHBoxLayout* headerLayout = new QHBoxLayout();
        QLabel* userLabel = new QLabel(review.username);
        userLabel->setStyleSheet("font-weight: bold; color: #333333;");
        
        QLabel* dateLabel = new QLabel(review.reviewDate.toString("yyyy-MM-dd"));
        dateLabel->setStyleSheet("color: #666666;");
        
        headerLayout->addWidget(userLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(dateLabel);

        // Rating display
        QString stars;
        for (int i = 0; i < review.rating; i++) stars += "★";
        for (int i = review.rating; i < 5; i++) stars += "☆";
        QLabel* ratingLabel = new QLabel(stars);
        ratingLabel->setStyleSheet("color: #ffd700;"); // Gold color for stars

        // Review text
        QLabel* commentLabel = new QLabel(review.comment);
        commentLabel->setWordWrap(true);
        commentLabel->setStyleSheet("color: #333333; margin-top: 4px;");
        
        reviewLayout->addLayout(headerLayout);
        reviewLayout->addWidget(ratingLabel);
        reviewLayout->addWidget(commentLabel);

        layout->addWidget(reviewCard);
    }

    if (reviews.isEmpty()) {
        QLabel* noReviewsLabel = new QLabel("No reviews yet");
        noReviewsLabel->setStyleSheet("color: #666666; padding: 20px;");
        noReviewsLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(noReviewsLabel);
    }
}

void ProductWidget::showAddReviewDialog()
{
    AuthManager& authManager = AuthManager::getInstance();
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        QMessageBox::warning(this, "Login Required", "Please log in to write a review.");
        return;
    }
    
    // Check if user has already reviewed this product
    Review existingReview = dbManager.getUserProductReview(userId, product.id);
    
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(existingReview.id != -1 ? "Update Review" : "Write a Review");
    dialog->setModal(true);
    
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    
    // Rating
    QHBoxLayout* ratingLayout = new QHBoxLayout();
    QLabel* ratingLabel = new QLabel("Rating:", dialog);
    QSpinBox* ratingSpinBox = new QSpinBox(dialog);
    ratingSpinBox->setRange(1, 5);
    ratingSpinBox->setValue(existingReview.id != -1 ? existingReview.rating : 5);
    ratingSpinBox->setPrefix("★ ");
    ratingLayout->addWidget(ratingLabel);
    ratingLayout->addWidget(ratingSpinBox);
    layout->addLayout(ratingLayout);
    
    // Comment
    QLabel* commentLabel = new QLabel("Your Review:", dialog);
    layout->addWidget(commentLabel);
    
    QTextEdit* commentEdit = new QTextEdit(dialog);
    commentEdit->setPlaceholderText("Write your review here...");
    if (existingReview.id != -1) {
        commentEdit->setText(existingReview.comment);
    }
    layout->addWidget(commentEdit);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        dialog
    );
    
    // Connect the accepted signal to a lambda that validates and submits the review
    connect(buttonBox, &QDialogButtonBox::accepted, [this, dialog, ratingSpinBox, commentEdit, existingReview, userId, &authManager]() {
        QString comment = commentEdit->toPlainText().trimmed();
        if (comment.isEmpty()) {
            QMessageBox::warning(dialog, "Validation Error", "Please write a review comment.");
            return;
        }
        
        Review review;
        review.id = existingReview.id;
        review.productId = product.id;
        review.userId = userId;
        review.username = authManager.getCurrentUserEmail();
        review.rating = ratingSpinBox->value();
        review.comment = comment;
        review.reviewDate = QDateTime::currentDateTime();
        
        bool success;
        if (existingReview.id != -1) {
            success = dbManager.updateReview(review);
        } else {
            success = dbManager.addReview(review);
        }
        
        if (success) {
            QMessageBox::information(dialog, "Success",
                existingReview.id != -1 ? "Review updated successfully!" : "Review added successfully!");
            dialog->accept();
           // emit reviewAdded();
        } else {
            QMessageBox::warning(dialog, "Error", "Failed to save review. Please try again.");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    dialog->exec();
    dialog->deleteLater();
}

ProductBrowsePage::ProductBrowsePage(QWidget *parent)
    : ProtectedPage(parent)
    , networkManager(new QNetworkAccessManager(this))
    , dbManager(DatabaseManager::getInstance())
    , authManager(AuthManager::getInstance())
{
    setupUI();
    fetchProducts();
}

void ProductBrowsePage::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
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

    // Create filter and search controls with modern styling
    QWidget* controlsContainer = new QWidget(this);
    controlsContainer->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "}"
    );
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsContainer);
    controlsLayout->setSpacing(15);

    QString controlStyle = 
        "QComboBox, QLineEdit {"
        "    padding: 8px 15px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    color: #2c3e50;"
        "    font-size: 14px;"
        "    min-width: 150px;"
        "}"
        "QComboBox:focus, QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    padding-right: 10px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: url(:/images/down-arrow.png);"
        "    width: 12px;"
        "    height: 12px;"
        "}";

    categoryFilter = new QComboBox(this);
    categoryFilter->addItem("All Categories");
    categoryFilter->addItems({"Electronics", "Clothing", "Books", "Home & Garden", "Sports", "Other"});
    categoryFilter->setStyleSheet(R"(
        QComboBox {
            padding: 5px 10px;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            background: white;
            color: #2c3e50;
            min-width: 150px;
        }
        QComboBox::drop-down {
            border: none;
            padding-right: 5px;
        }
        QComboBox::down-arrow {
            image: url(:/icons/dropdown-arrow.png);
            width: 12px;
            height: 12px;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            background: white;
            color: #2c3e50;
            selection-background-color: #ecf0f1;
            selection-color: #2c3e50;
        }
    )");

    sortComboBox = new QComboBox(this);
    sortComboBox->addItems({"Price: Low to High", "Price: High to Low", "Name: A to Z"});
    sortComboBox->setStyleSheet(R"(
        QComboBox {
            padding: 5px 10px;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            background: white;
            color: #2c3e50;
            min-width: 150px;
        }
        QComboBox::drop-down {
            border: none;
            padding-right: 5px;
        }
        QComboBox::down-arrow {
            image: url(:/icons/dropdown-arrow.png);
            width: 12px;
            height: 12px;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            background: white;
            color: #2c3e50;
            selection-background-color: #ecf0f1;
            selection-color: #2c3e50;
        }
    )");

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("🔍 Search products...");
    searchEdit->setStyleSheet(controlStyle);

    controlsLayout->addWidget(categoryFilter);
    controlsLayout->addWidget(sortComboBox);
    controlsLayout->addWidget(searchEdit);
    mainLayout->addWidget(controlsContainer);

    // Create scroll area for products
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

    productsContainer = new QWidget(scrollArea);
    productsContainer->setStyleSheet(
        "QWidget {"
        "    background: transparent;"
        "}"
    );
    
    // Use QGridLayout instead of QVBoxLayout for products
    productsLayout = new QGridLayout(productsContainer);
    productsLayout->setSpacing(20);
    productsLayout->setContentsMargins(0, 20, 0, 20);
    scrollArea->setWidget(productsContainer);
    mainLayout->addWidget(scrollArea);

    connect(categoryFilter, &QComboBox::currentTextChanged, this, &ProductBrowsePage::onFilterChanged);
    connect(sortComboBox, &QComboBox::currentTextChanged, this, &ProductBrowsePage::onSortChanged);
    connect(searchEdit, &QLineEdit::textChanged, this, &ProductBrowsePage::onSearchTextChanged);
}

void ProductBrowsePage::fetchProducts()
{
    QList<Product> fetchedProducts = dbManager.getAllProducts();
    
    if (!fetchedProducts.isEmpty()) {
        QVector<Product> productsVector;
        for (const Product& product : fetchedProducts) {
            productsVector.append(product);
        }
        onProductsFetchedSuccess(productsVector);
    } else {
        onProductsFetchedFailed("No products found in database");
    }
}

void ProductBrowsePage::onProductsFetchedSuccess(const QVector<Product>& fetchedProducts)
{
    products = fetchedProducts;
    filteredProducts = products;  // Initialize filtered products with all products
    displayProducts();
}

void ProductBrowsePage::onProductsFetchedFailed(const QString& error)
{
    QMessageBox::critical(this, "Error", "Failed to fetch products: " + error);
}

void ProductBrowsePage::displayProducts()
{
    // Clear existing widgets
    clearProducts();
    
    int row = 0;
    int col = 0;
    int maxColumns = 3; // Display 3 products per row
    
    for (const Product& product : filteredProducts) {
        ProductWidget* widget = new ProductWidget(product, productsContainer);
        connect(widget, &ProductWidget::clicked, this, &ProductBrowsePage::handleProductClicked);
        connect(widget, &ProductWidget::addToCartClicked, this, &ProductBrowsePage::showAddToCartDialog);
        connect(widget, &ProductWidget::reviewAdded, this, &ProductBrowsePage::handleReviewAdded);
        
        productWidgets[product.id] = widget;
        productsLayout->addWidget(widget, row, col);
        
        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }
    
    // Add stretch to center the grid if not enough products to fill the last row
    if (col != 0) {
        for (; col < maxColumns; col++) {
            productsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), row, col);
        }
    }
}

void ProductBrowsePage::clearProducts()
{
    for (QWidget* widget : productWidgets.values()) {
        productsLayout->removeWidget(widget);
        delete widget;
    }
    productWidgets.clear();
}

void ProductBrowsePage::onFilterChanged()
{
    QString category = categoryFilter->currentText();
    filteredProducts.clear();
    
    if (category == "All Categories") {
        filteredProducts = products;
    } else {
        for (const Product& product : products) {
            if (product.category == category) {
                filteredProducts.append(product);
            }
        }
    }
    
    displayProducts();
}

void ProductBrowsePage::onSortChanged()
{
    QString sortBy = sortComboBox->currentText();
    
    if (sortBy == "Price: Low to High") {
        std::sort(filteredProducts.begin(), filteredProducts.end(), 
            [](const Product& a, const Product& b) { return a.price < b.price; });
    }
    else if (sortBy == "Price: High to Low") {
        std::sort(filteredProducts.begin(), filteredProducts.end(), 
            [](const Product& a, const Product& b) { return a.price > b.price; });
    }
    else if (sortBy == "Name: A to Z") {
        std::sort(filteredProducts.begin(), filteredProducts.end(), 
            [](const Product& a, const Product& b) { return a.name < b.name; });
    }
    
    displayProducts();
}

void ProductBrowsePage::onSearchTextChanged(const QString& text)
{
    filteredProducts.clear();
    
    if (text.isEmpty()) {
        filteredProducts = products;
    } else {
        for (const Product& product : products) {
            if (product.name.contains(text, Qt::CaseInsensitive) ||
                product.description.contains(text, Qt::CaseInsensitive)) {
                filteredProducts.append(product);
            }
        }
    }
    
    displayProducts();
}

void ProductBrowsePage::showAddToCartDialog(const Product& product)
{
    if (!authManager.isAuthenticated()) {
        QMessageBox::warning(this, "Authentication Required", "Please log in to add items to your cart.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add to Cart");
    dialog.setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Product info
    QLabel* nameLabel = new QLabel(product.name, &dialog);
    QLabel* priceLabel = new QLabel(QString("Price: $%1").arg(product.price, 0, 'f', 2), &dialog);
    
    // Quantity selector
    QHBoxLayout* quantityLayout = new QHBoxLayout();
    QLabel* quantityLabel = new QLabel("Quantity:", &dialog);
    QSpinBox* quantitySpinBox = new QSpinBox(&dialog);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(99);
    quantitySpinBox->setValue(1);
    quantityLayout->addWidget(quantityLabel);
    quantityLayout->addWidget(quantitySpinBox);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton("Add to Cart", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);
    
    // Add all widgets to main layout
    layout->addWidget(nameLabel);
    layout->addWidget(priceLabel);
    layout->addLayout(quantityLayout);
    layout->addLayout(buttonLayout);
    
    // Connect signals
    connect(addButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        onAddToCartClicked(product.id, quantitySpinBox->value());
    }
}

void ProductBrowsePage::onAddToCartClicked(int productId, int quantity)
{
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        QMessageBox::warning(this, "Error", "User ID not found");
        return;
    }

    if (dbManager.addToCart(userId, productId, quantity)) {
        QMessageBox::information(this, "Success", "Item added to cart successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to add item to cart");
    }
}

void ProductBrowsePage::showReviewsDialog(const Product& product)
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString("Reviews for %1").arg(product.name));
    dialog.setModal(true);
    dialog.resize(500, 400);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Product info section
    QHBoxLayout* productInfoLayout = new QHBoxLayout();
    
    // Product image
    QLabel* productImage = new QLabel(&dialog);
    QPixmap pixmap;
    if (!product.imageData.isEmpty()) {
        pixmap.loadFromData(product.imageData);
    } else {
        pixmap = QPixmap(":/images/no-image.png");
    }
    pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    productImage->setPixmap(pixmap);
    productInfoLayout->addWidget(productImage);
    
    // Product details
    QVBoxLayout* productDetailsLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel(product.name, &dialog);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    productDetailsLayout->addWidget(nameLabel);
    
    double avgRating = dbManager.getProductAverageRating(product.id);
    QList<Review> reviews = dbManager.getProductReviews(product.id);
    
    QString ratingText = avgRating > 0 
        ? QString("★ %1/5 (%2 reviews)").arg(avgRating, 0, 'f', 1).arg(reviews.size())
        : "No reviews yet";
    QLabel* ratingLabel = new QLabel(ratingText, &dialog);
    ratingLabel->setStyleSheet("color: #666;");
    productDetailsLayout->addWidget(ratingLabel);
    
    productInfoLayout->addLayout(productDetailsLayout);
    productInfoLayout->addStretch();
    layout->addLayout(productInfoLayout);
    
    // Separator line
    QFrame* line = new QFrame(&dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);
    
    // Reviews section
    QScrollArea* scrollArea = new QScrollArea(&dialog);
    QWidget* scrollContent = new QWidget(scrollArea);
    QVBoxLayout* reviewsLayout = new QVBoxLayout(scrollContent);
    
    setupReviewsSection(reviewsLayout, reviews);
    
    reviewsLayout->addStretch();
    scrollContent->setLayout(reviewsLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);
    
    // Close button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    dialog.setLayout(layout);
    dialog.exec();
}

void ProductBrowsePage::handleProductClicked(const Product& product)
{
    emit productSelected(product);
}

void ProductBrowsePage::handleReviewAdded()
{
    // Refresh the products display to update ratings
    updateProducts();
}

void ProductBrowsePage::updateProducts()
{
    // Clear existing widgets
    clearProducts();
    
    // Fetch updated products
    products = dbManager.getAllProducts();
    filteredProducts = products;  // Initialize filtered products with all products
    
    if (!products.isEmpty()) {
        displayProducts();
    }
}

void ProductBrowsePage::setupReviewsSection(QVBoxLayout* layout, const QList<Review>& reviews) {
    for (const Review& review : reviews) {
        QFrame* reviewCard = new QFrame();
        reviewCard->setFrameStyle(QFrame::StyledPanel);
        reviewCard->setStyleSheet("QFrame { background-color: #f5f5f5; border-radius: 8px; margin: 4px; padding: 8px; }"
                                "QLabel { color: #333333; }");

        QVBoxLayout* reviewLayout = new QVBoxLayout(reviewCard);

        // Header with user and date
        QHBoxLayout* headerLayout = new QHBoxLayout();
        QLabel* userLabel = new QLabel(review.username);
        userLabel->setStyleSheet("font-weight: bold; color: #333333;");
        
        QLabel* dateLabel = new QLabel(review.reviewDate.toString("yyyy-MM-dd"));
        dateLabel->setStyleSheet("color: #666666;");
        
        headerLayout->addWidget(userLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(dateLabel);

        // Rating display
        QString stars;
        for (int i = 0; i < review.rating; i++) stars += "★";
        for (int i = review.rating; i < 5; i++) stars += "☆";
        QLabel* ratingLabel = new QLabel(stars);
        ratingLabel->setStyleSheet("color: #ffd700;"); // Gold color for stars

        // Review text
        QLabel* commentLabel = new QLabel(review.comment);
        commentLabel->setWordWrap(true);
        commentLabel->setStyleSheet("color: #333333; margin-top: 4px;");
        
        reviewLayout->addLayout(headerLayout);
        reviewLayout->addWidget(ratingLabel);
        reviewLayout->addWidget(commentLabel);

        layout->addWidget(reviewCard);
    }

    if (reviews.isEmpty()) {
        QLabel* noReviewsLabel = new QLabel("No reviews yet");
        noReviewsLabel->setStyleSheet("color: #666666; padding: 20px;");
        noReviewsLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(noReviewsLabel);
    }
} 