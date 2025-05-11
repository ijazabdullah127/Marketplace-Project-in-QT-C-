#ifndef ADMINLOGINDIALOG_H
#define ADMINLOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class AdminLoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit AdminLoginDialog(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onCancelClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;
};

#endif // ADMINLOGINDIALOG_H 