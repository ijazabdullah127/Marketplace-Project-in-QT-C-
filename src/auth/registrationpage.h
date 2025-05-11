#ifndef REGISTRATIONPAGE_H
#define REGISTRATIONPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include "../database/databasemanager.h"

class RegistrationPage : public QWidget {
    Q_OBJECT

public:
    explicit RegistrationPage(QWidget *parent = nullptr);

signals:
    void registrationSuccessful();
    void switchToLogin();

private slots:
    void onRegisterClicked();
    void onLoginLinkClicked();

private:
    QLineEdit *emailInput;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QLineEdit *confirmPasswordInput;
    QCheckBox *sellerCheckbox;
    QPushButton *registerButton;
    QPushButton *loginLink;
    DatabaseManager& db;

    void setupUI();
    bool validateInputs();
};

#endif // REGISTRATIONPAGE_H 