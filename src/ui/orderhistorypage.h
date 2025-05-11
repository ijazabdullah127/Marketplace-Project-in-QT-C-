#ifndef ORDERHISTORYPAGE_H
#define ORDERHISTORYPAGE_H

#include "protectedpage.h"
#include "../database/databasemanager.h"
#include "../auth/authmanager.h"
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QShowEvent>
#include <QSpinBox>
#include <QTextEdit>
#include <QStyledItemDelegate>

// Forward declarations
class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

class OrderTableItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit OrderTableItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class OrderHistoryPage : public ProtectedPage {
    Q_OBJECT

public:
    explicit OrderHistoryPage(QWidget *parent = nullptr);
    static QString getStatusWithIcon(const QString& status);
    static QColor getStatusColor(const QString& status);

signals:
    void loginRequired();
    void reviewAdded();  // Signal to notify when a review is added

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void updateOrders();
    void filterOrders();
    void viewOrderDetails(int orderId);
    void resetFilters();
    void showReviewDialog(const OrderItem& item);
    bool submitReview(int productId, int rating, const QString& comment);

private:
    void setupUI();
    void loadOrders();
    void showOrderDetailsDialog(const Order& order);
    void setupReviewDialog(QDialog& dialog, int& rating, QString& comment);

    QTableWidget* ordersTable;
    QComboBox* statusFilter;
    QDateTimeEdit* startDateFilter;
    QDateTimeEdit* endDateFilter;
    QPushButton* filterButton;
    QPushButton* resetButton;
    QVBoxLayout* mainLayout;
    DatabaseManager& dbManager;
    AuthManager& authManager;
};

#endif // ORDERHISTORYPAGE_H 