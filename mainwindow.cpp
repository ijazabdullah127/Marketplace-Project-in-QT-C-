#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , authManager(AuthManager::getInstance())
    , networkManager(nullptr)
    , cookieJar(nullptr)
    , orderHistoryPage(nullptr)
    , cartPage(nullptr)
    , productListingPage(nullptr)
    , productBrowsePage(nullptr)
    , sessionCheckTimer(nullptr)
    , adminButton(nullptr)
    , adminDashboard(nullptr)
{
    ui->setupUi(this);
    
    // Set window properties
    setWindowTitle("Marketplace");
    
    // Completely remove menu bar and status bar
    setMenuBar(nullptr);
    setStatusBar(nullptr);
    
    setMinimumSize(800, 600);
    
    // Remove window frame elements and set fixed size
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(800, 600);
    
    setupNetworkManager();
    setupProtectedPages();
    
    // Connect signals
    connect(&authManager, &AuthManager::registrationSuccess, this, &MainWindow::onRegistrationSuccess);
    connect(&authManager, &AuthManager::registrationFailed, this, &MainWindow::onRegistrationFailed);
    connect(&authManager, &AuthManager::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(&authManager, &AuthManager::loginFailed, this, &MainWindow::onLoginFailed);
    connect(&authManager, &AuthManager::logoutSuccess, this, &MainWindow::onLogoutSuccess);
    
    // Connect UI buttons
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::onLoginButtonClicked);
    connect(ui->showRegisterButton, &QPushButton::clicked, this, &MainWindow::showRegistrationPage);
    connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterButtonClicked);
    connect(ui->showLoginButton, &QPushButton::clicked, this, &MainWindow::showLoginPage);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutButtonClicked);
    
    // Setup session check timer
    sessionCheckTimer = new QTimer(this);
    connect(sessionCheckTimer, &QTimer::timeout, this, &MainWindow::checkSessionValidity);
    sessionCheckTimer->start(300000); // Check every 5 minutes
    
    // Try to restore session from cookie
    loadSessionCookie();

    setupUI();
    
    // Ensure we start with login page
    showLoginPage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupProtectedPages()
{
    orderHistoryPage = new OrderHistoryPage(this);
    cartPage = new CartPage(this);
    productListingPage = new ProductListingPage(this);
    productBrowsePage = new ProductBrowsePage(this);
    
    // Connect access denied signals
    connect(orderHistoryPage, &ProtectedPage::accessDenied, this, &MainWindow::onAccessDenied);
    connect(cartPage, &ProtectedPage::accessDenied, this, &MainWindow::onAccessDenied);
    connect(productListingPage, &ProtectedPage::accessDenied, this, &MainWindow::onAccessDenied);
    connect(productBrowsePage, &ProtectedPage::accessDenied, this, &MainWindow::onAccessDenied);
    
    // Connect navigation signals
    connect(orderHistoryPage, &ProtectedPage::navigateHome, this, &MainWindow::showMainPage);
    connect(cartPage, &ProtectedPage::navigateHome, this, &MainWindow::showMainPage);
    connect(productListingPage, &ProtectedPage::navigateHome, this, &MainWindow::showMainPage);
    connect(productBrowsePage, &ProtectedPage::navigateHome, this, &MainWindow::showMainPage);
    
    // Connect logout signals
    connect(orderHistoryPage, &ProtectedPage::logout, this, &MainWindow::onLogoutButtonClicked);
    connect(cartPage, &ProtectedPage::logout, this, &MainWindow::onLogoutButtonClicked);
    connect(productListingPage, &ProtectedPage::logout, this, &MainWindow::onLogoutButtonClicked);
    connect(productBrowsePage, &ProtectedPage::logout, this, &MainWindow::onLogoutButtonClicked);
    
    
    
    // Add pages to stacked widget
    ui->stackedWidget->addWidget(orderHistoryPage);
    ui->stackedWidget->addWidget(cartPage);
    ui->stackedWidget->addWidget(productListingPage);
    ui->stackedWidget->addWidget(productBrowsePage);
}

void MainWindow::onAccessDenied()
{
    QMessageBox::warning(this, "Access Denied", "Please log in to access this page");
    showLoginPage();
}

