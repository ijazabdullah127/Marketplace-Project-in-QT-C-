#include "sellerdashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QComboBox>

SellerDashboard::SellerDashboard(QWidget *parent)
    : QMainWindow(parent)
    , db(DatabaseManager::getInstance())
{
    setupUI();
    refreshOrderList();
    refreshProductList();
}

void SellerDashboard::setupUI() {
    setWindowTitle("Seller Dashboard");
    resize(800, 600);

    QWidget* mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Navigation buttons
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setAlignment(Qt::AlignRight);
    navLayout->setSpacing(10);

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

    // Title
    QLabel* titleLabel = new QLabel("🏪 Seller Dashboard", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2c3e50;"
        "margin: 10px 0;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Metrics section
    setupMetrics();

    // Tab widget
    QTabWidget* tabWidget = new QTabWidget(this);
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

    // Create and add tabs
    QWidget* ordersTab = new QWidget();
    QWidget* productsTab = new QWidget();
    
    setupOrderManagement();
    setupProductManagement();

    tabWidget->addTab(ordersTab, "📦 Orders");
    tabWidget->addTab(productsTab, "🛍️ Products");
    
    mainLayout->addWidget(tabWidget);

    // Connect signals
    connect(homeBtn, &QPushButton::clicked, this, &SellerDashboard::onHomeClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &SellerDashboard::onLogoutClicked);
}

void SellerDashboard::setupMetrics() {
    QWidget* metricsContainer = new QWidget(this);
    metricsContainer->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 12px;"
        "    padding: 20px;"
        "}"
    );

    QHBoxLayout* metricsLayout = new QHBoxLayout(metricsContainer);
    metricsLayout->setSpacing(20);

    QString metricStyle =
        "QLabel {"
        "    font-size: 18px;"
        "    color: #2c3e50;"
        "    padding: 15px;"
        "    background-color: #f8f9fa;"
        "    border-radius: 8px;"
        "    border: 1px solid #dcdde1;"
        "}";

    totalSalesLabel = new QLabel("Total Sales: $0.00");
    totalOrdersLabel = new QLabel("Total Orders: 0");

    totalSalesLabel->setStyleSheet(metricStyle);
    totalOrdersLabel->setStyleSheet(metricStyle);

    metricsLayout->addWidget(totalSalesLabel);
    metricsLayout->addWidget(totalOrdersLabel);

    mainLayout->addWidget(metricsContainer);
}

void SellerDashboard::setupOrderManagement() {
    QVBoxLayout* orderLayout = new QVBoxLayout(ordersTab);
    orderLayout->setSpacing(15);

    // Order table
    orderTable = new QTableWidget(this);
    orderTable->setStyleSheet(
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
    );

    orderTable->setColumnCount(6);
    orderTable->setHorizontalHeaderLabels({"Order ID", "Date", "Customer", "Items", "Total", "Status"});
    orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    orderTable->setSelectionBehavior(QTableWidget::SelectRows);
    orderTable->setSelectionMode(QTableWidget::SingleSelection);
    orderTable->verticalHeader()->hide();

    orderLayout->addWidget(orderTable);
}

void SellerDashboard::setupProductManagement() {
    QVBoxLayout* productLayout = new QVBoxLayout(productsTab);
    productLayout->setSpacing(15);

    // Product table
    productTable = new QTableWidget(this);
    productTable->setStyleSheet(orderTable->styleSheet());

    productTable->setColumnCount(6);
    productTable->setHorizontalHeaderLabels({"ID", "Name", "Price", "Category", "Stock", "Actions"});
    productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    productTable->setSelectionBehavior(QTableWidget::SelectRows);
    productTable->setSelectionMode(QTableWidget::SingleSelection);
    productTable->verticalHeader()->hide();

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
    buttonLayout->setSpacing(10);

    // Add Product button
    addProductButton = new QPushButton("➕ Add Product");
    addProductButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-weight: bold;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
    );

    // Edit Product button
    editProductButton = new QPushButton("✏️ Edit");
    editProductButton->setStyleSheet(
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
    );

    // Delete Product button
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
    );

    buttonLayout->addWidget(addProductButton);
    buttonLayout->addWidget(editProductButton);
    buttonLayout->addWidget(deleteProductButton);
    productLayout->addWidget(buttonContainer);

    // Connect signals
    connect(addProductButton, &QPushButton::clicked, this, &SellerDashboard::onAddProductClicked);
    connect(editProductButton, &QPushButton::clicked, this, &SellerDashboard::onEditProductClicked);
    connect(deleteProductButton, &QPushButton::clicked, this, &SellerDashboard::onDeleteProductClicked);
}

