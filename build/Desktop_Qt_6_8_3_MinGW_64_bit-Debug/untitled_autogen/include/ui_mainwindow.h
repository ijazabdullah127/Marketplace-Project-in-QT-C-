/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *loginPage;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLineEdit *loginEmail;
    QLineEdit *loginPassword;
    QPushButton *loginButton;
    QPushButton *showRegisterButton;
    QWidget *registerPage;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QLineEdit *registerEmail;
    QLineEdit *registerUsername;
    QLineEdit *registerPassword;
    QPushButton *registerButton;
    QPushButton *showLoginButton;
    QWidget *mainPage;
    QVBoxLayout *verticalLayout_4;
    QLabel *welcomeLabel;
    QPushButton *logoutButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(400, 300);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        loginPage = new QWidget();
        loginPage->setObjectName("loginPage");
        verticalLayout_2 = new QVBoxLayout(loginPage);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(loginPage);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        loginEmail = new QLineEdit(loginPage);
        loginEmail->setObjectName("loginEmail");

        verticalLayout_2->addWidget(loginEmail);

        loginPassword = new QLineEdit(loginPage);
        loginPassword->setObjectName("loginPassword");
        loginPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(loginPassword);

        loginButton = new QPushButton(loginPage);
        loginButton->setObjectName("loginButton");

        verticalLayout_2->addWidget(loginButton);

        showRegisterButton = new QPushButton(loginPage);
        showRegisterButton->setObjectName("showRegisterButton");

        verticalLayout_2->addWidget(showRegisterButton);

        stackedWidget->addWidget(loginPage);
        registerPage = new QWidget();
        registerPage->setObjectName("registerPage");
        verticalLayout_3 = new QVBoxLayout(registerPage);
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_2 = new QLabel(registerPage);
        label_2->setObjectName("label_2");

        verticalLayout_3->addWidget(label_2);

        registerEmail = new QLineEdit(registerPage);
        registerEmail->setObjectName("registerEmail");

        verticalLayout_3->addWidget(registerEmail);

        registerUsername = new QLineEdit(registerPage);
        registerUsername->setObjectName("registerUsername");

        verticalLayout_3->addWidget(registerUsername);

        registerPassword = new QLineEdit(registerPage);
        registerPassword->setObjectName("registerPassword");
        registerPassword->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(registerPassword);

        registerButton = new QPushButton(registerPage);
        registerButton->setObjectName("registerButton");

        verticalLayout_3->addWidget(registerButton);

        showLoginButton = new QPushButton(registerPage);
        showLoginButton->setObjectName("showLoginButton");

        verticalLayout_3->addWidget(showLoginButton);

        stackedWidget->addWidget(registerPage);
        mainPage = new QWidget();
        mainPage->setObjectName("mainPage");
        verticalLayout_4 = new QVBoxLayout(mainPage);
        verticalLayout_4->setObjectName("verticalLayout_4");
        welcomeLabel = new QLabel(mainPage);
        welcomeLabel->setObjectName("welcomeLabel");

        verticalLayout_4->addWidget(welcomeLabel);

        logoutButton = new QPushButton(mainPage);
        logoutButton->setObjectName("logoutButton");

        verticalLayout_4->addWidget(logoutButton);

        stackedWidget->addWidget(mainPage);

        verticalLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Marketplace", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        loginEmail->setPlaceholderText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        loginPassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        loginButton->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        showRegisterButton->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        registerEmail->setPlaceholderText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        registerUsername->setPlaceholderText(QCoreApplication::translate("MainWindow", "Username", nullptr));
        registerPassword->setPlaceholderText(QCoreApplication::translate("MainWindow", "Password", nullptr));
        registerButton->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        showLoginButton->setText(QCoreApplication::translate("MainWindow", "Back to Login", nullptr));
        welcomeLabel->setText(QCoreApplication::translate("MainWindow", "Welcome!", nullptr));
        logoutButton->setText(QCoreApplication::translate("MainWindow", "Logout", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
