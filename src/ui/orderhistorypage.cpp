#include "orderhistorypage.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QColor>
#include <QStyleOptionViewItem>

OrderHistoryPage::OrderHistoryPage(QWidget *parent)
    : ProtectedPage(parent)
    , ordersTable(nullptr)
    , statusFilter(nullptr)
    , startDateFilter(nullptr)
    , endDateFilter(nullptr)
    , filterButton(nullptr)
    , resetButton(nullptr)
    , mainLayout(nullptr)
    , dbManager(DatabaseManager::getInstance())
    , authManager(AuthManager::getInstance())
{
    qDebug() << "OrderHistoryPage constructor called";
    setupUI();
}

void OrderHistoryPage::showEvent(QShowEvent* event)
{
    qDebug() << "OrderHistoryPage showEvent called";
    QWidget::showEvent(event);
    loadOrders();
}

void OrderHistoryPage::setupUI()
{
    qDebug() << "Setting up OrderHistoryPage UI";
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
    
    // Header section with title and stats
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    // Title with icon
    QLabel* titleLabel = new QLabel("📋 Order History", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2c3e50;"
        "margin-bottom: 10px;"
    );
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);
    
    // Filter section with modern styling
    QWidget* filterWidget = new QWidget(this);
    filterWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 15px;"
        "    border: 1px solid #e0e0e0;"
        "    padding: 15px;"
        "}"
        "QLabel {"
        "    color: #2c3e50;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "    margin-right: 8px;"
        "}"
        "QComboBox {"
        "    padding: 6px 12px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    background-color: white;"
        "    min-width: 150px;"
        "    max-width: 150px;"
        "    color: #2c3e50;"
        "    font-size: 12px;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    color: #2c3e50;"
        "    selection-background-color: #f0f0f0;"
        "    selection-color: #2c3e50;"
        "    padding: 4px;"
        "}"
        "QComboBox:hover {"
        "    border-color: #3498db;"
        "}"
        "QDateTimeEdit {"
        "    padding: 6px 8px;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    background-color: white;"
        "    min-width: 110px;"
        "    max-width: 110px;"
        "    color: #2c3e50;"
        "    font-size: 11px;"
        "}"
        "QDateTimeEdit:hover {"
        "    border-color: #3498db;"
        "}"
        "QCalendarWidget {"
        "    background-color: white;"
        "    color: #2c3e50;"
        "}"
        "QCalendarWidget QToolButton {"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "    padding: 6px;"
        "    border: none;"
        "}"
        "QCalendarWidget QMenu {"
        "    background-color: white;"
        "    color: #2c3e50;"
        "    padding: 4px;"
        "}"
        "QCalendarWidget QSpinBox {"
        "    color: #2c3e50;"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QCalendarWidget QTableView {"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QCalendarWidget QAbstractItemView:enabled {"
        "    color: #2c3e50;"
        "    background-color: white;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QCalendarWidget QWidget#qt_calendar_navigationbar {"
        "    background-color: #f8f9fa;"
        "    padding: 4px;"
        "}"
        "QTimeEdit::drop-down {"
        "    border: none;"
        "    background-color: transparent;"
        "    padding-right: 4px;"
        "}"
        "QTimeEdit::down-arrow {"
        "    image: none;"
        "    border: none;"
        "}"
    );
    
    // Create a vertical layout for the filter widget
    QVBoxLayout* filterMainLayout = new QVBoxLayout(filterWidget);
    filterMainLayout->setSpacing(15);
    filterMainLayout->setContentsMargins(20, 15, 20, 15);
    
    // Create horizontal layout for filters
    QHBoxLayout* filtersLayout = new QHBoxLayout();
    filtersLayout->setSpacing(15);
    
    // Create a container for status filter
    QWidget* statusContainer = new QWidget(this);
    QHBoxLayout* statusLayout = new QHBoxLayout(statusContainer);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(8);
    
    QLabel* statusLabel = new QLabel("Status:", this);
    statusLayout->addWidget(statusLabel);
    
    statusFilter = new QComboBox(this);
    statusFilter->addItem("All Orders", "");
    statusFilter->addItem("Pending", "Pending");
    statusFilter->addItem("Processing", "Processing");
    statusFilter->addItem("Shipped", "Shipped");
    statusFilter->addItem("Delivered", "Delivered");
    statusFilter->addItem("Cancelled", "Cancelled");
    statusLayout->addWidget(statusFilter);
    
    filtersLayout->addWidget(statusContainer);
    
    // Create a container for date range filter
    QWidget* dateContainer = new QWidget(this);
    QHBoxLayout* dateLayout = new QHBoxLayout(dateContainer);
    dateLayout->setContentsMargins(0, 0, 0, 0);
    dateLayout->setSpacing(4);
    
    QLabel* dateRangeLabel = new QLabel("Date:", this);
    dateLayout->addWidget(dateRangeLabel);
    
    startDateFilter = new QDateTimeEdit(this);
    startDateFilter->setCalendarPopup(true);
    startDateFilter->setDateTime(QDateTime::currentDateTime().addMonths(-1));
    startDateFilter->setDisplayFormat("MM/dd/yyyy h:mm AP");
    dateLayout->addWidget(startDateFilter);
    
    QLabel* toLabel = new QLabel("to", this);
    toLabel->setStyleSheet(
        "margin: 0 2px;"
        "font-size: 11px;"
    );
    dateLayout->addWidget(toLabel);
    
    endDateFilter = new QDateTimeEdit(this);
    endDateFilter->setCalendarPopup(true);
    endDateFilter->setDateTime(QDateTime::currentDateTime());
    endDateFilter->setDisplayFormat("MM/dd/yyyy h:mm AP");
    dateLayout->addWidget(endDateFilter);
    
    filtersLayout->addWidget(dateContainer);
    filtersLayout->addStretch(); // Add stretch to push filters to the left
    
    // Add filters layout to main filter layout
    filterMainLayout->addLayout(filtersLayout);
    
    // Create a container for buttons with reduced size
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->setAlignment(Qt::AlignRight); // Align buttons to the right
    
    // Filter buttons with smaller size
    filterButton = new QPushButton("Apply", this);
    filterButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "    min-width: 70px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
    );
    connect(filterButton, &QPushButton::clicked, this, &OrderHistoryPage::filterOrders);
    buttonLayout->addWidget(filterButton);
    
    resetButton = new QPushButton("Reset", this);
    resetButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "    min-width: 70px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
    );
    connect(resetButton, &QPushButton::clicked, this, &OrderHistoryPage::resetFilters);
    buttonLayout->addWidget(resetButton);
    
    // Add button layout to main filter layout
    filterMainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(filterWidget);
    
    // Orders table with modern styling
    ordersTable = new QTableWidget(this);
    ordersTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 15px;"
        "    gridline-color: #f0f0f0;"
        "}"
        "QTableWidget::item {"
        "    padding: 10px 8px;"
        "    border-bottom: 1px solid #f0f0f0;"
        "    color: #2c3e50;"
        "    font-size: 12px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #3498db15;"
        "    color: #2c3e50;"
        "    border-radius: 8px;"
        "    margin: 2px 4px;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f8f9fa;"
        "    color: #2c3e50;"
        "    padding: 10px 8px;"
        "    border: none;"
        "    border-bottom: 2px solid #e0e0e0;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QHeaderView::section:hover {"
        "    background-color: #f0f0f0;"
        "}"
    );
    
    ordersTable->setColumnCount(5);
    ordersTable->setHorizontalHeaderLabels({
        "Order ID",
        "Date",
        "Status",
        "Items",
        "Total"
    });
    
    ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ordersTable->setSelectionBehavior(QTableWidget::SelectRows);
    ordersTable->setSelectionMode(QTableWidget::SingleSelection);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setShowGrid(false);
    ordersTable->setAlternatingRowColors(true);
    
    // Set custom delegates for better data display
    ordersTable->setItemDelegate(new OrderTableItemDelegate(this));
    
    mainLayout->addWidget(ordersTable);
    
    // Info label
    QLabel* infoLabel = new QLabel("💡 Double-click any order to view details", this);
    infoLabel->setStyleSheet(
        "color: #7f8c8d;"
        "font-size: 12px;"
        "margin-top: 5px;"
    );
    mainLayout->addWidget(infoLabel);
    
    connect(ordersTable, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem* item) {
        int orderId = ordersTable->item(item->row(), 0)->text().toInt();
        viewOrderDetails(orderId);
    });
    
    setLayout(mainLayout);
}

