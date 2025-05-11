#include "cartpage.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QFormLayout>

CartPage::CartPage(QWidget *parent)
    : ProtectedPage(parent)
    , cartTable(nullptr)
    , checkoutButton(nullptr)
    , removeButton(nullptr)
    , totalLabel(nullptr)
    , mainLayout(nullptr)
    , total(0.0)
    , dbManager(DatabaseManager::getInstance())
    , authManager(AuthManager::getInstance())
{
    qDebug() << "CartPage constructor called";
    setupUI();
    // Don't load cart in constructor, wait for showEvent
}

void CartPage::showEvent(QShowEvent* event)
{
    qDebug() << "CartPage showEvent called";
    QWidget::showEvent(event);
    loadCart(); // Reload cart data when page becomes visible
}

void CartPage::setupUI()
{
    qDebug() << "Setting up CartPage UI";
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
    
    // Connect navigation buttons
    connect(homeBtn, &QPushButton::clicked, this, [this]() { emit navigateHome(); });
    connect(logoutBtn, &QPushButton::clicked, this, [this]() { emit logout(); });
    
    navLayout->addWidget(homeBtn);
    navLayout->addWidget(logoutBtn);
    mainLayout->addLayout(navLayout);
    
    // Header section
    QWidget* headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 15px;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 20px;"
        "}"
    );
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    
    QLabel* titleLabel = new QLabel("🛒 Shopping Cart", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2c3e50;"
    );
    headerLayout->addWidget(titleLabel);
    
    totalLabel = new QLabel("Total: $0.00", this);
    totalLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "color: #27ae60;"
    );
    headerLayout->addStretch();
    headerLayout->addWidget(totalLabel);
    
    mainLayout->addWidget(headerWidget);
    
    // Cart table with modern styling
    cartTable = new QTableWidget(this);
    cartTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 15px;"
        "    gridline-color: #f0f0f0;"
        "    padding: 10px;"
        "}"
        "QTableWidget::item {"
        "    padding: 12px;"
        "    border-bottom: 1px solid #f0f0f0;"
        "    color: #2c3e50;"
        "    font-size: 14px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #f5f6fa;"
        "    color: #2c3e50;"
        "}"
        "QTableWidget::item:hover:!selected {"
        "    background-color: #f8f9fa;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f8f9fa;"
        "    color: #2c3e50;"
        "    padding: 15px;"
        "    border: none;"
        "    border-bottom: 2px solid #e0e0e0;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QTableWidget:focus {"
        "    outline: none;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
    
    cartTable->setColumnCount(4);
    cartTable->setHorizontalHeaderLabels({"Product", "Quantity", "Price", "Subtotal"});
    cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cartTable->setSelectionBehavior(QTableWidget::SelectRows);
    cartTable->setSelectionMode(QTableWidget::SingleSelection);
    cartTable->setShowGrid(false);
    cartTable->verticalHeader()->setVisible(false);
    cartTable->setAlternatingRowColors(true);
    cartTable->setFocusPolicy(Qt::NoFocus); // Disable focus rectangle
    
    mainLayout->addWidget(cartTable);
    
    // Buttons container
    QWidget* buttonContainer = new QWidget(this);
    buttonContainer->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 15px;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 20px;"
        "}"
    );
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(15);
    
    removeButton = new QPushButton("🗑️ Remove Selected", this);
    removeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px 20px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}"
    );
    connect(removeButton, &QPushButton::clicked, this, &CartPage::removeSelectedItem);
    
    checkoutButton = new QPushButton("💳 Checkout", this);
    checkoutButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px 20px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}"
    );
    connect(checkoutButton, &QPushButton::clicked, this, &CartPage::checkout);
    
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(checkoutButton);
    
    mainLayout->addWidget(buttonContainer);
    
    setLayout(mainLayout);
}

