#include "permissionsservice.h"
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QDBusAbstractAdaptor::QObject parent_object;
  PermissionsService p(&parent_object);

  return a.exec();
}
