#include "databasemanager.h"
#include <QDebug>
#include <QDir>
#include <QCryptographicHash>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QDir::currentPath() + "/marketplace.db";
    db.setDatabaseName(dbPath);
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize() {
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }
    qDebug() << "Database opened successfully at:" << db.databaseName();
    return createTables();
}

bool DatabaseManager::createTables() {
    bool success = true;
    success &= createUsersTable();
    success &= createProductsTable();
    success &= createOrdersTable();
    success &= createCartTable();
    success &= createSellerTable();
    return success;
}

bool DatabaseManager::createUsersTable() {
    QSqlQuery query;
    
    // Drop existing tables in reverse order of dependencies
   // QStringList tablesToDrop = {
       // "reviews",
      //  "order_items",
      //  "orders",
      //  "cart",
      //  "products",
      //  "users"
 //   };
    
   // for (const QString& table : tablesToDrop) {
    //    query.exec("DROP TABLE IF EXISTS " + table);
   //     if (query.lastError().isValid()) {
    //        qDebug() << "Error dropping table" << table << ":" << query.lastError().text();
    //   } else {
        //    qDebug() << "Successfully dropped table:" << table;
    //    }
   // }
    
    // Create users table
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "email TEXT UNIQUE NOT NULL,"
                 "username TEXT UNIQUE NOT NULL,"
                 "password TEXT NOT NULL,"
                 "is_admin BOOLEAN NOT NULL DEFAULT 0,"
                 "is_suspended BOOLEAN NOT NULL DEFAULT 0"
                 ")")) {
        qDebug() << "Error creating users table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Successfully created users table";
    
    // Check if admin user exists
    query.prepare("SELECT COUNT(*) FROM users WHERE email = 'admin@marketplace.com'");
    if (query.exec() && query.next() && query.value(0).toInt() == 0) {
        // Create default admin user if it doesn't exist
        QString hashedPassword = QCryptographicHash::hash(
            QString("admin123").toUtf8(),
            QCryptographicHash::Sha256
        ).toHex();
        
        query.prepare("INSERT INTO users (email, username, password, is_admin) VALUES (?, ?, ?, ?)");
        query.addBindValue("admin@marketplace.com");
        query.addBindValue("admin");
        query.addBindValue(hashedPassword);
        query.addBindValue(true);
        
        if (!query.exec()) {
            qDebug() << "Error creating admin user:" << query.lastError().text();
        } else {
            qDebug() << "Successfully created admin user";
        }
    }
    
    return true;
}

bool DatabaseManager::createProductsTable() {
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS products ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    description TEXT,"
        "    price REAL NOT NULL,"
        "    seller_id INTEGER NOT NULL,"
        "    category TEXT,"
        "    image_data BLOB,"
        "    image_url TEXT,"
        "    stock INTEGER NOT NULL DEFAULT 0,"
        "    FOREIGN KEY(seller_id) REFERENCES users(id)"
        ")"
    );
}

bool DatabaseManager::createOrdersTable() {
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS orders ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id INTEGER NOT NULL,"
        "    order_date DATETIME NOT NULL,"
        "    status TEXT NOT NULL,"
        "    total_amount REAL NOT NULL,"
        "    FOREIGN KEY (user_id) REFERENCES users(id)"
        ")"
    );
}

bool DatabaseManager::createCartTable() {
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS cart ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id INTEGER NOT NULL,"
        "    product_id INTEGER NOT NULL,"
        "    quantity INTEGER NOT NULL,"
        "    price REAL NOT NULL,"
        "    FOREIGN KEY (user_id) REFERENCES users(id),"
        "    FOREIGN KEY (product_id) REFERENCES products(id)"
        ")"
    );
}

bool DatabaseManager::createSellerTable() {
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS sellers ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id INTEGER NOT NULL,"
        "    business_name TEXT,"
        "    registration_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (user_id) REFERENCES users(id)"
        ")"
    );
}

