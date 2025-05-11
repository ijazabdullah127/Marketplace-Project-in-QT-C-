#include "admindashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QScrollArea>

AdminDashboard::AdminDashboard(QWidget *parent) 
    : QMainWindow(parent)
    , tabWidget(nullptr)
    , userTab(nullptr)
    , productTab(nullptr)
    , salesTab(nullptr)
    , userTable(nullptr)
    , productTable(nullptr)
    , suspendUserButton(nullptr)
    , resetPasswordButton(nullptr)
    , deleteProductButton(nullptr)
    , totalSalesLabel(nullptr)
    , totalOrdersLabel(nullptr)
    , averageOrderValueLabel(nullptr)
    , db(DatabaseManager::getInstance())
{
    setupUI();
    refreshUserList();
    refreshProductList();
    refreshSalesReport();
}

void AdminDashboard::setupUI() {
    setWindowTitle("Admin Dashboard");
    resize(800, 600);

    // Create main widget and layout
    QWidget* mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create navigation buttons at the top
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setAlignment(Qt::AlignRight);
    navLayout->setSpacing(10);

    // Create home button
    QPushButton* homeBtn = new QPushButton("🏠 Home", this);
    homeBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
    );

    // Create logout button
    QPushButton* logoutBtn = new QPushButton("🚪 Logout", this);
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
    );

    navLayout->addWidget(homeBtn);
    navLayout->addWidget(logoutBtn);
    mainLayout->addLayout(navLayout);

    // Add title
    QLabel* titleLabel = new QLabel("👑 Admin Dashboard", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2c3e50;"
        "margin: 10px 0;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Add tab widget with modern styling
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "    border: 1px solid #dcdde1;"
        "    border-radius: 8px;"
        "    background: white;"
        "    padding: 10px;"
        "}"
        "QTabBar::tab {"
        "    background: #f5f6fa;"
        "    color: #2c3e50;"
        "    padding: 12px 20px;"
        "    border: 1px solid #dcdde1;"
        "    border-bottom: none;"
        "    border-top-left-radius: 8px;"
        "    border-top-right-radius: 8px;"
        "    min-width: 150px;"
        "    font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "    background: white;"
        "    border-bottom: none;"
        "    margin-bottom: -1px;"
        "}"
        "QTabBar::tab:hover {"
        "    background: #ecf0f1;"
        "}"
    );
    mainLayout->addWidget(tabWidget);

    setupUserManagement();
    setupProductManagement();
    setupSalesReport();

    // Connect navigation buttons
    connect(homeBtn, &QPushButton::clicked, this, &AdminDashboard::onHomeClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &AdminDashboard::onLogoutClicked);
}

void AdminDashboard::setupUserManagement() {
    userTab = new QWidget();
    QVBoxLayout* userLayout = new QVBoxLayout(userTab);
    userLayout->setSpacing(15);

    // Style for the user table
    userTable = new QTableWidget();
    userTable->verticalHeader()->hide();
    userTable->setStyleSheet(QString(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #dcdde1;"
        "    border-radius: 8px;"
        "    gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f5f6fa;"
        "    color: #2c3e50;"
        "    padding: 10px;"
        "    border: none;"
        "    font-weight: bold;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
    ));

    userTable->setColumnCount(6);
    userTable->setHorizontalHeaderLabels({"Email", "Username", "Admin", "Suspended", "Reset", "Status"});
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable->setSelectionBehavior(QTableWidget::SelectRows);
    userTable->setSelectionMode(QTableWidget::SingleSelection);
    userLayout->addWidget(userTable);

    // Button container with modern styling
    QWidget* buttonContainer = new QWidget();
    buttonContainer->setStyleSheet(QString(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "}"
    ));

    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(10);

    QString buttonStyle = QString(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}"
    );

    suspendUserButton = new QPushButton("Suspend User");
    resetPasswordButton = new QPushButton("Reset Password");
    
    suspendUserButton->setStyleSheet(buttonStyle);
    resetPasswordButton->setStyleSheet(buttonStyle);

    buttonLayout->addWidget(suspendUserButton);
    buttonLayout->addWidget(resetPasswordButton);
    userLayout->addWidget(buttonContainer);

    connect(suspendUserButton, &QPushButton::clicked, this, &AdminDashboard::onSuspendUserClicked);
    connect(resetPasswordButton, &QPushButton::clicked, this, &AdminDashboard::onResetPasswordClicked);

    tabWidget->addTab(userTab, "👥 User Management");
}

