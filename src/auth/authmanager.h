#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QObject>
#include "../database/databasemanager.h"
#include "sessionmanager.h"
#include "user.h"

class AuthManager : public QObject {
    Q_OBJECT
    
public:
    static AuthManager& getInstance();
    
    bool registerUser(const QString& email, const QString& username, const QString& password);
    bool login(const QString& email, const QString& password);
    void logout();
    bool isAuthenticated() const;
    QString getCurrentUserEmail() const;
    QString getCurrentUserUsername() const;
    QString getCurrentSessionToken() const;
    void setSessionToken(const QString& token);
    int getCurrentUserId() const;
    
signals:
    void loginSuccess();
    void loginError(const QString& message);
    void loginFailed(const QString& message);
    void registrationSuccess();
    void registrationFailed(const QString& message);
    void logoutSuccess();
    
private:
    AuthManager();
    ~AuthManager();
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;
    
    bool validateEmail(const QString& email);
    bool validatePassword(const QString& password);
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& hashedPassword);
    
    bool authenticated;
    QString currentUserEmail;
    QString currentUserUsername;
    QString currentSessionToken;
    int currentUserId;
    User currentUser;
    DatabaseManager& dbManager;
    SessionManager& sessionManager;
};

#endif // AUTHMANAGER_H 