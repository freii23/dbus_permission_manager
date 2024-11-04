#include "permissionsservice.h"

PermissionsService::PermissionsService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // bus connection
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.interface()->isServiceRegistered(QStringLiteral("com.system.permissionsservice"))) {
        bus.registerService(QStringLiteral("com.system.permissionsservice"));
        bus.registerObject(QStringLiteral("/"), "com.system.permissionsservice", parent,
                           QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSlots);
        qDebug() << "new permission service registered";
    }
    else {
        qDebug() << "permission service is taken";
    }

    // db connection
    if (!this->db.open()) {
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

PermissionsService::~PermissionsService() {
//    QDBusConnection bus = QDBusConnection::sessionBus();
//    bus.unregisterObject("com.system.permissionsservice");
//    bus.unregisterService("com.system.permissionsservice");
}

void PermissionsService::RequestPermission(int permissionEnumCode)
{
    QString path = this->GetExecPath();
    QSqlQuery *query = this->executeQuery("INSERT INTO logs"
                                    "(filepath, perm_code)"
                                    "VALUES('"+path+"', "+permissionEnumCode+");");
    if (!this->CheckQueryResult(query)) {
        return;
    }
    query->finish();
    delete query;
}

bool PermissionsService::CheckApplicationHasPermission(const QString &applicationExecPath, int permissionEnumCode)
{
    QSqlQuery *query = this->executeQuery("select exists (select 1 from logs "
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

QString PermissionsService::GetExecPath()
{
    QString path;
    QDBusReply<uint> reply = QDBusConnection::sessionBus().interface()->call("GetConnectionUnixProcessID", SERVICE_NAME);
    if (!reply.isValid()) {
        qDebug() << "Не удалось получить PID для сервиса:" << reply.error().message();
        return path;
    }

    uint pid = reply.value();
    qDebug() << "PID клиента:" << pid;

    // Читаем символьную ссылку /proc/<pid>/exe, чтобы получить путь к исполняемому файлу
    QString exePath = QString("/usr/bin/%1").arg(pid);
    QFileInfo fileInfo(exePath);
    if (fileInfo.exists() && fileInfo.isSymLink()) {
        path = fileInfo.symLinkTarget();
    }
    return path;
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
