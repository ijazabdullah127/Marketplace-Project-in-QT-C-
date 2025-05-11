#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include "../database/databasemanager.h"
#include "../auth/user.h"

class AdminDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminDashboard(QWidget *parent = nullptr);

signals:
    void navigateHome();
    void logout();

private slots:
    void refreshUserList();
    void refreshProductList();
    void handleSuspendUser(const User& user, QPushButton* button);
    void handleResetPassword(const User& user);
    void refreshSalesReport();
    void onSuspendUserClicked();
    void onDeleteProductClicked();
    void onResetPasswordClicked();
    void onHomeClicked();
    void onLogoutClicked();

private:
    void setupUI();
    void setupUserManagement();
    void setupProductManagement();
    void setupSalesReport();

    QTabWidget* tabWidget;
    QWidget* userTab;
    QWidget* productTab;
    QWidget* salesTab;
    QTableWidget* userTable;
    QTableWidget* productTable;
    QPushButton* suspendUserButton;
    QPushButton* resetPasswordButton;
    QPushButton* deleteProductButton;
    QLabel* totalSalesLabel;
    QLabel* totalOrdersLabel;
    QLabel* averageOrderValueLabel;
    DatabaseManager& db;
};

#endif // ADMINDASHBOARD_H 