void AdminDashboard::setupProductManagement() {
    productTab = new QWidget();
    QVBoxLayout* productLayout = new QVBoxLayout(productTab);
    productLayout->setSpacing(15);

    // Style for the product table
    productTable = new QTableWidget();
    productTable->verticalHeader()->hide();
    productTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #dcdde1;"
        "    border-radius: 8px;"
        "    gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f5f6fa;"
        "    color: #2c3e50;"
        "    padding: 10px;"
        "    border: none;"
        "    font-weight: bold;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
    );

    productTable->setColumnCount(6);
    productTable->setHorizontalHeaderLabels({"ID", "Name", "Price", "Seller", "Category", "Stock"});
    productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    productTable->setSelectionBehavior(QTableWidget::SelectRows);
    productTable->setSelectionMode(QTableWidget::SingleSelection);
    productLayout->addWidget(productTable);

    // Button container
    QWidget* buttonContainer = new QWidget();
    buttonContainer->setStyleSheet(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "}"
    );
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);

    deleteProductButton = new QPushButton("🗑️ Delete");
    deleteProductButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}"
    );
    buttonLayout->addWidget(deleteProductButton);
    productLayout->addWidget(buttonContainer);

    connect(deleteProductButton, &QPushButton::clicked, this, &AdminDashboard::onDeleteProductClicked);

    tabWidget->addTab(productTab, "📦 Product Management");
}

void AdminDashboard::setupSalesReport() {
    salesTab = new QWidget();
    QVBoxLayout* salesLayout = new QVBoxLayout(salesTab);
    salesLayout->setSpacing(20);
    salesLayout->setAlignment(Qt::AlignTop);

    // Create a scroll area for the sales metrics
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(QString(
        "QScrollArea {"
        "    border: none;"
        "    background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #f0f0f0;"
        "    width: 10px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #c0c0c0;"
        "    min-height: 30px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: none;"
        "}"
    ));

    // Create a container widget for the scroll area
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(15);
    scrollLayout->setContentsMargins(10, 10, 10, 10);

    // Create a container for the sales metrics
    QWidget* metricsContainer = new QWidget();
    metricsContainer->setStyleSheet(QString(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 12px;"
        "    padding: 20px;"
        "}"
    ));

    QVBoxLayout* metricsLayout = new QVBoxLayout(metricsContainer);
    metricsLayout->setSpacing(15);

    // Style for metric labels
    QString metricStyle = QString(
        "QLabel {"
        "    font-size: 18px;"
        "    color: #2c3e50;"
        "    padding: 15px;"
        "    background-color: #f8f9fa;"
        "    border-radius: 8px;"
        "    border: 1px solid #dcdde1;"
        "}"
        "QLabel[class='value'] {"
        "    font-weight: bold;"
        "    color: #3498db;"
        "    font-size: 24px;"
        "}"
    );

    // Total Sales
    QLabel* totalSalesTitle = new QLabel("💰 Total Sales");
    totalSalesTitle->setStyleSheet(metricStyle);
    totalSalesLabel = new QLabel("$0.00");
    totalSalesLabel->setProperty("class", "value");
    totalSalesLabel->setStyleSheet(metricStyle);
    metricsLayout->addWidget(totalSalesTitle);
    metricsLayout->addWidget(totalSalesLabel);

    // Total Orders
    QLabel* totalOrdersTitle = new QLabel("📦 Total Orders");
    totalOrdersTitle->setStyleSheet(metricStyle);
    totalOrdersLabel = new QLabel("0");
    totalOrdersLabel->setProperty("class", "value");
    totalOrdersLabel->setStyleSheet(metricStyle);
    metricsLayout->addWidget(totalOrdersTitle);
    metricsLayout->addWidget(totalOrdersLabel);

    // Average Order Value
    QLabel* avgOrderTitle = new QLabel("📊 Average Order Value");
    avgOrderTitle->setStyleSheet(metricStyle);
    averageOrderValueLabel = new QLabel("$0.00");
    averageOrderValueLabel->setProperty("class", "value");
    averageOrderValueLabel->setStyleSheet(metricStyle);
    metricsLayout->addWidget(avgOrderTitle);
    metricsLayout->addWidget(averageOrderValueLabel);

    scrollLayout->addWidget(metricsContainer);
    scrollArea->setWidget(scrollContent);
    salesLayout->addWidget(scrollArea);
    tabWidget->addTab(salesTab, "💵 Sales Report");
}