void OrderHistoryPage::loadOrders()
{
    qDebug() << "loadOrders() called";
    ordersTable->setRowCount(0);
    
    int userId = authManager.getCurrentUserId();
    qDebug() << "Current user ID:" << userId;
    
    if (userId == -1) {
        qDebug() << "User not authenticated, redirecting to login...";
        emit loginRequired();
        return;
    }
    
    QList<Order> orders = dbManager.getUserOrders(userId);
    qDebug() << "Found" << orders.size() << "orders";
    
    for (const Order& order : orders) {
        int row = ordersTable->rowCount();
        ordersTable->insertRow(row);
        
        // Order ID
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(order.id));
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        ordersTable->setItem(row, 0, idItem);
        
        // Date
        QTableWidgetItem* dateItem = new QTableWidgetItem(
            order.orderDate.toString("yyyy-MM-dd hh:mm:ss"));
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        ordersTable->setItem(row, 1, dateItem);
        
        // Status
        QTableWidgetItem* statusItem = new QTableWidgetItem(order.status);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        ordersTable->setItem(row, 2, statusItem);
        
        // Items count
        QTableWidgetItem* itemsItem = new QTableWidgetItem(
            QString("%1 items").arg(order.items.size()));
        itemsItem->setFlags(itemsItem->flags() & ~Qt::ItemIsEditable);
        ordersTable->setItem(row, 3, itemsItem);
        
        // Total
        QTableWidgetItem* totalItem = new QTableWidgetItem(
            QString("$%1").arg(order.totalAmount, 0, 'f', 2));
        totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
        ordersTable->setItem(row, 4, totalItem);
    }
}

