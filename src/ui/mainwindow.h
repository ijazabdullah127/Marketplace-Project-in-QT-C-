#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include "../auth/loginpage.h"
#include "../auth/registrationpage.h"
#include "../admin/admindashboard.h"
#include "../product/productlistingpage.h"
#include "../product/productbrowsepage.h"
#include "../cart/cartpage.h"
#include "../order/orderhistorypage.h"
#include "../seller/sellerdashboard.h"
#include "../database/databasemanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginSuccess(const QString& email);
    void onRegistrationSuccess();
    void onLogoutButtonClicked();
    void onLogoutSuccess();
    void checkSession();
    void showMainPage();
    void showLoginPage();
    void showRegistrationPage();

private:
    QStackedWidget *stackedWidget;
    LoginPage *loginPage;
    RegistrationPage *registrationPage;
    AdminDashboard *adminDashboard;
    SellerDashboard *sellerDashboard;
    QWidget *dashboardPage;
    ProductListingPage *productListingPage;
    ProductBrowsePage *productBrowsePage;
    CartPage *cartPage;
    OrderHistoryPage *orderHistoryPage;
    QTimer *sessionCheckTimer;
    DatabaseManager& db;
    QString currentUserEmail;

    void setupUI();
    void setupPages();
    void showUserSpecificPages();
    void hideAllPages();
    bool isUserAuthenticated() const;
    void clearSessionCookie();
};

#endif // MAINWINDOW_H 