void AdminDashboard::refreshUserList() {
    QList<User> users = db.getAllUsers();
    userTable->setRowCount(users.size());
    
    qDebug() << "Refreshing user list, found" << users.size() << "users";
    
    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        
        // Email
        QTableWidgetItem* emailItem = new QTableWidgetItem(user.getEmail());
        emailItem->setFlags(emailItem->flags() & ~Qt::ItemIsEditable);
        userTable->setItem(i, 0, emailItem);
        
        // Username
        QTableWidgetItem* usernameItem = new QTableWidgetItem(user.getUsername());
        usernameItem->setFlags(usernameItem->flags() & ~Qt::ItemIsEditable);
        userTable->setItem(i, 1, usernameItem);
        
        // Admin Status
        QTableWidgetItem* adminItem = new QTableWidgetItem(user.isAdmin() ? "Yes" : "No");
        adminItem->setFlags(adminItem->flags() & ~Qt::ItemIsEditable);
        userTable->setItem(i, 2, adminItem);
        
        // Suspension Status
        QTableWidgetItem* suspendedItem = new QTableWidgetItem(user.isSuspended() ? "Yes" : "No");
        suspendedItem->setFlags(suspendedItem->flags() & ~Qt::ItemIsEditable);
        userTable->setItem(i, 3, suspendedItem);
        
        // Reset Password Button
        QPushButton* resetBtn = new QPushButton("Reset", this);
        resetBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 4px 8px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #2980b9;"
            "}"
        );
        connect(resetBtn, &QPushButton::clicked, [this, user]() {
            handleResetPassword(user);
        });
        userTable->setCellWidget(i, 4, resetBtn);
        
        // Suspend/Unsuspend Button
        QPushButton* suspendBtn = new QPushButton(user.isSuspended() ? "Enable" : "Disable", this);
        suspendBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: " + QString(user.isSuspended() ? "#2ecc71" : "#e74c3c") + ";"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 4px 8px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: " + QString(user.isSuspended() ? "#27ae60" : "#c0392b") + ";"
            "}"
        );
        connect(suspendBtn, &QPushButton::clicked, [this, user, suspendBtn]() {
            handleSuspendUser(user, suspendBtn);
        });
        userTable->setCellWidget(i, 5, suspendBtn);
    }
}

void AdminDashboard::refreshProductList() {
    QList<Product> products = db.getAllProducts();
    productTable->setRowCount(products.size());

    for (int i = 0; i < products.size(); ++i) {
        const Product& product = products[i];
        productTable->setItem(i, 0, new QTableWidgetItem(QString::number(product.id)));
        productTable->setItem(i, 1, new QTableWidgetItem(product.name));
        productTable->setItem(i, 2, new QTableWidgetItem(QString::number(product.price, 'f', 2)));
        productTable->setItem(i, 3, new QTableWidgetItem(QString::number(product.sellerId)));
        productTable->setItem(i, 4, new QTableWidgetItem(product.category));
        productTable->setItem(i, 5, new QTableWidgetItem(QString::number(product.stock)));
    }
}

