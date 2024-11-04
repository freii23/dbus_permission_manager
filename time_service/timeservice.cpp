#include "timeservice.h"

TimeService::TimeService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // bus connection
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.interface()->isServiceRegistered(QStringLiteral("com.system.timeservice"))) {
        bus.registerService(QStringLiteral("com.system.timeservice"));
        bus.registerObject(QStringLiteral("/"), "com.system.timeservice", parent,
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
    QDBusInterface interface("com.system.permissionsservice",
                             "/",
                             "com.system.permissionsservice",
                             QDBusConnection::sessionBus());

    if (!interface.isValid()) {
        qDebug() << "Не удалось создать интерфейс для com.system.permissionsservice:" << interface.lastError().message();
        return -1;
    }

    QString path;
    // Вызываем метод getExecPath
    QDBusReply<QString> reply1 = interface.call("GetExecPath");
    if (reply1.isValid()) {
        path = reply1.value();
        qDebug() << "Ответ от GetExecPath:" << path;
    } else {
        qDebug() << "Ошибка при вызове метода GetExecPath:" << reply1.error().message();
        return -1;
    }

    // Вызываем метод check и передаем ему аргументы
    QDBusReply<bool> reply2 = interface.call("CheckApplicationHasPermission", path, 0);
    if (reply2.isValid()) {
        qDebug() << "Ответ от CheckApplicationHasPermission:" << reply2.value();
        if (reply2.value()) {
            res = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toUInt();
        }
        else {
            qDebug() << "UnauthorizedAccess";
            res = -2;
        }
    } else {
        qDebug() << "Ошибка при вызове метода CheckApplicationHasPermission:" << reply2.error().message();
    }
    return res;
}