void SellerDashboard::refreshOrderList() {
    // Get orders for the current seller
    QList<Order> orders = db.getSellerOrders(currentUserEmail);
    orderTable->setRowCount(orders.size());

    for (int i = 0; i < orders.size(); ++i) {
        const Order& order = orders[i];
        
        orderTable->setItem(i, 0, new QTableWidgetItem(QString::number(order.id)));
        orderTable->setItem(i, 1, new QTableWidgetItem(order.orderDate.toString("yyyy-MM-dd hh:mm")));
        orderTable->setItem(i, 2, new QTableWidgetItem(order.customerEmail));
        orderTable->setItem(i, 3, new QTableWidgetItem(QString::number(order.itemCount)));
        orderTable->setItem(i, 4, new QTableWidgetItem(QString("$%1").arg(order.totalAmount, 0, 'f', 2)));

        // Status combobox
        QComboBox* statusCombo = new QComboBox();
        statusCombo->addItems({"Pending", "Processing", "Shipped", "Delivered", "Cancelled"});
        statusCombo->setCurrentText(order.status);
        statusCombo->setStyleSheet(
            "QComboBox {"
            "    background-color: white;"
            "    border: 1px solid #bdc3c7;"
            "    border-radius: 4px;"
            "    padding: 4px;"
            "}"
        );
        
        connect(statusCombo, &QComboBox::currentTextChanged, 
            [this, orderId = order.id](const QString& newStatus) {
                handleOrderStatusUpdate(orderId, newStatus);
            }
        );
        
        orderTable->setCellWidget(i, 5, statusCombo);
    }
}

void SellerDashboard::refreshProductList() {
    QList<Product> products = db.getSellerProducts(currentUserEmail);
    productTable->setRowCount(products.size());

    for (int i = 0; i < products.size(); ++i) {
        const Product& product = products[i];
        
        productTable->setItem(i, 0, new QTableWidgetItem(QString::number(product.id)));
        productTable->setItem(i, 1, new QTableWidgetItem(product.name));
        productTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(product.price, 0, 'f', 2)));
        productTable->setItem(i, 3, new QTableWidgetItem(product.category));
        productTable->setItem(i, 4, new QTableWidgetItem(QString::number(product.stock)));

        // Action buttons
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setSpacing(5);
        actionLayout->setMargin(0);

        QPushButton* editBtn = new QPushButton("Edit");
        QPushButton* deleteBtn = new QPushButton("Delete");

        QString buttonStyle =
            "QPushButton {"
            "    padding: 4px 8px;"
            "    border-radius: 4px;"
            "    color: white;"
            "    font-weight: bold;"
            "}";

        editBtn->setStyleSheet(buttonStyle + "background-color: #3498db;");
        deleteBtn->setStyleSheet(buttonStyle + "background-color: #e74c3c;");

        connect(editBtn, &QPushButton::clicked, [this, product]() {
            handleEditProduct(product);
        });
        connect(deleteBtn, &QPushButton::clicked, [this, product]() {
            handleDeleteProduct(product);
        });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(deleteBtn);
        productTable->setCellWidget(i, 5, actionWidget);
    }
}

void SellerDashboard::handleOrderStatusUpdate(int orderId, const QString& newStatus) {
    if (db.updateOrderStatus(orderId, newStatus)) {
        QMessageBox::information(this, "Success", "Order status updated successfully");
        refreshOrderList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update order status");
    }
}

void SellerDashboard::onHomeClicked() {
    emit navigateHome();
}

void SellerDashboard::onLogoutClicked() {
    emit logout();
}

void SellerDashboard::onAddProductClicked() {
    // TODO: Implement add product functionality
}

void SellerDashboard::onEditProductClicked() {
    // TODO: Implement edit product functionality
}

void SellerDashboard::onDeleteProductClicked() {
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