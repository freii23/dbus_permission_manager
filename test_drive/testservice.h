#ifndef TESTSERVICE_H
#define TESTSERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QtDBus>
#include "../service_names.h"

class TestService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", CLIENT_NAME)
public:
    explicit TestService(QObject* parent = nullptr);
    ~TestService() override;
};
#endif // TESTSERVICE_H