bool DatabaseManager::addUser(const User& user) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (email, username, password, is_admin, is_suspended) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(user.getEmail());
    query.addBindValue(user.getUsername());
    query.addBindValue(user.getHashedPassword());
    query.addBindValue(user.isAdmin());
    query.addBindValue(user.isSuspended());
    
    if (!query.exec()) {
        qDebug() << "Error adding user:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addProduct(const Product& product) {
    QSqlQuery query;
    query.prepare("INSERT INTO products (name, description, price, seller_id, category, image_data, image_url, stock) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(product.name);
    query.addBindValue(product.description);
    query.addBindValue(product.price);
    query.addBindValue(product.sellerId);
    query.addBindValue(product.category);
    query.addBindValue(product.imageData);
    query.addBindValue(product.imageUrl);
    query.addBindValue(product.stock);
    
    bool success = query.exec();
    if (!success) {
        qDebug() << "Error adding product:" << query.lastError().text();
        qDebug() << "Product details - Name:" << product.name 
                 << "Price:" << product.price 
                 << "SellerId:" << product.sellerId 
                 << "Stock:" << product.stock;
    }
    return success;
}

bool DatabaseManager::updateProductStock(int productId, int newStock) {
    QSqlQuery query;
    query.prepare("UPDATE products SET stock = ? WHERE id = ?");
    query.addBindValue(newStock);
    query.addBindValue(productId);
    return query.exec();
}

bool DatabaseManager::decrementProductStock(int productId, int quantity) {
    QSqlQuery query;
    query.prepare("UPDATE products SET stock = stock - ? WHERE id = ? AND stock >= ?");
    query.addBindValue(quantity);
    query.addBindValue(productId);
    query.addBindValue(quantity);
    return query.exec();
}

Product DatabaseManager::getProductById(int productId) {
    QSqlQuery query;
    query.prepare("SELECT id, name, description, price, seller_id, category, image_data, image_url, stock "
                "FROM products WHERE id = ?");
    query.addBindValue(productId);
    
    Product product;
    if (query.exec()) {
        if (query.next()) {
            product.id = query.value("id").toInt();
            product.name = query.value("name").toString();
            product.description = query.value("description").toString();
            product.price = query.value("price").toDouble();
            product.sellerId = query.value("seller_id").toInt();
            product.category = query.value("category").toString();
            product.imageData = query.value("image_data").toByteArray();
            product.imageUrl = query.value("image_url").toString();
            product.stock = query.value("stock").toInt();
            qDebug() << "Found product - ID:" << product.id 
                     << "Name:" << product.name 
                     << "Stock:" << product.stock;
        } else {
            qDebug() << "No product found with ID:" << productId;
        }
    } else {
        qDebug() << "Error fetching product:" << query.lastError().text();
    }
    return product;
}

QList<Product> DatabaseManager::getAllProducts() {
    QList<Product> products;
    QSqlQuery query("SELECT id, name, description, price, seller_id, category, image_data, image_url, stock FROM products");
    
    while (query.next()) {
        Product product;
        product.id = query.value("id").toInt();
        product.name = query.value("name").toString();
        product.description = query.value("description").toString();
        product.price = query.value("price").toDouble();
        product.sellerId = query.value("seller_id").toInt();
        product.category = query.value("category").toString();
        product.imageData = query.value("image_data").toByteArray();
        product.imageUrl = query.value("image_url").toString();
        product.stock = query.value("stock").toInt();
        products.append(product);
    }
    
    return products;
}

User DatabaseManager::getUserByEmail(const QString& email) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE email = ?");
    query.addBindValue(email);
    
    if (query.exec() && query.next()) {
        return User(query.value("email").toString(),
                   query.value("username").toString(),
                   query.value("password").toString(),
                   query.value("is_admin").toBool(),
                   query.value("is_suspended").toBool());
    }
    return User();
}

User DatabaseManager::getUserByUsername(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return User(query.value("email").toString(),
                   query.value("username").toString(),
                   query.value("password").toString(),
                   query.value("is_admin").toBool(),
                   query.value("is_suspended").toBool());
    }
    return User();
}

bool DatabaseManager::userExists(const QString& email, const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE email = ? OR username = ?");
    query.addBindValue(email);
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

int DatabaseManager::getUserIdByEmail(const QString& email) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE email = ?");
    query.addBindValue(email);
    
    if (query.exec() && query.next()) {
        return query.value("id").toInt();
    }
    
    qDebug() << "Error getting user id by email:" << query.lastError().text();
    return -1;
}

