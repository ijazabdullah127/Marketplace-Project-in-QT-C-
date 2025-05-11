#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>
#include "src/auth/authmanager.h"
#include "src/ui/orderhistorypage.h"
#include "src/ui/cartpage.h"
#include "src/ui/productlistingpage.h"
#include "src/ui/productbrowsepage.h"
#include "src/admin/adminlogindialog.h"
#include "src/admin/admindashboard.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRegisterButtonClicked();
    void onLoginButtonClicked();
    void onLogoutButtonClicked();
    void onRegistrationSuccess();
    void onRegistrationFailed(const QString& error);
    void onLoginSuccess();
    void onLoginFailed(const QString& error);
    void onLogoutSuccess();
    void onAccessDenied();
    void onViewOrderHistory();
    void onViewCart();
    void onViewProductListing();
    void onViewProductBrowse();
    void handleSessionToken(const QString& token);
    void checkSessionValidity();
    void onAdminButtonClicked();

private:
    void setupUI();
    void showLoginPage();
    void showRegistrationPage();
    void showMainPage();
    void setupNetworkManager();
    void saveSessionCookie(const QString& token);
    void loadSessionCookie();
    void clearSessionCookie();
    void setupProtectedPages();
    bool validateSessionAndNavigate();
    
    Ui::MainWindow *ui;
    AuthManager& authManager;
    QNetworkAccessManager* networkManager;
    QNetworkCookieJar* cookieJar;
    OrderHistoryPage* orderHistoryPage;
    CartPage* cartPage;
    ProductListingPage* productListingPage;
    ProductBrowsePage* productBrowsePage;
    QTimer* sessionCheckTimer;
    QPushButton* adminButton;
    AdminDashboard* adminDashboard;
};

#endif // MAINWINDOW_H
