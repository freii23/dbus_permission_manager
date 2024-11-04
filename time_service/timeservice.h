#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#define SERVICE_NAME "com.system.timeservice"

#include <QObject>
#include <QtDBus>
//#include <../permissions_service/permissionsservice.h>

class TimeService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)
public:
    explicit TimeService(QObject* parent = nullptr);
    ~TimeService() override;

public slots:
    Q_INVOKABLE quint64 GetSystemTime();
};

#endif // TIMESERVICE_H
