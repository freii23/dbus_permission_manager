#include "permissionsservice.h"

PermissionsService::PermissionsService(QObject *parent)
    : QDBusAbstractAdaptor{parent}
{
    // getting bus and interface
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return;
    }
//  возможно нужен QDBusConnectionInterface
    QDBusConnectionInterface *iface = bus.interface();

    if (!iface->isValid()) {
        qDebug() << "Invalid interface" << iface->lastError();
        return;
    }

    this->interface = iface;

    // db connection
    if (!this->db.open())
    {
        this->db = QSqlDatabase::addDatabase("QSQLITE", "dbcon");
        this->db.setDatabaseName("test_case_omp_db");
        this->db.setHostName("127.0.0.1");
//        this->db.setUserName("");
//        this->db.setPassword(pass());
//        this->db.setPort(port());
        this->db.setConnectOptions("connect_timeout=30");
    }
    else
    {
        qDebug() << "Используется уже установленное соединение с БД";
    }

    if (!this->db.open())
    {
        qWarning() << "Не удалось открыть соединение с БД (dbcon)";
        return;
    }
}


void PermissionsService::RequestPermission(int permissionEnumCode)
{
    QDBusReply<uint> reply = this->interface->call("GetConnectionUnixProcessID", SERVICE_NAME);
    if (!reply.isValid()) {
        qDebug() << "Не удалось получить PID для сервиса:" << reply.error().message();
        return;
    }

    uint pid = reply.value();
    qDebug() << "PID клиента:" << pid;

    // Читаем символьную ссылку /proc/<pid>/exe, чтобы получить путь к исполняемому файлу
    QString exePath = QString("/usr/bin/%1").arg(pid);
    QFileInfo fileInfo(exePath);

    if (fileInfo.exists() && fileInfo.isSymLink()) {
        QString path = fileInfo.symLinkTarget();
        QSqlQuery *query = this->executeQuery("INSERT INTO logs"
                                        "(filepath, perm_code)"
                                        "VALUES('"+path+"', "+permissionEnumCode+");");
        if (!this->CheckQueryResult(query)) {
            return;
        }
        query->finish();
        delete query;
    }
}

bool PermissionsService::CheckApplicationHasPermission(const QString &applicationExecPath, int permissionEnumCode)
{
    QSqlQuery *query = this->executeQuery("select exists (select 1 from permissions "
                                    "where filepath ='"+applicationExecPath+"' and"
                                    "permission_code ="+permissionEnumCode+");");
    if (!this->CheckQueryResult(query)) {
        return false;
    }
    query->first();
    bool out = query->value(0).toBool();
    query->finish();
    delete query;
    return out;
}

QSqlQuery *PermissionsService::executeQuery(const QString &queryString)
{
    QSqlQuery* query = new QSqlQuery(this->db);

    if (!query->prepare(queryString)) {
        qWarning() << "dbcon: Ошибка при подготовке запроса:" << query->lastError().text();
        delete query;
        return nullptr;
    }

    if (!query->exec()) {
        qWarning() << "dbcon: Ошибка при выполнении запроса:" << query->lastError().text();
        delete query;
        return nullptr;
    }

    return query;
}

bool PermissionsService::CheckQueryResult(QSqlQuery *query)
{
    if (query == nullptr || !query->isActive()) {
        qDebug() << "Ошибка подключения к базе данных";
        if (query != nullptr) {
            query->finish();
            delete query;
        }
        return false;
    }
    return true;
}
