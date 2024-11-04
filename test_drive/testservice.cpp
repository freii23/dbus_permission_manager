#include "testservice.h"

TestService::TestService(QObject *parent) : QDBusAbstractAdaptor(parent) {
  // bus connection
  QDBusConnection bus = QDBusConnection::sessionBus();

  if (!bus.interface()->isServiceRegistered(QStringLiteral(CLIENT_NAME))) {
    bus.registerService(QStringLiteral(CLIENT_NAME));
    bus.registerObject(QStringLiteral("/"), QStringLiteral(CLIENT_NAME), parent,
                       QDBusConnection::ExportAdaptors |
                           QDBusConnection::ExportAllSlots);
    qDebug() << "test service registered";
  } else {
    qDebug() << "test service is taken";
  }
}

TestService::~TestService() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  bus.unregisterObject(QStringLiteral("/"));
  bus.unregisterService(QStringLiteral(CLIENT_NAME));
}
