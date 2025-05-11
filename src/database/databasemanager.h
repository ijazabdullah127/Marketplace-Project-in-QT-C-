#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QObject>
#include "../auth/user.h"

struct CartItem {
    int id;
    int userId;
    int productId;
    int quantity;
    double price;  // Price at time of adding to cart
    
    CartItem() : id(-1), userId(-1), productId(-1), quantity(0), price(0.0) {}
};

struct OrderItem {
    int id;
    int orderId;
    int productId;
    QString productName;
    int quantity;
    double price;
    
    OrderItem() : id(-1), orderId(-1), productId(-1), quantity(0), price(0.0) {}
};

struct Order {
    int id;
    int userId;
    QDateTime orderDate;
    QString status;
    double totalAmount;
    QList<OrderItem> items;
    
    Order() : id(-1), userId(-1), totalAmount(0.0) {}
};

class Product {
public:
    int id;
    QString name;
    QString description;
    double price;
    QByteArray imageData;
    int sellerId;
    QString category;
    QString imageUrl;  // URL for fetching the image
    int stock;        // Stock quantity
    
    Product() : id(-1), price(0.0), sellerId(-1), stock(0) {}
};

struct Review {
    int id;
    int productId;
    int userId;
    QString username;  // Store username for display
    int rating;       // 1-5 stars
    QString comment;
    QDateTime reviewDate;
    
    Review() : id(-1), productId(-1), userId(-1), rating(0) {}
};

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager& getInstance();
    
    bool initialize();
    bool addUser(const User& user);
    bool addProduct(const Product& product);
    bool updateProductStock(int productId, int newStock);
    bool decrementProductStock(int productId, int quantity);
    QList<Product> getProductsBySeller(int sellerId);
    QList<Product> getAllProducts();
    Product getProductById(int productId);
    User getUserByEmail(const QString& email);
    User getUserByUsername(const QString& username);
    User getUserById(int userId);
    bool userExists(const QString& email, const QString& username);
    int getUserIdByEmail(const QString& email);

    // Cart operations
    bool addToCart(int userId, int productId, int quantity);
    bool updateCartItemQuantity(int cartItemId, int quantity);
    bool removeFromCart(int cartItemId);
    QList<CartItem> getCartItems(int userId);
    bool clearCart(int userId);

    // Order operations
    bool createOrder(int userId, const QList<CartItem>& items);
    bool addOrderItem(int orderId, const CartItem& item);
    QList<Order> getUserOrders(int userId);
    QList<Order> getUserOrdersByDateRange(int userId, const QDateTime& startDate, const QDateTime& endDate);
    QList<Order> getUserOrdersByStatus(int userId, const QString& status);
    Order getOrderById(int orderId);
    bool updateOrderStatus(int orderId, const QString& status);

    // Review operations
    bool addReview(const Review& review);
    bool updateReview(const Review& review);
    bool deleteReview(int reviewId);
    Review getReviewById(int reviewId);
    QList<Review> getProductReviews(int productId);
    Review getUserProductReview(int userId, int productId);
    double getProductAverageRating(int productId);
    bool hasUserPurchasedProduct(int userId, int productId);

    // Admin operations
    bool isUserAdmin(int userId);
    bool suspendUser(int userId);
    bool unsuspendUser(int userId);
    bool resetUserPassword(int userId, const QString& newHashedPassword);
    bool deleteProduct(int productId);
    QList<User> getAllUsers();
    double getTotalSales();
    int getTotalOrders();
    double getAverageOrderValue();

    // User related methods
    bool createUser(const QString& email, const QString& username, const QString& password, bool isAdmin = false, bool isSeller = false);
    bool authenticateUser(const QString& email, const QString& password);
    bool isUserAdmin(const QString& email);
    bool isUserSeller(const QString& email);
    bool updateUserRole(int userId, bool isAdmin, bool isSeller);
    
private:
    DatabaseManager();
    ~DatabaseManager();
    
    QSqlDatabase db;
    bool createTables();
    bool createUsersTable();
    bool createProductsTable();
    bool createOrdersTable();
    bool createCartTable();
    bool createSellerTable();

    static DatabaseManager* instance;
};

#endif // DATABASEMANAGER_H 
