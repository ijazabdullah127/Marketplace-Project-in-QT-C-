void MainWindow::onLogoutButtonClicked()
{
    // Clear session data
    authManager.logout();
    clearSessionCookie();
    sessionCheckTimer->stop();
    currentUserEmail.clear();

    // Reset UI state
    if (adminDashboard) {
        adminDashboard->hide();  // Hide instead of close
        adminDashboard->deleteLater();  // Schedule for deletion
        adminDashboard = nullptr;
    }

    // Get the dashboard page and clear its layout
    QWidget* dashboardPage = ui->stackedWidget->widget(2);
    if (dashboardPage && dashboardPage->layout()) {
        QLayout* oldLayout = dashboardPage->layout();
        while (QLayoutItem* item = oldLayout->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
        dashboardPage->setLayout(nullptr);
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

void MainWindow::onLoginSuccess()
{
    // Get the dashboard page and clear any existing layout
    QWidget* dashboardPage = ui->stackedWidget->widget(2);
    if (dashboardPage && dashboardPage->layout()) {
        QLayout* oldLayout = dashboardPage->layout();
        while (QLayoutItem* item = oldLayout->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
        dashboardPage->setLayout(nullptr);
    }

    // Show the main page which will rebuild the layout with correct permissions
    showMainPage();
}

void MainWindow::showMainPage()
{
    // Get the current user's email and check admin status
    QString email = authManager.getCurrentUserEmail();
    User user = DatabaseManager::getInstance().getUserByEmail(email);
    
    // If user is not found or not authenticated, show login page
    if (email.isEmpty() || !authManager.isAuthenticated()) {
        showLoginPage();
        return;
    }

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

void MainWindow::setupPages() {
    loginPage = new LoginPage(this);
    registrationPage = new RegistrationPage(this);
    adminDashboard = new AdminDashboard(this);
    sellerDashboard = new SellerDashboard(this);
    productListingPage = new ProductListingPage(this);
    productBrowsePage = new ProductBrowsePage(this);
    cartPage = new CartPage(this);
    orderHistoryPage = new OrderHistoryPage(this);

    ui->stackedWidget->addWidget(loginPage);
    ui->stackedWidget->addWidget(registrationPage);
    ui->stackedWidget->addWidget(adminDashboard);
    ui->stackedWidget->addWidget(sellerDashboard);
    ui->stackedWidget->addWidget(productListingPage);
    ui->stackedWidget->addWidget(productBrowsePage);
    ui->stackedWidget->addWidget(cartPage);
    ui->stackedWidget->addWidget(orderHistoryPage);

    // Connect signals
    connect(loginPage, &LoginPage::loginSuccessful, this, &MainWindow::onLoginSuccess);
    connect(registrationPage, &RegistrationPage::registrationSuccessful, this, &MainWindow::onRegistrationSuccess);
    connect(adminDashboard, &AdminDashboard::logout, this, &MainWindow::onLogoutButtonClicked);
    connect(sellerDashboard, &SellerDashboard::logout, this, &MainWindow::onLogoutButtonClicked);
}

void MainWindow::showUserSpecificPages() {
    hideAllPages();

    if (!authManager.isAuthenticated()) {
        showLoginPage();
        return;
    }

    QString email = authManager.getCurrentUserEmail();
    User user = DatabaseManager::getInstance().getUserByEmail(email);

    if (user.isAdmin()) {
        // Show admin pages
        adminDashboard->show();
        ui->stackedWidget->setCurrentWidget(adminDashboard);
    }
    else if (DatabaseManager::getInstance().isUserSeller(email)) {
        // Show seller pages
        sellerDashboard->show();
        productListingPage->show();
        orderHistoryPage->show();
        ui->stackedWidget->setCurrentWidget(sellerDashboard);
    }
    else {
        // Show consumer pages
        productBrowsePage->show();
        cartPage->show();
        orderHistoryPage->show();
        ui->stackedWidget->setCurrentWidget(productBrowsePage);
    }
}

void MainWindow::hideAllPages() {
    adminDashboard->hide();
    sellerDashboard->hide();
    productListingPage->hide();
    productBrowsePage->hide();
    cartPage->hide();
    orderHistoryPage->hide();
}

void MainWindow::onLoginSuccess(const QString& email) {
    currentUserEmail = email;
    showUserSpecificPages();
    sessionCheckTimer->start(30000); // Check session every 30 seconds
}

void MainWindow::checkSession() {
    if (!authManager.isAuthenticated()) {
        onLogoutButtonClicked();
    }
} 