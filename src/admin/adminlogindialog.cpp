#include "adminlogindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "../database/databasemanager.h"

AdminLoginDialog::AdminLoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Admin Login");
    setFixedWidth(300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Username field
    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameEdit = new QLineEdit(this);
    mainLayout->addWidget(usernameLabel);
    mainLayout->addWidget(usernameEdit);

    // Password field
    QLabel *passwordLabel = new QLabel("Password:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);

    // Error label
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->hide();
    mainLayout->addWidget(errorLabel);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Login", this);
    cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &AdminLoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &AdminLoginDialog::onCancelClicked);
}

void AdminLoginDialog::onLoginClicked() {
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Please enter both username and password");
        errorLabel->show();
        return;
    }

    // TODO: Implement actual admin authentication
    // For now, we'll use a simple hardcoded check
    if (username == "admin" && password == "admin123") {
        QDialog::accept();
    } else {
        errorLabel->setText("Invalid credentials");
        errorLabel->show();
    }
}

void AdminLoginDialog::onCancelClicked() {
    QDialog::reject();
} 