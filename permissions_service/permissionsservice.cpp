#include "permissionsservice.h"

PermissionsService::PermissionsService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // bus connection
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.interface()->isServiceRegistered(QStringLiteral(PERM_NAME))) {
        bus.registerService(QStringLiteral(PERM_NAME));
        bus.registerObject(QStringLiteral("/"), QStringLiteral(PERM_NAME), parent,
                           QDBusConnection::ExportAdaptors | QDBusConnection::ExportAllSlots);
        qDebug() << "new permission service registered";
    }
    else {
        qDebug() << "permission service is taken";
    }

    // db connection
    if (!this->db.open()) {
        this->db = QSqlDatabase::addDatabase("QSQLITE", "dbcon");
        this->db.setDatabaseName("/home/sandor/prog/test_case_omp/test_case_omp_db.db");
//        this->db.setHostName("127.0.0.1");
//        this->db.setUserName();
//        this->db.setPassword();
//        this->db.setPort();
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
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.unregisterObject(QStringLiteral("/"));
    bus.unregisterService(QStringLiteral(PERM_NAME));
}

void PermissionsService::RequestPermission(int permissionEnumCode)
{
    QString path = this->GetExecPath(QStringLiteral(CLIENT_NAME));

    QString queryStr = "INSERT INTO logs ";
    queryStr += "(filepath, perm_code) ";
    queryStr += "VALUES('";
    queryStr += path;
    queryStr += "', ";
    queryStr += QString::number(permissionEnumCode);
    queryStr += ");";

    QSqlQuery *query = this->executeQuery(queryStr);
    if (!this->CheckQueryResult(query)) {
        return;
    }
    query->finish();
    delete query;
}

bool PermissionsService::CheckApplicationHasPermission(const QString &applicationExecPath, int permissionEnumCode)
{
    QString queryStr = "select exists (select 1 from logs ";
    queryStr += "where filepath = '";
    queryStr += applicationExecPath;
    queryStr += "' and perm_code = ";
    queryStr += QString::number(permissionEnumCode);
    queryStr += ");";

    QSqlQuery *query = this->executeQuery(queryStr);
    if (!this->CheckQueryResult(query)) {
        return false;
    }
    query->first();
    bool out = query->value(0).toBool();
    query->finish();
    delete query;
    return out;
}

QString PermissionsService::GetExecPath(const QString& client_path)
{
    QDBusReply<uint> reply = QDBusConnection::sessionBus().interface()->call("GetConnectionUnixProcessID", client_path);
    if (!reply.isValid()) {
        qDebug() << "Не удалось получить PID для сервиса:" << reply.error().message();
        return QString();
    }

    uint pid = reply.value();

    // ищем путь по pid с помощью консольной команды
    QProcess process;
    process.setProgram("ps");
    process.setArguments({"-fp", QString::number(pid)});
    process.start();
    process.waitForFinished();

    QString output = process.readAllStandardOutput();

    // Используем регулярное выражение для извлечения пути исполняемого файла
    QRegularExpression regex(R"(\s+(\S+)$)");
    QRegularExpressionMatch match = regex.match(output);

    if (match.hasMatch()) {
        QString executablePath = match.captured(1);
        return executablePath;
    }

    return QString();
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
