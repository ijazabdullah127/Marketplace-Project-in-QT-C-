#ifndef PRODUCTBROWSEPAGE_H
#define PRODUCTBROWSEPAGE_H

#include "protectedpage.h"
#include "../database/databasemanager.h"
#include "../auth/authmanager.h"
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QNetworkAccessManager>
#include <QMouseEvent>
#include <QSpinBox>
#include <QDialog>
#include <QTextEdit>

class ProductWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProductWidget(const Product& product, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();
    void showReviewsDialog();
    void showAddReviewDialog();
    void setupReviewsSection(QVBoxLayout* layout, const QList<Review>& reviews);

signals:
    void clicked(const Product& product);
    void addToCartClicked(const Product& product);
    void reviewAdded();

private:
    Product product;
    QLabel* imageLabel;
    QLabel* nameLabel;
    QLabel* priceLabel;
    QLabel* ratingLabel;
    DatabaseManager& dbManager;
};

class ProductBrowsePage : public ProtectedPage {
    Q_OBJECT

public:
    explicit ProductBrowsePage(QWidget *parent = nullptr);

signals:
    void productSelected(const Product& product);
    void loginRequired();

private slots:
    void handleProductClicked(const Product& product);
    void updateProducts();
    void displayProducts();
    void handleReviewAdded();
    void onFilterChanged();
    void onSortChanged();
    void onSearchTextChanged(const QString& text);
    void onProductsFetchedSuccess(const QVector<Product>& products);
    void onProductsFetchedFailed(const QString& error);
    void onAddToCartClicked(int productId, int quantity);

private:
    void setupUI();
    void fetchProducts();
    void displayProductReviews(const Product& product, QDialog& dialog);
    void setupReviewsSection(QVBoxLayout* layout, const QList<Review>& reviews);
    void clearProducts();
    void showAddToCartDialog(const Product& product);
    void showReviewsDialog(const Product& product);

    QScrollArea* scrollArea;
    QWidget* productsContainer;
    QGridLayout* productsLayout;
    QComboBox* categoryFilter;
    QComboBox* sortComboBox;
    QLineEdit* searchEdit;
    QVector<Product> products;
    QVector<Product> filteredProducts;
    QMap<int, QWidget*> productWidgets;
    QNetworkAccessManager* networkManager;
    DatabaseManager& dbManager;
    AuthManager& authManager;
    QVBoxLayout* mainLayout;
};

#endif // PRODUCTBROWSEPAGE_H 
