#ifndef PROTECTEDPAGE_H
#define PROTECTEDPAGE_H

#include <QWidget>
#include "../auth/authmanager.h"

class ProtectedPage : public QWidget {
    Q_OBJECT

public:
    explicit ProtectedPage(QWidget *parent = nullptr);
    virtual ~ProtectedPage() = default;

signals:
    void accessDenied();
    void loginRequired();
    void navigateHome();
    void logout();

protected:
    bool checkAccess();
    AuthManager& authManager;
    virtual void showEvent(QShowEvent* event) override;
};

#endif // PROTECTEDPAGE_H 