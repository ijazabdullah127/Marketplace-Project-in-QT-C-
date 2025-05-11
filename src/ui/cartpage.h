#ifndef CARTPAGE_H
#define CARTPAGE_H

#include "protectedpage.h"
#include "../database/databasemanager.h"
#include "../auth/authmanager.h"
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QShowEvent>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>

class CartPage : public ProtectedPage {
    Q_OBJECT

public:
    explicit CartPage(QWidget *parent = nullptr);

signals:
    void loginRequired();
    void orderPlaced();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void updateCart();
    void checkout();
    void removeSelectedItem();

private:
    void setupUI();
    void loadCart();
    void updateTotal();

    QTableWidget* cartTable;
    QPushButton* checkoutButton;
    QPushButton* removeButton;
    QLabel* totalLabel;
    QVBoxLayout* mainLayout;
    double total;
    DatabaseManager& dbManager;
    AuthManager& authManager;
};

#endif // CARTPAGE_H