#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User();
    User(const QString& email, const QString& username, const QString& hashedPassword, bool isAdmin = false, bool isSuspended = false);
    
    QString getEmail() const;
    QString getUsername() const;
    QString getHashedPassword() const;
    bool isAdmin() const;
    bool isSuspended() const;
    
    void setEmail(const QString& email);
    void setUsername(const QString& username);
    void setHashedPassword(const QString& hashedPassword);
    void setAdmin(bool isAdmin);
    void setSuspended(bool isSuspended);
    
private:
    QString email;
    QString username;
    QString hashedPassword;
    bool admin;
    bool suspended;
};

#endif // USER_H 