bool MainWindow::validateSessionAndNavigate()
{
    if (!authManager.isAuthenticated()) {
        onAccessDenied();
        return false;
    }
    return true;
}

void MainWindow::onViewOrderHistory()
{
    if (validateSessionAndNavigate()) {
        orderHistoryPage->show();
        ui->stackedWidget->setCurrentWidget(orderHistoryPage);
    }
}

void MainWindow::onViewCart()
{
    if (validateSessionAndNavigate()) {
        cartPage->show();
        ui->stackedWidget->setCurrentWidget(cartPage);
    }
}

void MainWindow::onViewProductListing()
{
    if (validateSessionAndNavigate()) {
        productListingPage->show();
        ui->stackedWidget->setCurrentWidget(productListingPage);
    }
}

void MainWindow::onViewProductBrowse()
{
    if (validateSessionAndNavigate()) {
        productBrowsePage->show();
        ui->stackedWidget->setCurrentWidget(productBrowsePage);
    }
}

void MainWindow::checkSessionValidity()
{
    if (authManager.isAuthenticated()) {
        QString token = authManager.getCurrentSessionToken();
        if (!token.isEmpty()) {
            handleSessionToken(token);
        }
    }
}

void MainWindow::setupNetworkManager()
{
    networkManager = new QNetworkAccessManager(this);
    cookieJar = new QNetworkCookieJar(this);
    networkManager->setCookieJar(cookieJar);
}

void MainWindow::saveSessionCookie(const QString& token)
{
    QNetworkCookie cookie("session_token", token.toUtf8());
    cookie.setHttpOnly(true);
    cookie.setSecure(true);
    cookie.setPath("/");
    cookie.setExpirationDate(QDateTime::currentDateTime().addSecs(7 * 24 * 3600)); // 7 days to match session duration
    
    QList<QNetworkCookie> cookies;
    cookies.append(cookie);
    cookieJar->setCookiesFromUrl(cookies, QUrl("http://localhost"));
    
    // Also save to settings for persistence
    QSettings settings;
    settings.setValue("session_token", token);
    settings.sync();
}

void MainWindow::loadSessionCookie()
{
    QSettings settings;
    QString token = settings.value("session_token").toString();
    
    if (!token.isEmpty()) {
        saveSessionCookie(token);
        handleSessionToken(token);
    }
}

void MainWindow::clearSessionCookie()
{
    QNetworkCookie cookie("session_token", "");
    cookie.setHttpOnly(true);
    cookie.setSecure(true);
    cookie.setPath("/");
    cookie.setExpirationDate(QDateTime::currentDateTime().addSecs(-1)); // Expired
    
    QList<QNetworkCookie> cookies;
    cookies.append(cookie);
    cookieJar->setCookiesFromUrl(cookies, QUrl("http://localhost"));
    
    // Clear from settings
    QSettings settings;
    settings.remove("session_token");
    settings.sync();
}

void MainWindow::handleSessionToken(const QString& token)
{
    if (authManager.isAuthenticated() && !token.isEmpty()) {
        // Session is valid, update UI
        showMainPage();
    } else {
        // Session is invalid, clear everything and show login
        clearSessionCookie();
        showLoginPage();
    }
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentIndex(0);
    // Clear input fields
    ui->loginEmail->clear();
    ui->loginPassword->clear();
    // Hide protected pages
    orderHistoryPage->hide();
    cartPage->hide();
    productListingPage->hide();
    productBrowsePage->hide();
  
}

void MainWindow::showRegistrationPage()
{
    ui->stackedWidget->setCurrentIndex(1);
    // Clear input fields
    ui->registerEmail->clear();
    ui->registerUsername->clear();
    ui->registerPassword->clear();
}