void OrderHistoryPage::updateOrders()
{
    qDebug() << "updateOrders() called";
    if (checkAccess()) {
        loadOrders();
    } else {
        qDebug() << "Access check failed in updateOrders";
    }
}

void OrderHistoryPage::filterOrders()
{
    qDebug() << "filterOrders() called";
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        emit loginRequired();
        return;
    }
    
    QString status = statusFilter->currentData().toString();
    QDateTime startDate = startDateFilter->dateTime();
    QDateTime endDate = endDateFilter->dateTime();
    
    QList<Order> orders;
    if (!status.isEmpty()) {
        orders = dbManager.getUserOrdersByStatus(userId, status);
    } else {
        orders = dbManager.getUserOrdersByDateRange(userId, startDate, endDate);
    }
    
    ordersTable->setRowCount(0);
    for (const Order& order : orders) {
        if ((status.isEmpty() || order.status == status) &&
            order.orderDate >= startDate && order.orderDate <= endDate) {
            int row = ordersTable->rowCount();
            ordersTable->insertRow(row);
            
            QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(order.id));
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
            ordersTable->setItem(row, 0, idItem);
            
            QTableWidgetItem* dateItem = new QTableWidgetItem(
                order.orderDate.toString("yyyy-MM-dd hh:mm:ss"));
            dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
            ordersTable->setItem(row, 1, dateItem);
            
            QTableWidgetItem* statusItem = new QTableWidgetItem(order.status);
            statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
            ordersTable->setItem(row, 2, statusItem);
            
            QTableWidgetItem* itemsItem = new QTableWidgetItem(
                QString("%1 items").arg(order.items.size()));
            itemsItem->setFlags(itemsItem->flags() & ~Qt::ItemIsEditable);
            ordersTable->setItem(row, 3, itemsItem);
            
            QTableWidgetItem* totalItem = new QTableWidgetItem(
                QString("$%1").arg(order.totalAmount, 0, 'f', 2));
            totalItem->setFlags(totalItem->flags() & ~Qt::ItemIsEditable);
            ordersTable->setItem(row, 4, totalItem);
        }
    }
}

void OrderHistoryPage::resetFilters()
{
    statusFilter->setCurrentIndex(0);
    startDateFilter->setDateTime(QDateTime::currentDateTime().addMonths(-1));
    endDateFilter->setDateTime(QDateTime::currentDateTime());
    loadOrders();
}

void OrderHistoryPage::viewOrderDetails(int orderId)
{
    Order order = dbManager.getOrderById(orderId);
    if (order.id == -1) {
        QMessageBox::warning(this, "Error", "Order not found");
        return;
    }
    
    showOrderDetailsDialog(order);
}