// Cart operations implementation
bool DatabaseManager::addToCart(int userId, int productId, int quantity) {
    // First check if there's enough stock
    Product product = getProductById(productId);
    if (product.id == -1) {
        qDebug() << "Failed to add to cart: Product not found with ID:" << productId;
        return false;
    }
    
    if (product.stock < quantity) {
        qDebug() << "Failed to add to cart: Insufficient stock. Available:" << product.stock << "Requested:" << quantity;
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO cart (user_id, product_id, quantity, price) "
                "VALUES (?, ?, ?, ?)");
    query.addBindValue(userId);
    query.addBindValue(productId);
    query.addBindValue(quantity);
    query.addBindValue(product.price);
    
    bool success = query.exec();
    if (!success) {
        qDebug() << "Failed to add to cart: Database error:" << query.lastError().text();
        qDebug() << "User ID:" << userId << "Product ID:" << productId << "Quantity:" << quantity;
    }
    return success;
}

bool DatabaseManager::updateCartItemQuantity(int cartItemId, int quantity) {
    QSqlQuery query;
    query.prepare("UPDATE cart SET quantity = ? WHERE id = ?");
    query.addBindValue(quantity);
    query.addBindValue(cartItemId);
    
    if (!query.exec()) {
        qDebug() << "Error updating cart item quantity:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeFromCart(int cartItemId) {
    QSqlQuery query;
    query.prepare("DELETE FROM cart WHERE id = ?");
    query.addBindValue(cartItemId);
    
    if (!query.exec()) {
        qDebug() << "Error removing from cart:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<CartItem> DatabaseManager::getCartItems(int userId) {
    QList<CartItem> items;
    QSqlQuery query;
    query.prepare("SELECT * FROM cart WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            CartItem item;
            item.id = query.value("id").toInt();
            item.userId = query.value("user_id").toInt();
            item.productId = query.value("product_id").toInt();
            item.quantity = query.value("quantity").toInt();
            item.price = query.value("price").toDouble();
            items.append(item);
            qDebug() << "Found cart item - ID:" << item.id 
                     << "Product ID:" << item.productId 
                     << "Quantity:" << item.quantity 
                     << "Price:" << item.price;
        }
    } else {
        qDebug() << "Error getting cart items:" << query.lastError().text();
    }
    
    return items;
}

bool DatabaseManager::clearCart(int userId) {
    QSqlQuery query;
    query.prepare("DELETE FROM cart WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Error clearing cart:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createOrder(int userId, const QList<CartItem>& items) {
    QSqlDatabase::database().transaction();
    
    qDebug() << "Creating order for user ID:" << userId << "with" << items.size() << "items";
    
    QSqlQuery query;
    query.prepare("INSERT INTO orders (user_id, order_date, status, total_amount) "
                "VALUES (?, ?, ?, ?)");
    query.addBindValue(userId);
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue("Pending");
    
    double totalAmount = 0;
    for (const CartItem& item : items) {
        totalAmount += item.price * item.quantity;
    }
    query.addBindValue(totalAmount);
    
    if (!query.exec()) {
        qDebug() << "Failed to create order: Database error:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }
    
    int orderId = query.lastInsertId().toInt();
    qDebug() << "Created order with ID:" << orderId << "Total amount:" << totalAmount;
    
    // Add order items and update stock
    for (const CartItem& item : items) {
        // Get product details
        Product product = getProductById(item.productId);
        if (product.id == -1) {
            qDebug() << "Failed to find product with ID:" << item.productId;
            QSqlDatabase::database().rollback();
            return false;
        }
        
        // Update stock
        if (!decrementProductStock(item.productId, item.quantity)) {
            qDebug() << "Failed to decrement stock for product:" << item.productId;
            QSqlDatabase::database().rollback();
            return false;
        }
        
        // Add order item
        query.prepare("INSERT INTO order_items (order_id, product_id, product_name, quantity, price) "
                    "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(orderId);
        query.addBindValue(item.productId);
        query.addBindValue(product.name);  // Add the product name
        query.addBindValue(item.quantity);
        query.addBindValue(item.price);
        
        if (!query.exec()) {
            qDebug() << "Failed to add order item: Database error:" << query.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
        
        qDebug() << "Added order item - Product:" << product.name 
                 << "Quantity:" << item.quantity 
                 << "Price:" << item.price;
    }
    
    // Clear cart
    query.prepare("DELETE FROM cart WHERE user_id = ?");
    query.addBindValue(userId);
    if (!query.exec()) {
        qDebug() << "Failed to clear cart: Database error:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }
    
    bool success = QSqlDatabase::database().commit();
    if (!success) {
        qDebug() << "Failed to commit transaction:" << QSqlDatabase::database().lastError().text();
    } else {
        qDebug() << "Successfully created order and cleared cart";
    }
    return success;
}

QList<Order> DatabaseManager::getUserOrders(int userId) {
    QList<Order> orders;
    QSqlQuery query;
    
    qDebug() << "Fetching orders for user ID:" << userId;
    
    // First check if the user exists
    QSqlQuery userCheck;
    userCheck.prepare("SELECT id FROM users WHERE id = ?");
    userCheck.addBindValue(userId);
    if (!userCheck.exec() || !userCheck.next()) {
        qDebug() << "User not found with ID:" << userId;
        return orders;
    }
    
    // Get all orders for the user
    query.prepare("SELECT o.*, COUNT(oi.id) as item_count "
                 "FROM orders o "
                 "LEFT JOIN order_items oi ON o.id = oi.order_id "
                 "WHERE o.user_id = ? "
                 "GROUP BY o.id "
                 "ORDER BY o.order_date DESC");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "Error fetching orders:" << query.lastError().text();
        return orders;
    }
    
    if (query.size() == 0) {
        qDebug() << "No orders found for user ID:" << userId;
        return orders;
    }
    
    while (query.next()) {
        Order order;
        order.id = query.value("id").toInt();
        order.userId = query.value("user_id").toInt();
        order.orderDate = query.value("order_date").toDateTime();
        order.status = query.value("status").toString();
        order.totalAmount = query.value("total_amount").toDouble();
        int itemCount = query.value("item_count").toInt();
        
        qDebug() << "Found order - ID:" << order.id 
                 << "Date:" << order.orderDate.toString("yyyy-MM-dd hh:mm:ss")
                 << "Status:" << order.status 
                 << "Total:" << order.totalAmount
                 << "Items:" << itemCount;
        
        // Get order items
        QSqlQuery itemsQuery;
        itemsQuery.prepare("SELECT oi.*, p.name as product_name "
                         "FROM order_items oi "
                         "LEFT JOIN products p ON oi.product_id = p.id "
                         "WHERE oi.order_id = ?");
        itemsQuery.addBindValue(order.id);
        
        if (itemsQuery.exec()) {
            while (itemsQuery.next()) {
                OrderItem item;
                item.id = itemsQuery.value("id").toInt();
                item.orderId = itemsQuery.value("order_id").toInt();
                item.productId = itemsQuery.value("product_id").toInt();
                item.productName = itemsQuery.value("product_name").toString();
                item.quantity = itemsQuery.value("quantity").toInt();
                item.price = itemsQuery.value("price").toDouble();
                order.items.append(item);
                
                qDebug() << "Order item - Product:" << item.productName 
                         << "Quantity:" << item.quantity 
                         << "Price:" << item.price
                         << "Total:" << (item.quantity * item.price);
            }
        } else {
            qDebug() << "Error fetching order items:" << itemsQuery.lastError().text();
        }
        
        orders.append(order);
    }
    
    qDebug() << "Total orders found:" << orders.size();
    return orders;
}

QList<Order> DatabaseManager::getUserOrdersByDateRange(int userId, const QDateTime& startDate, const QDateTime& endDate) {
    QList<Order> orders;
    QSqlQuery query;
    query.prepare("SELECT * FROM orders WHERE user_id = ? AND order_date BETWEEN ? AND ? ORDER BY order_date DESC");
    query.addBindValue(userId);
    query.addBindValue(startDate);
    query.addBindValue(endDate);
    
    if (query.exec()) {
        while (query.next()) {
            Order order;
            order.id = query.value("id").toInt();
            order.userId = query.value("user_id").toInt();
            order.orderDate = query.value("order_date").toDateTime();
            order.status = query.value("status").toString();
            order.totalAmount = query.value("total_amount").toDouble();
            orders.append(order);
        }
    }
    
    return orders;
}

QList<Order> DatabaseManager::getUserOrdersByStatus(int userId, const QString& status) {
    QList<Order> orders;
    QSqlQuery query;
    query.prepare("SELECT * FROM orders WHERE user_id = ? AND status = ? ORDER BY order_date DESC");
    query.addBindValue(userId);
    query.addBindValue(status);
    
    if (query.exec()) {
        while (query.next()) {
            Order order;
            order.id = query.value("id").toInt();
            order.userId = query.value("user_id").toInt();
            order.orderDate = query.value("order_date").toDateTime();
            order.status = query.value("status").toString();
            order.totalAmount = query.value("total_amount").toDouble();
            orders.append(order);
        }
    }
    
    return orders;
}

Order DatabaseManager::getOrderById(int orderId) {
    Order order;
    QSqlQuery query;
    query.prepare("SELECT * FROM orders WHERE id = ?");
    query.addBindValue(orderId);
    
    if (query.exec() && query.next()) {
        order.id = query.value("id").toInt();
        order.userId = query.value("user_id").toInt();
        order.orderDate = query.value("order_date").toDateTime();
        order.status = query.value("status").toString();
        order.totalAmount = query.value("total_amount").toDouble();
        
        // Get order items
        QSqlQuery itemsQuery;
        itemsQuery.prepare("SELECT * FROM order_items WHERE order_id = ?");
        itemsQuery.addBindValue(order.id);
        
        if (itemsQuery.exec()) {
            while (itemsQuery.next()) {
                OrderItem item;
                item.id = itemsQuery.value("id").toInt();
                item.orderId = itemsQuery.value("order_id").toInt();
                item.productId = itemsQuery.value("product_id").toInt();
                item.productName = itemsQuery.value("product_name").toString();
                item.quantity = itemsQuery.value("quantity").toInt();
                item.price = itemsQuery.value("price").toDouble();
                order.items.append(item);
            }
        }
    }
    
    return order;
}

bool DatabaseManager::updateOrderStatus(int orderId, const QString& status) {
    QSqlQuery query;
    query.prepare("UPDATE orders SET status = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(orderId);
    
    if (!query.exec()) {
        qDebug() << "Error updating order status:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::addReview(const Review& review) {
    QSqlQuery query;
    query.prepare("INSERT INTO reviews (product_id, user_id, username, rating, comment, review_date) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(review.productId);
    query.addBindValue(review.userId);
    query.addBindValue(review.username);
    query.addBindValue(review.rating);
    query.addBindValue(review.comment);
    query.addBindValue(review.reviewDate);
    
    if (!query.exec()) {
        qDebug() << "Error adding review:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateReview(const Review& review) {
    QSqlQuery query;
    query.prepare("UPDATE reviews SET rating = ?, comment = ? WHERE id = ?");
    query.addBindValue(review.rating);
    query.addBindValue(review.comment);
    query.addBindValue(review.id);
    
    if (!query.exec()) {
        qDebug() << "Error updating review:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteReview(int reviewId) {
    QSqlQuery query;
    query.prepare("DELETE FROM reviews WHERE id = ?");
    query.addBindValue(reviewId);
    
    if (!query.exec()) {
        qDebug() << "Error deleting review:" << query.lastError().text();
        return false;
    }
    return true;
}

Review DatabaseManager::getReviewById(int reviewId) {
    Review review;
    QSqlQuery query;
    query.prepare("SELECT * FROM reviews WHERE id = ?");
    query.addBindValue(reviewId);
    
    if (query.exec() && query.next()) {
        review.id = query.value("id").toInt();
        review.productId = query.value("product_id").toInt();
        review.userId = query.value("user_id").toInt();
        review.username = query.value("username").toString();
        review.rating = query.value("rating").toInt();
        review.comment = query.value("comment").toString();
        review.reviewDate = query.value("review_date").toDateTime();
    }
    
    return review;
}

QList<Review> DatabaseManager::getProductReviews(int productId) {
    QList<Review> reviews;
    QSqlQuery query;
    query.prepare("SELECT * FROM reviews WHERE product_id = ? ORDER BY review_date DESC");
    query.addBindValue(productId);
    
    if (query.exec()) {
        while (query.next()) {
            Review review;
            review.id = query.value("id").toInt();
            review.productId = query.value("product_id").toInt();
            review.userId = query.value("user_id").toInt();
            review.username = query.value("username").toString();
            review.rating = query.value("rating").toInt();
            review.comment = query.value("comment").toString();
            review.reviewDate = query.value("review_date").toDateTime();
            reviews.append(review);
        }
    }
    
    return reviews;
}

Review DatabaseManager::getUserProductReview(int userId, int productId) {
    Review review;
    QSqlQuery query;
    query.prepare("SELECT * FROM reviews WHERE user_id = ? AND product_id = ?");
    query.addBindValue(userId);
    query.addBindValue(productId);
    
    if (query.exec() && query.next()) {
        review.id = query.value("id").toInt();
        review.productId = query.value("product_id").toInt();
        review.userId = query.value("user_id").toInt();
        review.username = query.value("username").toString();
        review.rating = query.value("rating").toInt();
        review.comment = query.value("comment").toString();
        review.reviewDate = query.value("review_date").toDateTime();
    }
    
    return review;
}

double DatabaseManager::getProductAverageRating(int productId) {
    QSqlQuery query;
    query.prepare("SELECT AVG(rating) as avg_rating FROM reviews WHERE product_id = ?");
    query.addBindValue(productId);
    
    if (query.exec() && query.next()) {
        return query.value("avg_rating").toDouble();
    }
    
    return 0.0;
}

bool DatabaseManager::hasUserPurchasedProduct(int userId, int productId) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM orders o "
                 "JOIN order_items oi ON o.id = oi.order_id "
                 "WHERE o.user_id = ? AND oi.product_id = ? "
                 "AND o.status IN ('Delivered', 'Shipped')");
    query.addBindValue(userId);
    query.addBindValue(productId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

bool DatabaseManager::isUserAdmin(int userId) {
    QSqlQuery query(db);
    query.prepare("SELECT is_admin FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    return false;
}

bool DatabaseManager::suspendUser(int userId) {
    QSqlQuery query(db);
    query.prepare("UPDATE users SET is_suspended = true WHERE id = ?");
    query.addBindValue(userId);
    return query.exec();
}

bool DatabaseManager::unsuspendUser(int userId) {
    QSqlQuery query(db);
    query.prepare("UPDATE users SET is_suspended = false WHERE id = ?");
    query.addBindValue(userId);
    return query.exec();
}

bool DatabaseManager::resetUserPassword(int userId, const QString& newHashedPassword) {
    QSqlQuery query(db);
    
    qDebug() << "Attempting to reset password for user ID:" << userId;
    
    // First verify the user exists
    QSqlQuery checkUser;
    checkUser.prepare("SELECT id FROM users WHERE id = ?");
    checkUser.addBindValue(userId);
    if (!checkUser.exec() || !checkUser.next()) {
        qDebug() << "Failed to reset password: User not found with ID:" << userId;
        return false;
    }
    
    // Update the password
    query.prepare("UPDATE users SET password = ? WHERE id = ?");
    query.addBindValue(newHashedPassword);
    query.addBindValue(userId);
    
    bool success = query.exec();
    if (!success) {
        qDebug() << "Failed to reset password: Database error:" << query.lastError().text();
    } else {
        qDebug() << "Successfully reset password for user ID:" << userId;
    }
    return success;
}

bool DatabaseManager::deleteProduct(int productId) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM products WHERE id = ?");
    query.addBindValue(productId);
    return query.exec();
}

QList<User> DatabaseManager::getAllUsers() {
    QList<User> users;
    QSqlQuery query(db);
    
    qDebug() << "Fetching all users";
    
    if (!query.exec("SELECT * FROM users")) {
        qDebug() << "Error fetching users:" << query.lastError().text();
        return users;
    }
    
    while (query.next()) {
        User user;
        user.setEmail(query.value("email").toString());
        user.setUsername(query.value("username").toString());
        user.setHashedPassword(query.value("password").toString());
        user.setAdmin(query.value("is_admin").toBool());
        user.setSuspended(query.value("is_suspended").toBool());
        users.append(user);
        
        qDebug() << "Found user - Email:" << user.getEmail() 
                 << "Username:" << user.getUsername() 
                 << "Admin:" << user.isAdmin() 
                 << "Suspended:" << user.isSuspended();
    }
    
    qDebug() << "Total users found:" << users.size();
    return users;
}

double DatabaseManager::getTotalSales() {
    QSqlQuery query(db);
    
    qDebug() << "Calculating total sales";
    
    if (!query.exec("SELECT COALESCE(SUM(total_amount), 0) as total FROM orders")) {
        qDebug() << "Error calculating total sales:" << query.lastError().text();
        return 0.0;
    }
    
    if (query.next()) {
        double total = query.value("total").toDouble();
        qDebug() << "Total sales:" << total;
        return total;
    }
    
    qDebug() << "No sales data found";
    return 0.0;
}

int DatabaseManager::getTotalOrders() {
    QSqlQuery query(db);
    
    qDebug() << "Counting total orders";
    
    if (!query.exec("SELECT COUNT(*) as count FROM orders")) {
        qDebug() << "Error counting orders:" << query.lastError().text();
        return 0;
    }
    
    if (query.next()) {
        int count = query.value("count").toInt();
        qDebug() << "Total orders count:" << count;
        return count;
    }
    
    qDebug() << "No orders found";
    return 0;
}

double DatabaseManager::getAverageOrderValue() {
    QSqlQuery query(db);
    query.exec("SELECT COALESCE(AVG(total_amount), 0) FROM orders");
    if (query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

User DatabaseManager::getUserById(int userId) {
    QSqlQuery query;
    query.prepare("SELECT email, username, password, is_admin, is_suspended FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    User user;
    if (query.exec() && query.next()) {
        user.setEmail(query.value("email").toString());
        user.setUsername(query.value("username").toString());
        user.setHashedPassword(query.value("password").toString());
        user.setAdmin(query.value("is_admin").toBool());
        user.setSuspended(query.value("is_suspended").toBool());
    }
    return user;
}

bool DatabaseManager::createUser(const QString& email, const QString& username, const QString& password, bool isAdmin, bool isSeller) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO users (email, username, password, is_admin, is_seller, created_at) "
        "VALUES (:email, :username, :password, :is_admin, :is_seller, CURRENT_TIMESTAMP)"
    );
    query.bindValue(":email", email);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":is_admin", isAdmin);
    query.bindValue(":is_seller", isSeller);

    if (!query.exec()) {
        qDebug() << "Failed to create user:" << query.lastError().text();
        return false;
    }

    // If user is a seller, create seller record
    if (isSeller) {
        QSqlQuery sellerQuery;
        sellerQuery.prepare(
            "INSERT INTO sellers (user_id, business_name) "
            "VALUES ((SELECT id FROM users WHERE email = :email), :business_name)"
        );
        sellerQuery.bindValue(":email", email);
        sellerQuery.bindValue(":business_name", username + "'s Store"); // Default business name
        
        if (!sellerQuery.exec()) {
            qDebug() << "Failed to create seller record:" << sellerQuery.lastError().text();
            return false;
        }
    }

    return true;
}

bool DatabaseManager::isUserSeller(const QString& email) {
    QSqlQuery query;
    query.prepare("SELECT is_seller FROM users WHERE email = :email");
    query.bindValue(":email", email);
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    return false;
}

bool DatabaseManager::updateUserRole(int userId, bool isAdmin, bool isSeller) {
    QSqlQuery query;
    query.prepare(
        "UPDATE users SET is_admin = :is_admin, is_seller = :is_seller "
        "WHERE id = :user_id"
    );
    query.bindValue(":is_admin", isAdmin);
    query.bindValue(":is_seller", isSeller);
    query.bindValue(":user_id", userId);
    
    return query.exec();
} 