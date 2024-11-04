#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <QObject>
#include <QtDBus>
#include "../service_names.h"

class TimeService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", TIME_NAME)
public:
    explicit TimeService(QObject* parent = nullptr);
    ~TimeService() override;

public slots:
    Q_INVOKABLE quint64 GetSystemTime();
};

#endif // TIMESERVICE_H
