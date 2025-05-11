#include "authmanager.h"
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QDebug>
#include <QRandomGenerator>

AuthManager::AuthManager() 
    : authenticated(false)
    , currentUserId(-1)
    , dbManager(DatabaseManager::getInstance())
    , sessionManager(SessionManager::getInstance())
{
    if (!dbManager.initialize()) {
        qDebug() << "Failed to initialize database";
    }
}

AuthManager::~AuthManager() {}

AuthManager& AuthManager::getInstance() {
    static AuthManager instance;
    return instance;
}

bool AuthManager::validateEmail(const QString& email) {
    QRegularExpression emailRegex("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}\\b");
    return emailRegex.match(email).hasMatch();
}

bool AuthManager::validatePassword(const QString& password) {
    // Password must be at least 8 characters long and contain at least one number
    return password.length() >= 8 && password.contains(QRegularExpression("[0-9]"));
}

QString AuthManager::hashPassword(const QString& password) {
    // Generate a random salt using QRandomGenerator
    QByteArray salt = QCryptographicHash::hash(
        QByteArray::number(QRandomGenerator::global()->generate64()),
        QCryptographicHash::Sha256
    ).toHex();
    
    // Hash password with salt
    QByteArray hashedPassword = QCryptographicHash::hash(
        (password + salt).toUtf8(),
        QCryptographicHash::Sha256
    ).toHex();
    
    // Return combined hash and salt
    return QString(hashedPassword + salt);
}

bool AuthManager::verifyPassword(const QString& password, const QString& hashedPassword) {
    // Extract salt from stored hash
    QByteArray salt = hashedPassword.mid(64).toUtf8();
    
    // Hash input password with same salt
    QByteArray computedHash = QCryptographicHash::hash(
        (password + QString(salt)).toUtf8(),
        QCryptographicHash::Sha256
    ).toHex();
    
    // Compare hashes
    return computedHash == hashedPassword.left(64);
}

bool AuthManager::registerUser(const QString& email, const QString& username, const QString& password) {
    // Validate email
    if (!validateEmail(email)) {
        emit registrationFailed("Invalid email format");
        return false;
    }
    
    // Validate password
    if (!validatePassword(password)) {
        emit registrationFailed("Password must be at least 8 characters long and contain at least one number");
        return false;
    }
    
    // Check if user exists
    if (dbManager.userExists(email, username)) {
        emit registrationFailed("Email or username already exists");
        return false;
    }
    
    // Hash password and create user
    QString hashedPassword = hashPassword(password);
    User user(email, username, hashedPassword);
    
    if (dbManager.addUser(user)) {
        emit registrationSuccess();
        return true;
    } else {
        emit registrationFailed("Failed to create user");
        return false;
    }
}

bool AuthManager::login(const QString& email, const QString& password) {
    qDebug() << "Attempting login for email:" << email;
    
    User user = dbManager.getUserByEmail(email);
    
    if (user.getEmail().isEmpty()) {
        qDebug() << "Login failed: User not found with email:" << email;
        emit loginFailed("Invalid email or password");
        return false;
    }
    
    if (user.isSuspended()) {
        qDebug() << "Login failed: Account is suspended for email:" << email;
        emit loginError("This account has been suspended. Please contact an administrator.");
        return false;
    }
    
    if (verifyPassword(password, user.getHashedPassword())) {
        authenticated = true;
        currentUser = user;
        currentUserEmail = user.getEmail();
        currentUserUsername = user.getUsername();
        currentUserId = dbManager.getUserIdByEmail(email);
        
        // Create session
        currentSessionToken = sessionManager.createSession(user.getEmail());
        
        qDebug() << "Login successful for user:" << email 
                 << "ID:" << currentUserId 
                 << "Admin:" << user.isAdmin();
        
        emit loginSuccess();
        return true;
    }
    
    qDebug() << "Login failed: Invalid password for email:" << email;
    emit loginFailed("Invalid email or password");
    return false;
}

void AuthManager::logout() {
    if (!currentSessionToken.isEmpty()) {
        sessionManager.invalidateSession(currentSessionToken);
    }
    
    authenticated = false;
    currentUserEmail.clear();
    currentUserUsername.clear();
    currentSessionToken.clear();
    currentUserId = -1;
    
    qDebug() << "User logged out";
    emit logoutSuccess();
}

bool AuthManager::isAuthenticated() const {
    if (!authenticated || currentSessionToken.isEmpty() || currentUserId == -1) {
        return false;
    }
    
    return sessionManager.validateSession(currentSessionToken);
}

QString AuthManager::getCurrentUserEmail() const {
    return currentUserEmail;
}

QString AuthManager::getCurrentUserUsername() const {
    return currentUserUsername;
}

QString AuthManager::getCurrentSessionToken() const {
    return currentSessionToken;
}

void AuthManager::setSessionToken(const QString& token) {
    if (sessionManager.validateSession(token)) {
        currentSessionToken = token;
        authenticated = true;
        QString email = sessionManager.getUserIdFromToken(token);
        User user = dbManager.getUserByEmail(email);
        currentUserEmail = user.getEmail();
        currentUserUsername = user.getUsername();
        currentUserId = dbManager.getUserIdByEmail(email);
        qDebug() << "Session restored - User ID:" << currentUserId;
    }
}

int AuthManager::getCurrentUserId() const {
    if (!authenticated || currentUserId == -1) {
        qDebug() << "User not authenticated or invalid ID";
        return -1;
    }
    qDebug() << "Current user ID:" << currentUserId;
    return currentUserId;
}
