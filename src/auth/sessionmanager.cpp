#include "sessionmanager.h"
#include <QCryptographicHash>
#include <QUuid>
#include <QDebug>

SessionManager::SessionManager() {}

SessionManager::~SessionManager() {}

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

QString SessionManager::createSession(const QString& userId) {
    // Generate a unique token
    QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // Create session with expiration time
    Session session;
    session.userId = userId;
    session.expirationTime = QDateTime::currentDateTime().addSecs(SESSION_DURATION_HOURS * 3600);
    
    // Store session
    activeSessions[token] = session;
    
    return token;
}

bool SessionManager::validateSession(const QString& token) {
    if (!activeSessions.contains(token)) {
        return false;
    }
    
    Session& session = activeSessions[token];
    
    // Check if session has expired
    if (QDateTime::currentDateTime() > session.expirationTime) {
        activeSessions.remove(token);
        return false;
    }
    
    return true;
}

void SessionManager::invalidateSession(const QString& token) {
    activeSessions.remove(token);
}

QString SessionManager::getUserIdFromToken(const QString& token) {
    if (activeSessions.contains(token)) {
        return activeSessions[token].userId;
    }
    return QString();
} 