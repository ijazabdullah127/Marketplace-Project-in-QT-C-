#include "protectedpage.h"
#include <QShowEvent>

ProtectedPage::ProtectedPage(QWidget *parent)
    : QWidget(parent)
    , authManager(AuthManager::getInstance())
{
}

bool ProtectedPage::checkAccess()
{
    if (!authManager.isAuthenticated()) {
        emit accessDenied();
        return false;
    }
    return true;
}

void ProtectedPage::showEvent(QShowEvent* event)
{
    if (!checkAccess()) {
        event->ignore();
        return;
    }
    QWidget::showEvent(event);
} 