#ifndef PERMISSIONSSERVICE_H
#define PERMISSIONSSERVICE_H

#define SERVICE_NAME "com.system.permissions"

#include <stdio.h>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDBus>


enum Permissions {SystemTime = 0};

class PermissionsService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)
public:
    explicit PermissionsService(QObject* parent);

public slots:
    void RequestPermission(int);
    bool CheckApplicationHasPermission(const QString&, int);

private:
    QSqlDatabase db;
    //  возможно нужен QDBusConnectionInterface
    QDBusConnectionInterface *interface = nullptr;

    QSqlQuery* executeQuery(const QString &);
    bool CheckQueryResult(QSqlQuery*);
};

#endif // PERMISSIONSSERVICE_H
