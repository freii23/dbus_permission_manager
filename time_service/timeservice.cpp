#include "timeservice.h"

TimeService::TimeService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // bus connection
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.interface()->isServiceRegistered(QStringLiteral(TIME_NAME))) {
        bus.registerService(QStringLiteral(TIME_NAME));
        bus.registerObject(QStringLiteral("/"), QStringLiteral(TIME_NAME), parent,
                           QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSlots);
        qDebug() << "new time service registered";
    }
    else {
        qDebug() << "time service is taken";
    }
}

TimeService::~TimeService()
{

}

quint64 TimeService::GetSystemTime()
{
    quint64 res = 0;
    // Создаем интерфейс для вызова метода на permissions_service
    QDBusInterface interface(PERM_NAME,
                             "/",
                             PERM_NAME,
                             QDBusConnection::sessionBus());

    if (!interface.isValid()) {
        qDebug() << "Не удалось создать интерфейс для com.system.permissionsservice:" << interface.lastError().message();
        return 1;
    }

    QString path;
    // Вызываем метод getExecPath
    QDBusReply<QString> reply1 = interface.call("GetExecPath", CLIENT_NAME);
    if (reply1.isValid()) {
        path = reply1.value();
        qDebug() << "Ответ от GetExecPath:" << path;
    } else {
        qDebug() << "Ошибка при вызове метода GetExecPath:" << reply1.error().message();
        return 1;
    }

    // Вызываем метод check и передаем ему аргументы
    QDBusReply<bool> reply2 = interface.call("CheckApplicationHasPermission", path, int(0));
    if (reply2.isValid()) {
        qDebug() << "Ответ от CheckApplicationHasPermission:" << reply2.value();
        if (reply2.value()) {
            res = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toULongLong();
            qDebug() << "res = " << res;
        }
        else {
            qDebug() << "UnauthorizedAccess";
            res = 2;
        }
    } else {
        qDebug() << "Ошибка при вызове метода CheckApplicationHasPermission:" << reply2.error().message();
    }
    return res;
}
