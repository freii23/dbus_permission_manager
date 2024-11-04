#include <QCoreApplication>
#include "testservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusAbstractAdaptor::QObject parent_object;
    TestService p(&parent_object);

    // подрубаем интерфес сервиса времени
    QDBusInterface interface_time(TIME_NAME,
                             "/",
                             TIME_NAME,
                             QDBusConnection::sessionBus());

    if (!interface_time.isValid()) {
        qDebug() << "test: Не удалось создать интерфейс для com.system.timeservice:" << interface_time.lastError().message();
        return -1;
    }

    // первая попытка запроса времени
    QDBusReply<quint64> reply1 = interface_time.call("GetSystemTime");
    if (reply1.isValid()) {
        qDebug() << "1 Ответ от GetSystemTime:" << reply1.value();
    } else {
        qDebug() << "Ошибка при вызове метода GetSystemTime:" << reply1.error().message();
    }

    // подрубаем интерфес сервиса разрешений
    QDBusInterface interface_perm(PERM_NAME,
                             "/",
                             PERM_NAME,
                             QDBusConnection::sessionBus());

    if (!interface_time.isValid()) {
        qDebug() << "test: Не удалось создать интерфейс для com.system.permissionsservicetimeservice:" << interface_time.lastError().message();
        return -1;
    }

    // делаем реквест
    QDBusReply<void> reply2 = interface_perm.call("RequestPermission", 0);
    if (reply2.isValid()) {
        qDebug() << "Ответ от RequestPermission: ok";
    } else {
        qDebug() << "Ошибка при вызове метода RequestPermission:" << reply2.error().message();
    }

    // вторая попытка запроса времени
    QDBusReply<quint64> reply3 = interface_time.call("GetSystemTime");
    if (reply3.isValid()) {
        qDebug() << "2 Ответ от GetSystemTime:" << reply3.value();
    } else {
        qDebug() << "Ошибка при вызове метода GetSystemTime:" << reply3.error().message();
    }

    return a.exec();
}