void CartPage::loadCart()
{
    qDebug() << "loadCart() called";
    // Clear existing rows
    cartTable->setRowCount(0);
    total = 0.0;
    
    int userId = authManager.getCurrentUserId();
    qDebug() << "Current user ID:" << userId;
    
    if (userId == -1) {
        qDebug() << "User not authenticated, redirecting to login...";
        emit loginRequired();
        return;
    }
    
    // Get cart items from database
    qDebug() << "Fetching cart items from database...";
    QList<CartItem> cartItems = dbManager.getCartItems(userId);
    qDebug() << "Found" << cartItems.size() << "items in cart";
    
    if (cartItems.isEmpty()) {
        qDebug() << "Cart is empty";
        QMessageBox::information(this, "Shopping Cart", "Your cart is empty. Add some products to your cart!");
        return;
    }
    
    qDebug() << "Processing cart items...";
    for (const CartItem& item : cartItems) {
        qDebug() << "Processing cart item - ID:" << item.id << "Product ID:" << item.productId << "Quantity:" << item.quantity;
        
        // Get product details
        Product product = dbManager.getProductById(item.productId);
        if (product.id == -1) {
            qDebug() << "ERROR: Product not found for cart item:" << item.productId;
            continue;
        }
        
        qDebug() << "Found product:" << product.name << "Price:" << product.price;
        
        int row = cartTable->rowCount();
        cartTable->insertRow(row);
        
        // Product name
        QTableWidgetItem* nameItem = new QTableWidgetItem(product.name);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        cartTable->setItem(row, 0, nameItem);
        
        // Quantity
        QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(item.quantity));
        quantityItem->setFlags(quantityItem->flags() & ~Qt::ItemIsEditable);
        cartTable->setItem(row, 1, quantityItem);
        
        // Price
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString("$%1").arg(item.price, 0, 'f', 2));
        priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
        cartTable->setItem(row, 2, priceItem);
        
        // Subtotal
        double subtotal = item.price * item.quantity;
        QTableWidgetItem* subtotalItem = new QTableWidgetItem(QString("$%1").arg(subtotal, 0, 'f', 2));
        subtotalItem->setFlags(subtotalItem->flags() & ~Qt::ItemIsEditable);
        cartTable->setItem(row, 3, subtotalItem);
        
        // Store cart item ID in the first column's data role
        nameItem->setData(Qt::UserRole, item.id);
        
        total += subtotal;
        qDebug() << "Added row" << row << "to cart table - Product:" << product.name << "Subtotal:" << subtotal;
    }
    
    qDebug() << "Cart loading complete. Total:" << total;
    updateTotal();
}

void CartPage::updateCart()
{
    qDebug() << "updateCart() called";
    if (checkAccess()) {
        loadCart();
    } else {
        qDebug() << "Access check failed in updateCart";
    }
}

