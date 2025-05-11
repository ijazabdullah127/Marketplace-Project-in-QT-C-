#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
#include <QDateTime>
#include <QObject>

class SessionManager : public QObject {
    Q_OBJECT
    
public:
    static SessionManager& getInstance();
    
    QString createSession(const QString& userId);
    bool validateSession(const QString& token);
    void invalidateSession(const QString& token);
    QString getUserIdFromToken(const QString& token);
    
private:
    SessionManager();
    ~SessionManager();
    
    struct Session {
        QString userId;
        QDateTime expirationTime;
    };
    
    QMap<QString, Session> activeSessions;
    static const int SESSION_DURATION_HOURS = 24;
};

#endif // SESSIONMANAGER_H 