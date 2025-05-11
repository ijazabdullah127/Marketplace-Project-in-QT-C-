#include "user.h"

User::User() : email(""), username(""), hashedPassword(""), admin(false), suspended(false) {}

User::User(const QString& email, const QString& username, const QString& hashedPassword, bool isAdmin, bool isSuspended)
    : email(email), username(username), hashedPassword(hashedPassword), admin(isAdmin), suspended(isSuspended) {}

QString User::getEmail() const {
    return email;
}

QString User::getUsername() const {
    return username;
}

QString User::getHashedPassword() const {
    return hashedPassword;
}

bool User::isAdmin() const {
    return admin;
}

bool User::isSuspended() const {
    return suspended;
}

void User::setEmail(const QString& email) {
    this->email = email;
}

void User::setUsername(const QString& username) {
    this->username = username;
}

void User::setHashedPassword(const QString& hashedPassword) {
    this->hashedPassword = hashedPassword;
}

void User::setAdmin(bool isAdmin) {
    this->admin = isAdmin;
}

void User::setSuspended(bool isSuspended) {
    this->suspended = isSuspended;
} 