void MainWindow::showMainPage()
{
    ui->stackedWidget->setCurrentIndex(2);
    
    // Get the dashboard page
    QWidget* dashboardPage = ui->stackedWidget->widget(2);
    
    // Clear any existing layout
    if (dashboardPage->layout()) {
        QLayout* oldLayout = dashboardPage->layout();
        while (QLayoutItem* item = oldLayout->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }

    // Create new layout
    QVBoxLayout* mainLayout = new QVBoxLayout(dashboardPage);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Navigation buttons at the top
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setAlignment(Qt::AlignRight);
    navLayout->setSpacing(10);
    
    QPushButton* homeBtn = new QPushButton("🏠 Home", dashboardPage);
    QPushButton* logoutBtn = new QPushButton("🚪 Logout", dashboardPage);
    
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
    
    connect(homeBtn, &QPushButton::clicked, this, &MainWindow::showMainPage);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutButtonClicked);
    
    navLayout->addWidget(homeBtn);
    navLayout->addWidget(logoutBtn);
    mainLayout->addLayout(navLayout);

    // Welcome text
    QLabel* welcomeLabel = new QLabel("Welcome, " + authManager.getCurrentUserUsername() + "!", dashboardPage);
    welcomeLabel->setStyleSheet(
        "font-size: 32px;"
        "font-weight: bold;"
        "color: #2c3e50;"
        "margin-bottom: 20px;"
    );
    welcomeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(welcomeLabel);

    // Create grid for main buttons
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);
    gridLayout->setAlignment(Qt::AlignCenter);

    // Create main option buttons
    QPushButton* orderHistoryBtn = new QPushButton("📋 Order History", dashboardPage);
    QPushButton* cartBtn = new QPushButton("🛒 Shopping Cart", dashboardPage);
    QPushButton* addProductBtn = new QPushButton("➕ Add Product", dashboardPage);
    QPushButton* browseProductsBtn = new QPushButton("🔍 Browse Products", dashboardPage);

    // Style for main buttons
    QString buttonStyle = 
        "QPushButton {"
        "    background-color: white;"
        "    color: #2c3e50;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    min-width: 200px;"
        "    min-height: 60px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border-color: #3498db;"
        "}";

    orderHistoryBtn->setStyleSheet(buttonStyle);
    cartBtn->setStyleSheet(buttonStyle);
    addProductBtn->setStyleSheet(buttonStyle);
    browseProductsBtn->setStyleSheet(buttonStyle);

    // Add buttons to grid
    gridLayout->addWidget(orderHistoryBtn, 0, 0);
    gridLayout->addWidget(cartBtn, 0, 1);
    gridLayout->addWidget(addProductBtn, 1, 0);
    gridLayout->addWidget(browseProductsBtn, 1, 1);

    // Add admin button if user is admin
    QString email = authManager.getCurrentUserEmail();
    User user = DatabaseManager::getInstance().getUserByEmail(email);
    if (user.isAdmin()) {
        QPushButton* adminBtn = new QPushButton("👑 Admin Dashboard", dashboardPage);
        adminBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #e74c3c;"
            "    color: white;"
            "    border: 2px solid #e74c3c;"
            "    border-radius: 15px;"
            "    padding: 20px;"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "    min-width: 200px;"
            "    min-height: 60px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c0392b;"
            "    border-color: #c0392b;"
            "}"
        );
        gridLayout->addWidget(adminBtn, 2, 0, 1, 2);
        connect(adminBtn, &QPushButton::clicked, this, &MainWindow::onAdminButtonClicked);
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    // Connect button signals
    connect(orderHistoryBtn, &QPushButton::clicked, this, &MainWindow::onViewOrderHistory);
    connect(cartBtn, &QPushButton::clicked, this, &MainWindow::onViewCart);
    connect(addProductBtn, &QPushButton::clicked, this, &MainWindow::onViewProductListing);
    connect(browseProductsBtn, &QPushButton::clicked, this, &MainWindow::onViewProductBrowse);

    dashboardPage->setLayout(mainLayout);
}

void MainWindow::onRegisterButtonClicked()
{
    QString email = ui->registerEmail->text();
    QString username = ui->registerUsername->text();
    QString password = ui->registerPassword->text();
    
    if (email.isEmpty() || username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Registration Failed", "Please fill in all fields");
        return;
    }
    
    authManager.registerUser(email, username, password);
}