void CartPage::checkout()
{
    if (!checkAccess()) {
        qDebug() << "Access check failed in checkout";
        return;
    }
    
    if (cartTable->rowCount() == 0) {
        QMessageBox::warning(this, "Checkout Failed", "Your cart is empty!");
        return;
    }
    
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        QMessageBox::warning(this, "Checkout Failed", "Please log in to complete your purchase.");
        return;
    }
    
    // Get cart items
    QList<CartItem> cartItems = dbManager.getCartItems(userId);
    if (cartItems.isEmpty()) {
        QMessageBox::warning(this, "Checkout Failed", "Your cart is empty!");
        return;
    }
    
    // Create payment dialog with modern styling
    QDialog paymentDialog(this);
    paymentDialog.setWindowTitle("Checkout");
    paymentDialog.setModal(true);
    paymentDialog.setMinimumWidth(400);
    paymentDialog.setStyleSheet(
        "QDialog {"
        "    background-color: white;"
        "}"
        "QLabel {"
        "    color: #2c3e50;"
        "    font-size: 14px;"
        "}"
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
        "QPushButton {"
        "    padding: 10px 20px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 100px;"
        "}"
        "QPushButton[text=\"Pay Now\"] {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "}"
        "QPushButton[text=\"Pay Now\"]:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton[text=\"Cancel\"] {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "}"
        "QPushButton[text=\"Cancel\"]:hover {"
        "    background-color: #c0392b;"
        "}"
    );
    
    QVBoxLayout* layout = new QVBoxLayout(&paymentDialog);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // Order summary section
    QWidget* summaryWidget = new QWidget(&paymentDialog);
    summaryWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    border-radius: 12px;"
        "    padding: 20px;"
        "}"
        "QLabel {"
        "    color: #2c3e50;"
        "}"
    );
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryWidget);
    
    QLabel* summaryTitle = new QLabel("Order Summary", &paymentDialog);
    summaryTitle->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px;");
    summaryLayout->addWidget(summaryTitle);
    
    QLabel* itemCountLabel = new QLabel(QString("Items: %1").arg(cartItems.size()), &paymentDialog);
    summaryLayout->addWidget(itemCountLabel);
    
    QLabel* totalLabel = new QLabel(QString("Total Amount: $%1").arg(total, 0, 'f', 2), &paymentDialog);
    totalLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #27ae60;");
    summaryLayout->addWidget(totalLabel);
    
    layout->addWidget(summaryWidget);
    
    // Payment details section
    QWidget* paymentWidget = new QWidget(&paymentDialog);
    paymentWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    border-radius: 12px;"
        "    padding: 20px;"
        "}"
    );
    QFormLayout* paymentLayout = new QFormLayout(paymentWidget);
    paymentLayout->setSpacing(15);
    
    QLineEdit* cardInput = new QLineEdit(&paymentDialog);
    cardInput->setPlaceholderText("1234-5678-9012-3456");
    cardInput->setMaxLength(19);
    
    QLineEdit* expiryInput = new QLineEdit(&paymentDialog);
    expiryInput->setPlaceholderText("MM/YY");
    expiryInput->setMaxLength(5);
    
    QLineEdit* cvvInput = new QLineEdit(&paymentDialog);
    cvvInput->setPlaceholderText("123");
    cvvInput->setMaxLength(3);
    cvvInput->setEchoMode(QLineEdit::Password);
    
    paymentLayout->addRow("Card Number:", cardInput);
    paymentLayout->addRow("Expiry Date:", expiryInput);
    paymentLayout->addRow("CVV:", cvvInput);
    
    layout->addWidget(paymentWidget);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    QPushButton* cancelButton = new QPushButton("Cancel", &paymentDialog);
    QPushButton* payButton = new QPushButton("Pay Now", &paymentDialog);
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(payButton);
    
    layout->addLayout(buttonLayout);
    
    connect(cancelButton, &QPushButton::clicked, &paymentDialog, &QDialog::reject);
    connect(payButton, &QPushButton::clicked, &paymentDialog, &QDialog::accept);
    
    if (paymentDialog.exec() == QDialog::Accepted) {
        // Create order
        int orderId = dbManager.createOrder(userId, cartItems);
        if (orderId != -1) {
            // Clear cart after successful order
            dbManager.clearCart(userId);
            
            QString message = QString("Order placed successfully!\n\n"
                                   "Order ID: %1\n"
                                   "Total amount: $%2\n\n"
                                   "You can track your order in the Order History page.")
                                .arg(orderId)
                                .arg(total, 0, 'f', 2);
            
            QMessageBox::information(this, "Order Confirmation", message);
            
            // Clear cart display
            cartTable->setRowCount(0);
            total = 0.0;
            updateTotal();
            
            // Emit signal to update order history
            emit orderPlaced();
        } else {
            QMessageBox::critical(this, "Error", "Failed to create order. Please try again.");
        }
    }
}

void CartPage::removeSelectedItem()
{
    if (!checkAccess()) {
        qDebug() << "Access check failed in removeSelectedItem";
        return;
    }
    
    QList<QTableWidgetItem*> selectedItems = cartTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Remove Item", "Please select an item to remove");
        return;
    }
    
    int row = selectedItems.first()->row();
    QTableWidgetItem* nameItem = cartTable->item(row, 0);
    int cartItemId = nameItem->data(Qt::UserRole).toInt();
    
    if (dbManager.removeFromCart(cartItemId)) {
        double subtotal = cartTable->item(row, 3)->text().remove('$').toDouble();
        total -= subtotal;
        cartTable->removeRow(row);
        updateTotal();
        QMessageBox::information(this, "Success", "Item removed from cart");
    } else {
        QMessageBox::warning(this, "Error", "Failed to remove item from cart");
    }
}

void CartPage::updateTotal()
{
    totalLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
} 