void OrderHistoryPage::showOrderDetailsDialog(const Order& order)
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString("Order Details - #%1").arg(order.id));
    dialog.setModal(true);
    dialog.setMinimumWidth(600);
    dialog.setStyleSheet(
        "QDialog {"
        "    background-color: white;"
        "}"
        "QLabel {"
        "    color: #2c3e50;"
        "    font-size: 13px;"
        "}"
        "QLabel#headerLabel {"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    padding-bottom: 15px;"
        "}"
        "QTableWidget {"
        "    color: #2c3e50;"
        "}"
        "QTableWidget::item {"
        "    color: #2c3e50;"
        "    font-size: 13px;"
        "}"
    );
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setSpacing(20);
    layout->setContentsMargins(25, 25, 25, 25);
    
    // Order header
    QLabel* headerLabel = new QLabel(QString("Order Details #%1").arg(order.id), &dialog);
    headerLabel->setObjectName("headerLabel");
    layout->addWidget(headerLabel);
    
    // Order info in a styled container
    QWidget* infoContainer = new QWidget(&dialog);
    infoContainer->setStyleSheet(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    border-radius: 10px;"
        "    padding: 20px;"
        "}"
        "QLabel {"
        "    font-size: 14px;"
        "}"
        "QLabel.label {"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    margin-right: 10px;"
        "}"
        "QLabel.value {"
        "    color: #2c3e50;"
        "}"
    );
    
    QGridLayout* infoLayout = new QGridLayout(infoContainer);
    infoLayout->setSpacing(15);
    infoLayout->setColumnMinimumWidth(0, 120);
    
    auto addInfoRow = [&](int row, const QString& label, const QString& value) {
        QLabel* labelWidget = new QLabel(label, &dialog);
        labelWidget->setProperty("class", "label");
        QLabel* valueWidget = new QLabel(value, &dialog);
        valueWidget->setProperty("class", "value");
        infoLayout->addWidget(labelWidget, row, 0);
        infoLayout->addWidget(valueWidget, row, 1);
    };
    
    addInfoRow(0, "Order Date:", order.orderDate.toString("MMMM d, yyyy hh:mm:ss"));
    addInfoRow(1, "Status:", getStatusWithIcon(order.status));
    
    layout->addWidget(infoContainer);
    
    // Items table with modern styling
    QTableWidget* itemsTable = new QTableWidget(&dialog);
    itemsTable->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 10px;"
        "    gridline-color: #f0f0f0;"
        "    color: #2c3e50;"
        "}"
        "QTableWidget::item {"
        "    padding: 12px 8px;"
        "    border-bottom: 1px solid #f0f0f0;"
        "    color: #2c3e50;"
        "    font-size: 13px;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f8f9fa;"
        "    color: #2c3e50;"
        "    padding: 12px 10px;"
        "    border: none;"
        "    border-bottom: 2px solid #e0e0e0;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "}"
    );
    
    itemsTable->setColumnCount(5);
    itemsTable->setHorizontalHeaderLabels({"Product", "Quantity", "Price", "Subtotal", "Actions"});
    itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemsTable->setShowGrid(false);
    itemsTable->verticalHeader()->setVisible(false);
    itemsTable->setAlternatingRowColors(true);
    
    for (const OrderItem& item : order.items) {
        int row = itemsTable->rowCount();
        itemsTable->insertRow(row);
        
        itemsTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        itemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.quantity)));
        itemsTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(item.price, 0, 'f', 2)));
        
        double subtotal = item.price * item.quantity;
        itemsTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(subtotal, 0, 'f', 2)));
        
        if (order.status == "Delivered") {
            QPushButton* reviewButton = new QPushButton(
                dbManager.getUserProductReview(authManager.getCurrentUserId(), item.productId).id != -1
                ? "Update Review"
                : "Add Review",
                &dialog
            );
            reviewButton->setStyleSheet(
                "QPushButton {"
                "    background-color: #3498db;"
                "    color: white;"
                "    border: none;"
                "    padding: 5px 10px;"
                "    border-radius: 3px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #2980b9;"
                "}"
            );
            
            itemsTable->setCellWidget(row, 4, reviewButton);
            connect(reviewButton, &QPushButton::clicked, this, [this, item]() {
                showReviewDialog(item);
            });
        }
    }
    
    layout->addWidget(itemsTable);
    
    // Total amount
    QLabel* totalLabel = new QLabel(QString("Total Amount: $%1").arg(order.totalAmount, 0, 'f', 2), &dialog);
    totalLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #27ae60;"
        "padding: 10px 0;"
    );
    totalLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(totalLabel);
    
    // Close button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    buttonBox->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
    );
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    dialog.exec();
}