void AdminDashboard::refreshSalesReport() {
    double totalSales = db.getTotalSales();
    int totalOrders = db.getTotalOrders();
    double avgOrderValue = db.getAverageOrderValue();

    totalSalesLabel->setText(QString("Total Sales: $%1").arg(totalSales, 0, 'f', 2));
    totalOrdersLabel->setText(QString("Total Orders: %1").arg(totalOrders));
    averageOrderValueLabel->setText(QString("Average Order Value: $%1").arg(avgOrderValue, 0, 'f', 2));
}

void AdminDashboard::onSuspendUserClicked() {
    QList<QTableWidgetItem*> selectedItems = userTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a user to suspend");
        return;
    }

    int row = selectedItems.first()->row();
    int userId = userTable->item(row, 0)->text().toInt();
    
    if (db.suspendUser(userId)) {
        QMessageBox::information(this, "Success", "User suspended successfully");
        refreshUserList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to suspend user");
    }
}

void AdminDashboard::onDeleteProductClicked() {
    QList<QTableWidgetItem*> selectedItems = productTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a product to delete");
        return;
    }

    int row = selectedItems.first()->row();
    int productId = productTable->item(row, 0)->text().toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this product?",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        if (db.deleteProduct(productId)) {
            QMessageBox::information(this, "Success", "Product deleted successfully");
            refreshProductList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product");
        }
    }
}

void AdminDashboard::onResetPasswordClicked() {
    QList<QTableWidgetItem*> selectedItems = userTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a user to reset password");
        return;
    }

    int row = selectedItems.first()->row();
    int userId = userTable->item(row, 0)->text().toInt();
    
    bool ok;
    QString newPassword = QInputDialog::getText(this, "Reset Password",
                                              "Enter new password:", QLineEdit::Password,
                                              "", &ok);
    
    if (ok && !newPassword.isEmpty()) {
        // Hash the password before storing
        QByteArray hashedPassword = QCryptographicHash::hash(
            newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
            
        if (db.resetUserPassword(userId, QString(hashedPassword))) {
            QMessageBox::information(this, "Success", "Password reset successfully");
        } else {
            QMessageBox::warning(this, "Error", "Failed to reset password");
        }
    }
}

void AdminDashboard::handleSuspendUser(const User& user, QPushButton* button) {
    int userId = db.getUserIdByEmail(user.getEmail());
    if (userId == -1) {
        QMessageBox::warning(this, "Error", "Failed to find user ID");
        return;
    }
    
    bool currentlySuspended = user.isSuspended();
    bool success;
    
    if (currentlySuspended) {
        success = db.unsuspendUser(userId);
        if (success) {
            button->setText("Suspend");
            QMessageBox::information(this, "Success", "User has been unsuspended");
        }
    } else {
        success = db.suspendUser(userId);
        if (success) {
            button->setText("Unsuspend");
            QMessageBox::information(this, "Success", "User has been suspended");
        }
    }
    
    if (!success) {
        QMessageBox::warning(this, "Error", 
            currentlySuspended ? "Failed to unsuspend user" : "Failed to suspend user");
    }
    
    // Refresh the user list to show updated status
    refreshUserList();
}

void AdminDashboard::handleResetPassword(const User& user) {
    bool ok;
    QString newPassword = QInputDialog::getText(this, "Reset Password",
                                              "Enter new password:", QLineEdit::Password,
                                              "", &ok);
    
    if (ok && !newPassword.isEmpty()) {
        // Hash the password before storing
        QByteArray hashedPassword = QCryptographicHash::hash(
            newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
            
        if (db.resetUserPassword(db.getUserIdByEmail(user.getEmail()), QString(hashedPassword))) {
            QMessageBox::information(this, "Success", "Password reset successfully");
        } else {
            QMessageBox::warning(this, "Error", "Failed to reset password");
        }
    }
}

void AdminDashboard::onHomeClicked() {
    emit navigateHome();
}

void AdminDashboard::onLogoutClicked() {
    emit logout();
} 