void MainWindow::onLoginButtonClicked()
{
    QString email = ui->loginEmail->text();
    QString password = ui->loginPassword->text();
    
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Please fill in all fields");
        return;
    }
    
    authManager.login(email, password);
}

void MainWindow::onLogoutButtonClicked()
{
    // Clear session data
    authManager.logout();
    clearSessionCookie();
    sessionCheckTimer->stop();
    QWidget* dashboardPage = ui->stackedWidget->widget(2);
    if (dashboardPage) {
        // Look for admin buttons by name or property
        QList<QPushButton*> buttons = dashboardPage->findChildren<QPushButton*>();
        for (QPushButton* button : buttons) {
            if (button->text().contains("Admin Dashboard")) {
                button->disconnect(); // Disconnect any signals
                button->deleteLater(); // Schedule for deletion
            }
        }
    }
    // Reset UI state
    if (adminDashboard) {
        adminDashboard->hide();  // Hide instead of close
        adminDashboard->close(); 
        adminDashboard->deleteLater();  // Schedule for deletion
        adminDashboard = nullptr;
    }

    // Hide all protected pages
    if (orderHistoryPage) orderHistoryPage->hide();
    if (cartPage) cartPage->hide();
    if (productListingPage) productListingPage->hide();
    if (productBrowsePage) productBrowsePage->hide();

    // Reset stacked widget to login page
    ui->stackedWidget->setCurrentIndex(0);
    showLoginPage();
}

void MainWindow::onRegistrationSuccess()
{
    QMessageBox::information(this, "Success", "Registration successful! Please login.");
    showLoginPage();
}

void MainWindow::onRegistrationFailed(const QString& error)
{
    QMessageBox::warning(this, "Registration Failed", error);
}

void MainWindow::onLoginSuccess()
{
    QString email = authManager.getCurrentUserEmail();
    User user = DatabaseManager::getInstance().getUserByEmail(email);

    if (email.isEmpty() || !authManager.isAuthenticated()) {
        showLoginPage();
        return;
    }

    showMainPage();
}

void MainWindow::onLoginFailed(const QString& error)
{
    QMessageBox::warning(this, "Login Failed", error);
}

void MainWindow::onLogoutSuccess()
{
    clearSessionCookie();
    sessionCheckTimer->stop();
    showLoginPage();

    if (adminDashboard) {
        adminDashboard->hide();  // Hide instead of close
        adminDashboard->deleteLater();  // Schedule for deletion
        adminDashboard = nullptr;
    }
}

void MainWindow::onAdminButtonClicked()
{
    if (!validateSessionAndNavigate()) {
        return;
    }

    // Check if user is admin
    QString email = authManager.getCurrentUserEmail();
    User user = DatabaseManager::getInstance().getUserByEmail(email);
    
    if (!user.isAdmin()) {
        QMessageBox::warning(this, "Access Denied", "You do not have permission to access the admin dashboard.");
        return;
    }

        if (!adminDashboard) {
            adminDashboard = new AdminDashboard(this);
        ui->stackedWidget->addWidget(adminDashboard);
        
        // Connect admin dashboard signals
        connect(adminDashboard, &AdminDashboard::navigateHome, this, &MainWindow::showMainPage);
        connect(adminDashboard, &AdminDashboard::logout, this, &MainWindow::onLogoutButtonClicked);
    }
    
    adminDashboard->show();
    ui->stackedWidget->setCurrentWidget(adminDashboard);
}

