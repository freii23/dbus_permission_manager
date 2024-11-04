#ifndef PERMISSIONSSERVICE_H
#define PERMISSIONSSERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDBus>
#include <QProcess>

#include "../service_names.h"

enum Permissions {SystemTime = 0};

class PermissionsService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", PERM_NAME)
public:
    explicit PermissionsService(QObject* parent = nullptr);
    ~PermissionsService() override;

public slots:
    Q_INVOKABLE void RequestPermission(int);
    Q_INVOKABLE bool CheckApplicationHasPermission(const QString&, int);
    Q_INVOKABLE QString GetExecPath(const QString&);
private:
    QSqlDatabase db;

    QSqlQuery* executeQuery(const QString &);
    bool CheckQueryResult(QSqlQuery*);
};

#endif // PERMISSIONSSERVICE_H