QString OrderHistoryPage::getStatusWithIcon(const QString& status)
{
    if (status == "Pending") return "🕒 Pending";
    if (status == "Processing") return "⚙️ Processing";
    if (status == "Shipped") return "🚚 Shipped";
    if (status == "Delivered") return "✅ Delivered";
    if (status == "Cancelled") return "❌ Cancelled";
    return status;
}

QColor OrderHistoryPage::getStatusColor(const QString& status)
{
    if (status == "Pending") return QColor("#f39c12");
    if (status == "Processing") return QColor("#3498db");
    if (status == "Shipped") return QColor("#2ecc71");
    if (status == "Delivered") return QColor("#27ae60");
    if (status == "Cancelled") return QColor("#e74c3c");
    return QColor("#95a5a6");
}

void OrderTableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 2) { // Status column
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        // Get the status text
        QString status = index.data().toString();
        QString displayText = OrderHistoryPage::getStatusWithIcon(status);
        
        // Draw with custom colors
        painter->save();
        
        // Draw background
        QColor bgColor = OrderHistoryPage::getStatusColor(status);
        painter->fillRect(option.rect, bgColor);
        
        // Draw text
        painter->setPen(Qt::white);
        QRect textRect = option.rect;
        textRect.setLeft(textRect.left() + 8);
        painter->drawText(textRect, Qt::AlignVCenter, displayText);
        
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void OrderHistoryPage::setupReviewDialog(QDialog& dialog, int& rating, QString& comment)
{
    dialog.setWindowTitle("Write Review");
    dialog.setModal(true);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Rating
    QHBoxLayout* ratingLayout = new QHBoxLayout();
    QLabel* ratingLabel = new QLabel("Rating (1-5 stars):", &dialog);
    QSpinBox* ratingSpinBox = new QSpinBox(&dialog);
    ratingSpinBox->setRange(1, 5);
    ratingSpinBox->setValue(5);
    ratingLayout->addWidget(ratingLabel);
    ratingLayout->addWidget(ratingSpinBox);
    layout->addLayout(ratingLayout);
    
    // Comment
    QLabel* commentLabel = new QLabel("Your Review:", &dialog);
    layout->addWidget(commentLabel);
    
    QTextEdit* commentEdit = new QTextEdit(&dialog);
    commentEdit->setPlaceholderText("Write your review here...");
    layout->addWidget(commentEdit);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        rating = ratingSpinBox->value();
        comment = commentEdit->toPlainText();
    } else {
        rating = 0;
        comment = QString();
    }
}

void OrderHistoryPage::showReviewDialog(const OrderItem& item)
{
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        emit loginRequired();
        return;
    }
    
    // Check if user has already reviewed this product
    Review existingReview = dbManager.getUserProductReview(userId, item.productId);
    
    QDialog dialog(this);
    int rating = 0;
    QString comment;
    
    if (existingReview.id != -1) {
        // Pre-fill existing review
        dialog.setWindowTitle("Update Review");
        rating = existingReview.rating;
        comment = existingReview.comment;
    }
    
    setupReviewDialog(dialog, rating, comment);
    
    // Only proceed with submission if we got valid input from the dialog
    if (rating > 0 && !comment.isEmpty()) {
        submitReview(item.productId, rating, comment);
    }
}

bool OrderHistoryPage::submitReview(int productId, int rating, const QString& comment)
{
    int userId = authManager.getCurrentUserId();
    if (userId == -1) {
        emit loginRequired();
        return false;
    }
    
    Review review;
    review.productId = productId;
    review.userId = userId;
    review.username = authManager.getCurrentUserEmail();
    review.rating = rating;
    review.comment = comment;
    review.reviewDate = QDateTime::currentDateTime();
    
    // Check if user has already reviewed this product
    Review existingReview = dbManager.getUserProductReview(userId, productId);
    bool success;
    
    if (existingReview.id != -1) {
        // Update existing review
        review.id = existingReview.id;
        success = dbManager.updateReview(review);
    } else {
        // Add new review
        success = dbManager.addReview(review);
    }
    
    if (success) {
        QMessageBox::information(this, "Success", 
            existingReview.id != -1 ? "Review updated successfully!" : "Review added successfully!");
        emit reviewAdded();
        return true;
    } else {
        QMessageBox::warning(this, "Error", "Failed to save review. Please try again.");
        return false;
    }
} 