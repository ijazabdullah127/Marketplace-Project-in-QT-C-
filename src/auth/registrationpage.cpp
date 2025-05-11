void RegistrationPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel *titleLabel = new QLabel("Create Account", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2c3e50;"
        "margin-bottom: 20px;"
    );
    layout->addWidget(titleLabel, 0, Qt::AlignHCenter);

    // Email input
    emailInput = new QLineEdit(this);
    emailInput->setPlaceholderText("Email");
    emailInput->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
    );
    layout->addWidget(emailInput);

    // Username input
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Username");
    usernameInput->setStyleSheet(emailInput->styleSheet());
    layout->addWidget(usernameInput);

    // Password input
    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setStyleSheet(emailInput->styleSheet());
    layout->addWidget(passwordInput);

    // Confirm password input
    confirmPasswordInput = new QLineEdit(this);
    confirmPasswordInput->setPlaceholderText("Confirm Password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    confirmPasswordInput->setStyleSheet(emailInput->styleSheet());
    layout->addWidget(confirmPasswordInput);

    // Seller checkbox
    sellerCheckbox = new QCheckBox("Register as a Seller", this);
    sellerCheckbox->setStyleSheet(
        "QCheckBox {"
        "    font-size: 14px;"
        "    color: #2c3e50;"
        "    padding: 5px;"
        "}"
        "QCheckBox::indicator {"
        "    width: 18px;"
        "    height: 18px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    border: 2px solid #bdc3c7;"
        "    background: white;"
        "    border-radius: 3px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    border: 2px solid #2ecc71;"
        "    background: #2ecc71;"
        "    border-radius: 3px;"
        "}"
    );
    layout->addWidget(sellerCheckbox);

    // Register button
    registerButton = new QPushButton("Create Account", this);
    registerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 12px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #219a52;"
        "}"
    );
    layout->addWidget(registerButton);

    // Login link
    QHBoxLayout *loginLayout = new QHBoxLayout();
    QLabel *loginLabel = new QLabel("Already have an account?", this);
    loginLabel->setStyleSheet("color: #7f8c8d;");
    loginLink = new QPushButton("Login", this);
    loginLink->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    color: #3498db;"
        "    font-weight: bold;"
        "    text-decoration: underline;"
        "}"
        "QPushButton:hover {"
        "    color: #2980b9;"
        "}"
    );
    loginLayout->addWidget(loginLabel);
    loginLayout->addWidget(loginLink);
    loginLayout->setAlignment(Qt::AlignCenter);
    layout->addLayout(loginLayout);

    connect(registerButton, &QPushButton::clicked, this, &RegistrationPage::onRegisterClicked);
    connect(loginLink, &QPushButton::clicked, this, &RegistrationPage::onLoginLinkClicked);
}

void RegistrationPage::onRegisterClicked() {
    if (!validateInputs()) {
        return;
    }

    QString email = emailInput->text().trimmed();
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();
    bool isSeller = sellerCheckbox->isChecked();

    // Hash the password
    QByteArray hashedPassword = QCryptographicHash::hash(
        password.toUtf8(), QCryptographicHash::Sha256).toHex();

    if (db.createUser(email, username, QString(hashedPassword), false, isSeller)) {
        QMessageBox::information(this, "Success", "Registration successful! Please login.");
        emit registrationSuccessful();
    } else {
        QMessageBox::warning(this, "Error", "Failed to register. Email might already be in use.");
    }
} 