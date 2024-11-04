#include <QCoreApplication>
#include <timeservice.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusAbstractAdaptor::QObject parent_object;
    TimeService p(&parent_object);

    return a.exec();
}
