#ifndef SELLERDASHBOARD_H
#define SELLERDASHBOARD_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "../database/databasemanager.h"

class SellerDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit SellerDashboard(QWidget *parent = nullptr);

signals:
    void logout();
    void navigateHome();

private slots:
    void onHomeClicked();
    void onLogoutClicked();
    void onAddProductClicked();
    void onEditProductClicked();
    void onDeleteProductClicked();
    void onUpdateOrderStatusClicked();
    void refreshOrderList();
    void refreshProductList();

private:
    QTableWidget *orderTable;
    QTableWidget *productTable;
    QPushButton *addProductButton;
    QPushButton *editProductButton;
    QPushButton *deleteProductButton;
    QLabel *totalSalesLabel;
    QLabel *totalOrdersLabel;
    DatabaseManager& db;

    void setupUI();
    void setupOrderManagement();
    void setupProductManagement();
    void setupMetrics();
    void handleOrderStatusUpdate(int orderId, const QString& newStatus);
};

#endif // SELLERDASHBOARD_H 