#include <QCoreApplication>
#include <QtDBus>

const QString permission_name = "com.system.permissionsservice";
const QString time_name = "com.system.timeservice";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // подрубаем интерфес сервиса разрешений
    QDBusInterface interface_time(time_name,
                             "/",
                             time_name,
                             QDBusConnection::sessionBus());

    if (!interface_time.isValid()) {
        qDebug() << "test: Не удалось создать интерфейс для com.system.timeservice:" << interface_time.lastError().message();
        return -1;
    }

    QDBusReply<quint64> reply = interface_time.call("GetSystemTime");
    if (reply.isValid()) {
        qDebug() << "Ответ от GetSystemTime:" << reply.value();
    } else {
        qDebug() << "Ошибка при вызове метода GetSystemTime:" << reply.error().message();
    }

    return a.exec();
}