void MainWindow::setupUI()
{
    // Set window style with no menu bar
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #f5f6fa;"
        "    border: none;"
        "}"
        "QStackedWidget {"
        "    background-color: #f5f6fa;"
        "}"
        "#centralwidget {"
        "    background-color: #f5f6fa;"
        "    border: none;"
        "    margin: 0;"
        "    padding: 0;"
        "}"
    );

    // Style the login page
    QWidget* loginPage = ui->stackedWidget->widget(0);
    loginPage->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f6fa;"
        "}"
        "QLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin-bottom: 20px;"
        "}"
    );

    // Update login page title
    ui->label->setText("🛍️ Marketplace");
    ui->label->setAlignment(Qt::AlignCenter);

    // Style email input
    ui->loginEmail->setStyleSheet(
        "QLineEdit {"
        "    padding: 12px;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    margin: 5px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
    );
    ui->loginEmail->setPlaceholderText("📧 Email");

    // Style password input
    ui->loginPassword->setStyleSheet(
        "QLineEdit {"
        "    padding: 12px;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    margin: 5px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
    );
    ui->loginPassword->setPlaceholderText("🔒 Password");

    // Style login button
    ui->loginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 8px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    margin: 10px 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2472a4;"
        "}"
    );
    ui->loginButton->setText("🔐 Login");

    // Style register button
    ui->showRegisterButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: #3498db;"
        "    border: none;"
        "    padding: 8px;"
        "    font-size: 14px;"
        "    margin: 5px;"
        "}"
        "QPushButton:hover {"
        "    color: #2980b9;"
        "    text-decoration: underline;"
        "}"
    );
    ui->showRegisterButton->setText("👤 Create New Account");

    // Style the registration page
    QWidget* registerPage = ui->stackedWidget->widget(1);
    registerPage->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f6fa;"
        "}"
        "QLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin-bottom: 20px;"
        "}"
    );

    // Update registration page title
    ui->label_2->setText("👤 Create Account");
    ui->label_2->setAlignment(Qt::AlignCenter);

    // Style registration inputs
    ui->registerEmail->setStyleSheet(
        "QLineEdit {"
        "    padding: 12px;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    margin: 5px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
    );
    ui->registerEmail->setPlaceholderText("📧 Email");

    ui->registerUsername->setStyleSheet(
        "QLineEdit {"
        "    padding: 12px;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    margin: 5px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
    );
    ui->registerUsername->setPlaceholderText("👤 Username");

    ui->registerPassword->setStyleSheet(
        "QLineEdit {"
        "    padding: 12px;"
        "    border: 2px solid #dcdde1;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    margin: 5px;"
        "    color: #2c3e50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #3498db;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #95a5a6;"
        "}"
    );
    ui->registerPassword->setPlaceholderText("🔒 Password");

    // Style register button
    ui->registerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 8px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    margin: 10px 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #219a52;"
        "}"
    );
    ui->registerButton->setText("✨ Create Account");

    // Style back to login button
    ui->showLoginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: #3498db;"
        "    border: none;"
        "    padding: 8px;"
        "    font-size: 14px;"
        "    margin: 5px;"
        "}"
        "QPushButton:hover {"
        "    color: #2980b9;"
        "    text-decoration: underline;"
        "}"
    );
    ui->showLoginButton->setText("← Back to Login");

    // Set margins for the pages
    ui->verticalLayout_2->setContentsMargins(40, 40, 40, 40);
    ui->verticalLayout_3->setContentsMargins(40, 40, 40, 40);
    ui->verticalLayout_2->setSpacing(15);
    ui->verticalLayout_3->setSpacing(15);

    // Create and style the main dashboard page (index 2)
    QWidget* dashboardPage = ui->stackedWidget->widget(2);
    dashboardPage->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f6fa;"
        "}"
        "QLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin-bottom: 20px;"
        "}"
        
        "QPushButton#adminButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border-color: #e74c3c;"
        "}"
        "QPushButton#adminButton:hover {"
        "    background-color: #c0392b;"
        "    border-color: #c0392b;"
        "}"
       
        "QPushButton#homeButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border-color: #2ecc71;"
        "}"
        "QPushButton#homeButton:hover {"
        "    background-color: #27ae60;"
        "    border-color: #27ae60;"
        "}"
        "QPushButton#logoutButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border-color: #e74c3c;"
        "}"
        "QPushButton#logoutButton:hover {"
        "    background-color: #c0392b;"
        "    border-color: #c0392b;"
        "}"
    );

    // Create dashboard layout
    QVBoxLayout* mainLayout = new QVBoxLayout(dashboardPage);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

  

    // Create grid layout for main options
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    

    
    // Add grid layout to main layout
    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

 

    

    // Add navigation buttons to all protected pages
    QList<QWidget*> protectedPages = {
        orderHistoryPage,
        cartPage,
        productListingPage,
        productBrowsePage,
        adminDashboard
    };

   
}
